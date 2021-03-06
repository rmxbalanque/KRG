#include "ResourceServer.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "Tools/Resource/Compilers/ResourceCompilerModule.h"
#include "System/Resource/ResourceProviders/ResourceNetworkMessages.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Settings/ConfigSettings.h"

#include "System/Core/ThirdParty/cereal/archives/json.hpp"

#include <sstream>

//-------------------------------------------------------------------------
namespace KRG
{
    namespace Resource
    {
        namespace Settings
        {
            static ConfigSettingString  g_sourceDataPath( "SourceDataPath", "Paths" );
            static ConfigSettingString  g_compiledDataPath( "CompiledDataPath", "Paths" );
            static ConfigSettingString  g_resourceServerExecutablePath( "ResourceServerWorkerExecutablePath", "Resource" );
            static ConfigSettingString  g_compiledResourceDatabasePath( "CompiledDataDatabaseName", "Resource" );
            static ConfigSettingInt     g_resourceServerPort( "ResourceServerPort", "Resource" );
        }

        //-------------------------------------------------------------------------

        ResourceServer::ResourceServer()
        {
            m_activeRequests.reserve( 100 );
        }

        bool ResourceServer::Initialize()
        {
            m_workingDir = FileSystem::GetCurrentProcessPath();

            // Load settings
            //-------------------------------------------------------------------------

            FileSystemPath const iniPath = m_workingDir + "KRG.ini";
            if( !m_settingsRegistry.LoadFromFile( iniPath ) )
            {
                m_errorMessage = "Failed to load settings from INI file";
                return false;
            }

            m_sourceDataDir = m_workingDir + Settings::g_sourceDataPath;
            m_compiledDataDir = m_workingDir + Settings::g_compiledDataPath;
            m_compiledResourceDatabasePath = m_workingDir + Settings::g_compiledResourceDatabasePath;
            m_workerFullPath = FileSystemPath( m_workingDir + Settings::g_resourceServerExecutablePath ).GetFullPath().c_str();

            // Generate any additional required settings from read data
            m_networkAddress = String().sprintf( "localhost:%d", (int32) Settings::g_resourceServerPort );

            // Connect to compiled resource database
            //-------------------------------------------------------------------------

            if ( !m_compiledResourceDatabase.TryConnect( m_compiledResourceDatabasePath ) )
            {
                m_errorMessage = String().sprintf( "Database connection error: %s", m_compiledResourceDatabase.GetError().c_str() );
                return false;
            }

            // Read all compiler modules
            //-------------------------------------------------------------------------

            TVector<FileSystemPath> compilerModules;
            FileSystem::GetDirectoryContents( m_workingDir, compilerModules, "KRG.ResourceCompilers.*.dll" );

            for ( auto compilerModule : compilerModules )
            {
                HINSTANCE moduleDLL = LoadLibraryA( compilerModule.GetFullPath().c_str() );
                if ( moduleDLL != nullptr )
                {
                    GetResourceCompilerModuleFuncPtr pModuleFactoryFunc = ( GetResourceCompilerModuleFuncPtr) GetProcAddress( moduleDLL, "CreateModule" );
                    if ( pModuleFactoryFunc != nullptr )
                    {
                        ResourceCompilerModule* pResourceModule = pModuleFactoryFunc();
                        KRG_ASSERT( pResourceModule != nullptr );
                        for ( auto compiler : pResourceModule->GetRegisteredCompilers() )
                        {
                            m_compilers.push_back( { compiler->GetName(), compiler->GetVersion(), compiler->GetOutputTypes(), compiler->GetVirtualTypes(), compilerModule } );
                        }

                        KRG::Delete( pResourceModule );
                    }
                    FreeLibrary( moduleDLL );
                }
            }

            // Open network connection
            //-------------------------------------------------------------------------

            if ( !m_networkServer.Start( Settings::g_resourceServerPort ) )
            {
                return false;
            }

            //-------------------------------------------------------------------------

            if( m_fileSystemWatcher.StartWatching( m_sourceDataDir ) )
            {
                m_fileSystemWatcher.RegisterChangeListener( this );
            }

            return true;
        }

        void ResourceServer::Shutdown()
        {
            if( m_fileSystemWatcher.IsWatching() )
            {
                m_fileSystemWatcher.StopWatching();
                m_fileSystemWatcher.UnregisterChangeListener( this );
            }

            // Delete requests
            for( auto& pRequest : m_pendingRequests )
            {
                KRG::Delete( pRequest );
            }

            for ( auto& pRequest : m_activeRequests )
            {
                KRG::Delete( pRequest );
            }

            CleanupCompletedRequests();
        }

        //-------------------------------------------------------------------------

        void ResourceServer::OnThreadStarted()
        {
            Memory::InitializeThreadHeap();

            // Create compilation workers
            //-------------------------------------------------------------------------
            // Need to do it here since they need to be created in the same thread as the execute loop

            m_workers.resize( m_maxSimultaneousCompilationTasks );
            for ( auto i = 0u; i < m_maxSimultaneousCompilationTasks; i++ )
            {
                m_workers[i] = KRG::New<CompilationWorker>( m_workerFullPath );
            }

            // Create and start loop timer
            //-------------------------------------------------------------------------

            m_pLoopTimer = new QTimer( this );
            connect( m_pLoopTimer, SIGNAL( timeout() ), this, SLOT( Execute() ) );
            m_pLoopTimer->start( 5 );
        }

        void ResourceServer::Execute()
        {
            bool shouldNotifyActiveListChanged = false;
            bool shouldNotifyCompletedListChanged = false;

            // Update network server
            //-------------------------------------------------------------------------

            if ( m_networkServer.IsRunning() )
            {
                Network::IPC::Message message;
                if ( m_networkServer.WaitForMessage( message, 0 ) )
                {
                    // Track connected clients
                    //-------------------------------------------------------------------------

                    uint64 const clientID = message.GetClientID().m_ID;
                    auto foundClientIter = VectorFind( m_knownClients, clientID, [] ( ClientRecord const& record, uint64 clientID ) { return record.m_clientID == clientID; } );
                    if ( foundClientIter != m_knownClients.end() )
                    {
                        foundClientIter->m_lastUpdateTime = SystemClock::GetTimeInSeconds();
                    }
                    else // Add new client record
                    {
                        m_knownClients.emplace_back( ClientRecord( clientID ) );
                    }

                    // Process messages
                    //-------------------------------------------------------------------------

                    if ( message.GetMessageID() == (int32) NetworkMessageID::RequestResource )
                    {
                        NetworkResourceRequest networkRequest = message.GetData<NetworkResourceRequest>();
                        auto pCompilationRequest = ProcessResourceRequest( networkRequest.m_path, clientID );

                        if( pCompilationRequest->IsPending() )
                        {
                            shouldNotifyActiveListChanged = true;
                        }
                        else if( pCompilationRequest->IsComplete() )
                        {
                            shouldNotifyCompletedListChanged = true;
                        }
                    }
                };

                // Clean up connected clients list
                //-------------------------------------------------------------------------

                static Seconds const keepAliveTime( 10 );
                Seconds const currentTime = SystemClock::GetTimeInSeconds();
                for ( int32 i = (int32) m_knownClients.size() - 1; i >= 0; i-- )
                {
                    Seconds const timeSinceLastUpdate = currentTime - m_knownClients[i].m_lastUpdateTime;
                    if( timeSinceLastUpdate > keepAliveTime )
                    {
                        m_knownClients.erase_unsorted( m_knownClients.begin() + i );
                    }
                }
            }

            // Update requests
            //-------------------------------------------------------------------------

            // Check status of active requests
            for ( int32 i = ( int32) m_activeRequests.size() - 1; i >= 0; i-- )
            {
                auto const pActiveRequest = m_activeRequests[i];
                if ( pActiveRequest->IsComplete() )
                {
                    // Update database
                    //-------------------------------------------------------------------------

                    if ( pActiveRequest->HasSucceeded() )
                    {
                        WriteCompiledResourceRecord( pActiveRequest );
                    }

                    // Send network response
                    //-------------------------------------------------------------------------

                    NotifyClientOnCompletedRequest( pActiveRequest );

                    // Remove from active list
                    //-------------------------------------------------------------------------

                    m_completedRequests.emplace_back( pActiveRequest );
                    m_activeRequests.erase_first_unsorted( pActiveRequest );
                    shouldNotifyCompletedListChanged = true;
                    shouldNotifyActiveListChanged = true;
                }
            }

            // Kick off new requests
            while ( m_pendingRequests.size() > 0 && m_activeRequests.size() < m_maxSimultaneousCompilationTasks )
            {
                auto pRequestToStart = m_pendingRequests[0];
                m_activeRequests.emplace_back( pRequestToStart );
                m_pendingRequests.erase( m_pendingRequests.begin() );
                shouldNotifyActiveListChanged = true;

                bool taskStarted = false;
                for ( auto pWorker : m_workers )
                {
                    if ( !pWorker->IsBusy() )
                    {
                        pWorker->StartCompileTask( pRequestToStart );
                        taskStarted = true;
                        break;
                    }
                }

                KRG_ASSERT( taskStarted );
            }

            // Process cleanup request
            //-------------------------------------------------------------------------

            if ( m_cleanupRequested )
            {
                CleanupCompletedRequests();
                shouldNotifyCompletedListChanged = true;
                m_cleanupRequested = false;
            }

            // Update File System Watcher
            //-------------------------------------------------------------------------

            if ( m_fileSystemWatcher.IsWatching() )
            {
                m_fileSystemWatcher.Update();
            }

            // Emit UI updates
            //-------------------------------------------------------------------------

            if( shouldNotifyCompletedListChanged )
            {
                emit CompletedRequestsUpdated( ( TVector<CompilationRequest const*> const* )& m_completedRequests );
            }

            if ( shouldNotifyActiveListChanged )
            {
                emit ActiveRequestsUpdated( ( TVector<CompilationRequest const*> const* )& m_pendingRequests, ( TVector<CompilationRequest const*> const* )& m_activeRequests );
            }

            if( shouldNotifyCompletedListChanged || shouldNotifyActiveListChanged )
            {
                emit BusyStateChanged();
            }
        }

        void ResourceServer::OnThreadStopped()
        {
            // Destroy timer
            m_pLoopTimer->stop();
            delete m_pLoopTimer;

            // Shutdown workers
            for ( auto& pWorker : m_workers )
            {
                KRG::Delete( pWorker );
            }

            Memory::ShutdownThreadHeap();
        }

        void ResourceServer::OnFileModified( FileSystemPath const& filePath )
        {
            KRG_ASSERT( filePath.IsValid() && filePath.IsFilePath() );

            DataPath dataPath = DataPath::FromFileSystemPath( m_sourceDataDir, filePath );
            if ( !dataPath.IsValid() )
            {
                return;
            }

            ResourceID resourceID( dataPath );
            if ( !resourceID.IsValid() )
            {
                return;
            }

            // Check compiled resources database for a record for this file
            auto compiledResourceRecord = m_compiledResourceDatabase.GetRecord( resourceID );
            if ( !compiledResourceRecord.IsValid() )
            {
                return;
            }

            // If we have a record, then schedule a recompile task
            ProcessResourceRequest( resourceID );
        }

        void ResourceServer::CleanupCompletedRequests()
        {
            for ( int32 i = (int32) m_completedRequests.size() - 1; i >= 0; i-- )
            {
                KRG_ASSERT( !VectorContains( m_activeRequests, m_completedRequests[i] ) && !VectorContains( m_pendingRequests, m_completedRequests[i] ) );
                KRG::Delete( m_completedRequests[i] );
                m_completedRequests.erase_unsorted( m_completedRequests.begin() + i );
            }
        }

        //-------------------------------------------------------------------------

        bool ResourceServer::HasCompilerForType( ResourceTypeID typeID ) const
        {
            for ( auto const& compiler : m_compilers )
            {
                if( VectorContains( compiler.m_outputTypes, typeID ) )
                {
                    return true;
                }
            }
            return false;
        }

        bool ResourceServer::IsVirtualResource( ResourceTypeID typeID ) const
        {
            for ( auto const& compiler : m_compilers )
            {
                if ( VectorContains( compiler.m_virtualTypes, typeID ) )
                {
                    return true;
                }
            }
            return false;
        }

        int32 ResourceServer::GetCompilerVersion( ResourceTypeID typeID ) const
        {
            for ( auto const& compiler : m_compilers )
            {
                if ( VectorContains( compiler.m_outputTypes, typeID ) )
                {
                    return compiler.m_version;
                }
            }

            return -1;
        }

        //-------------------------------------------------------------------------

        CompilationRequest const* ResourceServer::ProcessResourceRequest( ResourceID const& resourceID, uint64 clientID )
        {
            KRG_ASSERT( m_compiledResourceDatabase.IsConnected() );

            CompilationRequest* pRequest = KRG::New<CompilationRequest>();

            if ( resourceID.IsValid() )
            {
                pRequest->m_clientID = clientID;
                pRequest->m_isHotReloadRequest = ( clientID == 0 );
                pRequest->m_resourceID = resourceID;
                pRequest->m_sourceFile = DataPath::ToFileSystemPath( m_sourceDataDir, pRequest->m_resourceID.GetDataPath() );
                pRequest->m_destinationFile = DataPath::ToFileSystemPath( m_compiledDataDir, pRequest->m_resourceID.GetDataPath() );
                pRequest->m_compilerArgs = pRequest->m_resourceID.GetDataPath().c_str();

                // Resource type validity check
                ResourceTypeID const resourceTypeID = pRequest->m_resourceID.GetResourceTypeID();
                if ( IsVirtualResource( resourceTypeID ) )
                {
                    pRequest->m_log = String().sprintf( "Virtual Resource - Nothing to do!", pRequest->m_sourceFile.GetFullPath().c_str() );
                    pRequest->m_status = CompilationRequest::Status::Succeeded;
                }
                else
                {
                    if ( !HasCompilerForType( resourceTypeID ) )
                    {
                        pRequest->m_log = String().sprintf( "Error: No compiler found for resource type ( %s )!", pRequest->m_resourceID.ToString().c_str() );
                        pRequest->m_status = CompilationRequest::Status::Failed;
                    }

                    // File Validity check
                    if ( pRequest->m_status != CompilationRequest::Status::Failed )
                    {
                        if ( !FileSystem::FileExists( pRequest->m_sourceFile ) )
                        {
                            pRequest->m_log = String().sprintf( "Error: Source file ( %s ) doesnt exist!", pRequest->m_sourceFile.GetFullPath().c_str() );
                            pRequest->m_status = CompilationRequest::Status::Failed;
                        }
                    }

                    if ( pRequest->m_status != CompilationRequest::Status::Failed )
                    {
                        if ( !FileSystem::EnsurePathExists( pRequest->m_destinationFile ) )
                        {
                            pRequest->m_log = String().sprintf( "Error: Destination path ( %s ) doesnt exist!", pRequest->m_destinationFile.GetParentDirectory().c_str() );
                            pRequest->m_status = CompilationRequest::Status::Failed;
                        }
                    }

                    if ( pRequest->m_status != CompilationRequest::Status::Failed )
                    {
                        if ( FileSystem::FileExists( pRequest->m_destinationFile ) && FileSystem::IsFileReadOnly( pRequest->m_destinationFile ) )
                        {
                            pRequest->m_log = String().sprintf( "Error: Destination file ( %s ) is read-only!", pRequest->m_destinationFile.GetFullPath().c_str() );
                            pRequest->m_status = CompilationRequest::Status::Failed;
                        }
                    }

                    TVector<DataPath> compileDependencies;

                    if ( pRequest->m_status != CompilationRequest::Status::Failed )
                    {
                        // Try to read all the resource compile dependencies for non-map resources
                        if ( pRequest->GetResourceID().GetResourceTypeID() != ResourceTypeID( "MAP" ) )
                        {
                            if ( !TryReadCompileDependencies( pRequest->m_sourceFile, compileDependencies, &pRequest->m_log ) )
                            {
                                pRequest->m_log += "Error: failed to read compile dependencies!";
                                pRequest->m_status = CompilationRequest::Status::Failed;
                            }
                        }
                    }

                    // Run Up-to-date check
                    if ( pRequest->m_status != CompilationRequest::Status::Failed )
                    {
                        PerformResourceUpToDateCheck( pRequest, compileDependencies );
                    }
                }
            }
            else // Invalid resource ID
            {
                pRequest->m_log = String( "Error: Invalid resource ID (" ) + resourceID.c_str() + ") !";
                pRequest->m_status = CompilationRequest::Status::Failed;
            }

            // Enqueue new request
            //-------------------------------------------------------------------------

            if ( pRequest->IsPending() )
            {
                m_pendingRequests.emplace_back( pRequest );
            }
            else // Failed or Up-to-date
            {
                m_completedRequests.emplace_back( pRequest );
                KRG_ASSERT( pRequest->IsComplete() );
                NotifyClientOnCompletedRequest( pRequest );
            }

            return pRequest;
        }

        void ResourceServer::NotifyClientOnCompletedRequest( CompilationRequest* pRequest )
        {
            NetworkResourceResponse response;
            response.m_resourceID = pRequest->GetResourceID();
            if ( pRequest->HasSucceeded() )
            {
                response.m_filePath = pRequest->GetDestinationFilePath();
            }

            //-------------------------------------------------------------------------

            // Notify all clients
            if( pRequest->IsInternalRequest() )
            {
                // Only notify all clients is the request succeeded
                if( pRequest->HasSucceeded() )
                {
                    for ( auto const& clientRecord : m_knownClients )
                    {
                        Network::IPC::Message message;
                        message.SetData( (int32) NetworkMessageID::ResourceUpdated, response );
                        m_networkServer.SendMessageToClient( clientRecord.m_clientID, message );
                    }
                }
            }
            else // Notify single client
            {
                Network::IPC::Message message;
                message.SetData( (int32) NetworkMessageID::ResourceRequestComplete, response );
                m_networkServer.SendMessageToClient( pRequest->GetClientID(), message );
            }
        }

        //-------------------------------------------------------------------------

        void ResourceServer::PerformResourceUpToDateCheck( CompilationRequest* pRequest, TVector<DataPath> const& compileDependencies ) const
        {
            KRG_ASSERT( pRequest != nullptr && pRequest->IsPending() );

            pRequest->m_upToDateCheckTimeStarted = SystemClock::GetTime();

            // Read all up to date information
            //-------------------------------------------------------------------------

            pRequest->m_compilerVersion = GetCompilerVersion( pRequest->m_resourceID.GetResourceTypeID() );
            KRG_ASSERT( pRequest->m_compilerVersion >= 0 );

            pRequest->m_fileTimestamp = FileSystem::GetFileModifiedTime( pRequest->m_sourceFile );

            bool areCompileDependenciesAreUpToDate = true;
            for ( auto const& compileDep : compileDependencies )
            {
                KRG_ASSERT( compileDep.IsValid() );

                pRequest->m_sourceTimestampHash += FileSystem::GetFileModifiedTime( DataPath::ToFileSystemPath( m_sourceDataDir, compileDep ) );

                ResourceTypeID const extension( compileDep.GetExtension() );
                if ( IsCompileableResourceType( extension ) )
                {
                    areCompileDependenciesAreUpToDate &= IsResourceUpToDate( ResourceID( compileDep ) );
                }
            }

            // Check source file for changes
            //-------------------------------------------------------------------------

            // Check compile dependency state
            bool isResourceUpToDate = areCompileDependenciesAreUpToDate;

            // Check against previous compilation result
            if ( isResourceUpToDate )
            {
                auto existingRecord = m_compiledResourceDatabase.GetRecord( pRequest->m_resourceID );
                if ( existingRecord.IsValid() )
                {
                    if ( pRequest->m_compilerVersion != existingRecord.m_compilerVersion )
                    {
                        isResourceUpToDate = false;
                    }

                    if ( pRequest->m_fileTimestamp != existingRecord.m_fileTimestamp )
                    {
                        isResourceUpToDate = false;
                    }

                    if ( pRequest->m_sourceTimestampHash != existingRecord.m_sourceTimestampHash )
                    {
                        isResourceUpToDate = false;
                    }
                }
                else
                {
                    isResourceUpToDate = false;
                }
            }

            // Check that the target file exists
            if ( isResourceUpToDate && !FileSystem::FileExists( pRequest->m_destinationFile ) )
            {
                isResourceUpToDate = false;
            }

            //-------------------------------------------------------------------------

            if ( isResourceUpToDate )
            {
                pRequest->m_log = String().sprintf( "Resource up to date!", pRequest->m_sourceFile.GetFullPath().c_str() );
                pRequest->m_status = CompilationRequest::Status::Succeeded;
            }

            //-------------------------------------------------------------------------

            pRequest->m_upToDateCheckTimeFinished = SystemClock::GetTime();
        }

        bool ResourceServer::TryReadCompileDependencies( FileSystemPath const& resourceFilePath, TVector<DataPath>& outDependencies, String* pErrorLog ) const
        {
            KRG_ASSERT( resourceFilePath.IsValid() );

            // Read JSON descriptor file - we do this by hand since we dont want to create a type registry in the resource server
            if ( resourceFilePath.Exists() )
            {
                FILE* fp = fopen( resourceFilePath, "r" );
                if ( fp == nullptr )
                {
                    return false;
                }

                fseek( fp, 0, SEEK_END );
                size_t filesize = (size_t) ftell( fp );
                fseek( fp, 0, SEEK_SET );

                String fileContents;
                fileContents.resize( filesize );
                size_t const readLength = fread( fileContents.data(), 1, filesize, fp );
                fclose( fp );

                ResourceDescriptor::ReadCompileDependencies( fileContents, outDependencies );
            }
            else
            {
                return false;
            }

            //-------------------------------------------------------------------------

            for ( auto const& dep : outDependencies )
            {
                if ( !dep.IsValid() )
                {
                    return false;
                }
            }

            return true;
        }

        bool ResourceServer::IsResourceUpToDate( ResourceID const& resourceID ) const
        {
            // Read all up to date information
            //-------------------------------------------------------------------------

            int32 const compilerVersion = GetCompilerVersion( resourceID.GetResourceTypeID() );
            KRG_ASSERT( compilerVersion >= 0 );

            FileSystemPath const sourceFilePath = DataPath::ToFileSystemPath( m_sourceDataDir, resourceID.GetDataPath() );
            uint64 const fileTimestamp = FileSystem::GetFileModifiedTime( sourceFilePath );
            uint64 sourceTimestampHash = 0;

            bool areCompileDependenciesAreUpToDate = true;
            TVector<DataPath> compileDependencies;
            TryReadCompileDependencies( sourceFilePath, compileDependencies );
            for ( auto const& compileDep : compileDependencies )
            {
                sourceTimestampHash += FileSystem::GetFileModifiedTime( DataPath::ToFileSystemPath( m_sourceDataDir, compileDep ) );

                ResourceTypeID const extension( compileDep.GetExtension() );
                if ( IsCompileableResourceType( extension ) )
                {
                    areCompileDependenciesAreUpToDate &= IsResourceUpToDate( ResourceID( compileDep ) );
                }
            }

            // Check source file for changes
            //-------------------------------------------------------------------------

            // Check compile dependency state
            bool isResourceUpToDate = areCompileDependenciesAreUpToDate;

            // Check against previous compilation result
            if ( isResourceUpToDate )
            {
                auto existingRecord = m_compiledResourceDatabase.GetRecord( resourceID );
                if ( existingRecord.IsValid() )
                {
                    if ( compilerVersion != existingRecord.m_compilerVersion )
                    {
                        isResourceUpToDate = false;
                    }

                    if ( fileTimestamp != existingRecord.m_fileTimestamp )
                    {
                        isResourceUpToDate = false;
                    }

                    if ( sourceTimestampHash != existingRecord.m_sourceTimestampHash )
                    {
                        isResourceUpToDate = false;
                    }
                }
                else
                {
                    isResourceUpToDate = false;
                }
            }

            // Check that the target file exists
            if ( isResourceUpToDate && !FileSystem::FileExists( DataPath::ToFileSystemPath( m_compiledDataDir, resourceID.GetDataPath() ) ) )
            {
                isResourceUpToDate = false;
            }

            return isResourceUpToDate;
        }

        void ResourceServer::WriteCompiledResourceRecord( CompilationRequest* pRequest )
        {
            CompiledResourceRecord record;
            record.m_resourceID = pRequest->m_resourceID;
            record.m_compilerVersion = pRequest->m_compilerVersion;
            record.m_fileTimestamp = pRequest->m_fileTimestamp;
            record.m_sourceTimestampHash = pRequest->m_sourceTimestampHash;
            m_compiledResourceDatabase.WriteRecord( record );
        }

        bool ResourceServer::IsCompileableResourceType( ResourceTypeID ID ) const
        {
            return ( ID.IsValid() && HasCompilerForType( ID ) );
        }
    }
}