#include "ResourceServer.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"
#include "System/Resource/ResourceProviders/ResourceNetworkMessages.h"
#include "System/Core/ThirdParty/iniparser/krg_ini.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/ThirdParty/cereal/archives/json.hpp"

#include <sstream>

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    ResourceServer::ResourceServer()
    {
        m_activeRequests.reserve( 100 );
        m_maxSimultaneousCompilationTasks = Threading::GetProcessorInfo().m_numPhysicalCores;
    }

    bool ResourceServer::Initialize( Settings const& settings )
    {
        KRG_ASSERT( m_pSettings == nullptr );
        m_pSettings = &settings;

        // Connect to compiled resource database
        //-------------------------------------------------------------------------

        if ( !m_compiledResourceDatabase.TryConnect( settings.m_compiledResourceDatabasePath ) )
        {
            m_errorMessage = String().sprintf( "Database connection error: %s", m_compiledResourceDatabase.GetError().c_str() );
            return false;
        }

        // Register compilers
        //-------------------------------------------------------------------------

        m_animationModule.RegisterCompilers( m_compilerRegistry );
        m_renderModule.RegisterCompilers( m_compilerRegistry );
        m_physicsModule.RegisterCompilers( m_compilerRegistry );
        m_entityModule.RegisterCompilers( m_compilerRegistry );

        // Open network connection
        //-------------------------------------------------------------------------

        if ( !Network::NetworkSystem::Initialize() )
        {
            return false;
        }

        if ( !Network::NetworkSystem::StartServerConnection( &m_networkServer, settings.m_resourceServerPort ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( m_fileSystemWatcher.StartWatching( m_pSettings->m_rawResourcePath ) )
        {
            m_fileSystemWatcher.RegisterChangeListener( this );
        }

        // Create Workers
        //-------------------------------------------------------------------------

        m_taskSystem.Initialize();

        for ( auto i = 0u; i < m_maxSimultaneousCompilationTasks; i++ )
        {
            m_workers.emplace_back( KRG::New<ResourceServerWorker>( &m_taskSystem, m_pSettings->m_resourceCompilerExecutablePath.c_str() ) );
        }

        return true;
    }

    void ResourceServer::Shutdown()
    {
        // Destroy workers
        //-------------------------------------------------------------------------

        m_taskSystem.WaitForAll();

        for ( auto& pWorker : m_workers )
        {
            KRG::Delete( pWorker );
        }

        m_workers.clear();

        m_taskSystem.Shutdown();

        // Unregister File Watcher
        //-------------------------------------------------------------------------

        if ( m_fileSystemWatcher.IsWatching() )
        {
            m_fileSystemWatcher.StopWatching();
            m_fileSystemWatcher.UnregisterChangeListener( this );
        }

        // Delete requests
        //-------------------------------------------------------------------------

        for ( auto& pRequest : m_pendingRequests )
        {
            KRG::Delete( pRequest );
        }

        for ( auto& pRequest : m_activeRequests )
        {
            KRG::Delete( pRequest );
        }

        CleanupCompletedRequests();

        // Unregister compilers
        //-------------------------------------------------------------------------

        m_animationModule.UnregisterCompilers( m_compilerRegistry );
        m_renderModule.UnregisterCompilers( m_compilerRegistry );
        m_physicsModule.UnregisterCompilers( m_compilerRegistry );
        m_entityModule.UnregisterCompilers( m_compilerRegistry );

        //-------------------------------------------------------------------------

        Network::NetworkSystem::StopServerConnection( &m_networkServer );
        Network::NetworkSystem::Shutdown();

        //-------------------------------------------------------------------------

        m_pSettings = nullptr;
    }

    //-------------------------------------------------------------------------

    void ResourceServer::Update()
    {
        // Update network server
        //-------------------------------------------------------------------------

        Network::NetworkSystem::Update();

        if ( m_networkServer.IsRunning() )
        {
            auto ProcessIncomingMessages = [this] ( Network::IPC::Message const& message )
            {
                if ( message.GetMessageID() == (int32) NetworkMessageID::RequestResource )
                {
                    uint32 const clientID = message.GetClientConnectionID();
                    NetworkResourceRequest networkRequest = message.GetData<NetworkResourceRequest>();
                    ProcessResourceRequest( networkRequest.m_path, clientID );
                }
            };

            m_networkServer.ProcessIncomingMessages( ProcessIncomingMessages );
        }

        // Update requests
        //-------------------------------------------------------------------------

        // Check status of active requests
        for ( auto pWorker : m_workers )
        {
            if ( pWorker->IsComplete() )
            {
                auto const pCompletedRequest = pWorker->AcceptResult();

                // Update database
                //-------------------------------------------------------------------------

                if ( pCompletedRequest->HasSucceeded() )
                {
                    WriteCompiledResourceRecord( pCompletedRequest );
                }

                // Send network response
                //-------------------------------------------------------------------------

                NotifyClientOnCompletedRequest( pCompletedRequest );

                // Remove from active list
                //-------------------------------------------------------------------------

                m_completedRequests.emplace_back( pCompletedRequest );
                m_activeRequests.erase_first_unsorted( pCompletedRequest );
            }
        }

        // Kick off new requests
        while ( m_pendingRequests.size() > 0 && m_activeRequests.size() < m_maxSimultaneousCompilationTasks )
        {
            auto pRequestToStart = m_pendingRequests[0];
            m_activeRequests.emplace_back( pRequestToStart );
            m_pendingRequests.erase( m_pendingRequests.begin() );

            bool taskStarted = false;
            for ( auto& pWorker : m_workers )
            {
                if ( pWorker->IsIdle() )
                {
                    pWorker->Compile( pRequestToStart );
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
            m_cleanupRequested = false;
        }

        // Update File System Watcher
        //-------------------------------------------------------------------------

        if ( m_fileSystemWatcher.IsWatching() )
        {
            m_fileSystemWatcher.Update();
        }
    }

    void ResourceServer::OnFileModified( FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsValid() && filePath.IsFile() );

        ResourcePath resourcePath = ResourcePath::FromFileSystemPath( m_pSettings->m_rawResourcePath, filePath );
        if ( !resourcePath.IsValid() )
        {
            return;
        }

        ResourceID resourceID( resourcePath );
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

    CompilationRequest const* ResourceServer::ProcessResourceRequest( ResourceID const& resourceID, uint32 clientID )
    {
        KRG_ASSERT( m_compiledResourceDatabase.IsConnected() );

        CompilationRequest* pRequest = KRG::New<CompilationRequest>();

        if ( resourceID.IsValid() )
        {
            pRequest->m_clientID = clientID;
            pRequest->m_isHotReloadRequest = ( clientID == 0 );
            pRequest->m_resourceID = resourceID;
            pRequest->m_sourceFile = ResourcePath::ToFileSystemPath( m_pSettings->m_rawResourcePath, pRequest->m_resourceID.GetResourcePath() );
            pRequest->m_destinationFile = ResourcePath::ToFileSystemPath( m_pSettings->m_compiledResourcePath, pRequest->m_resourceID.GetResourcePath() );
            pRequest->m_compilerArgs = pRequest->m_resourceID.GetResourcePath().c_str();

            // Resource type validity check
            ResourceTypeID const resourceTypeID = pRequest->m_resourceID.GetResourceTypeID();
            if ( m_compilerRegistry.IsVirtualType( resourceTypeID ) )
            {
                pRequest->m_log = String().sprintf( "Virtual Resource - Nothing to do!", pRequest->m_sourceFile.GetFullPath().c_str() );
                pRequest->m_status = CompilationRequest::Status::Succeeded;
            }
            else
            {
                if ( !m_compilerRegistry.HasCompilerForType( resourceTypeID ) )
                {
                    pRequest->m_log = String().sprintf( "Error: No compiler found for resource type ( %s )!", pRequest->m_resourceID.ToString().c_str() );
                    pRequest->m_status = CompilationRequest::Status::Failed;
                }

                // File Validity check
                if ( pRequest->m_status != CompilationRequest::Status::Failed )
                {
                    if ( !FileSystem::Exists( pRequest->m_sourceFile ) )
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
                    if ( FileSystem::Exists( pRequest->m_destinationFile ) && FileSystem::IsFileReadOnly( pRequest->m_destinationFile ) )
                    {
                        pRequest->m_log = String().sprintf( "Error: Destination file ( %s ) is read-only!", pRequest->m_destinationFile.GetFullPath().c_str() );
                        pRequest->m_status = CompilationRequest::Status::Failed;
                    }
                }

                TVector<ResourcePath> compileDependencies;

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
        if ( pRequest->IsInternalRequest() )
        {
            // Only notify all clients is the request succeeded
            if ( pRequest->HasSucceeded() )
            {
                for ( auto const& clientID : m_networkServer.GetConnectedClientIDs() )
                {
                    Network::IPC::Message message;
                    message.SetClientConnectionID( clientID );
                    message.SetData( (int32) NetworkMessageID::ResourceUpdated, response );
                    m_networkServer.SendMessage( eastl::move( message ) );
                }
            }
        }
        else // Notify single client
        {
            Network::IPC::Message message;
            message.SetClientConnectionID( pRequest->GetClientID() );
            message.SetData( (int32) NetworkMessageID::ResourceRequestComplete, response );
            m_networkServer.SendMessage( eastl::move( message ) );
        }
    }

    //-------------------------------------------------------------------------

    void ResourceServer::PerformResourceUpToDateCheck( CompilationRequest* pRequest, TVector<ResourcePath> const& compileDependencies ) const
    {
        KRG_ASSERT( pRequest != nullptr && pRequest->IsPending() );

        pRequest->m_upToDateCheckTimeStarted = SystemClock::GetTime();

        // Read all up to date information
        //-------------------------------------------------------------------------

        pRequest->m_compilerVersion = m_compilerRegistry.GetVersionForType( pRequest->m_resourceID.GetResourceTypeID() );
        KRG_ASSERT( pRequest->m_compilerVersion >= 0 );

        pRequest->m_fileTimestamp = FileSystem::GetFileModifiedTime( pRequest->m_sourceFile );

        bool areCompileDependenciesAreUpToDate = true;
        for ( auto const& compileDep : compileDependencies )
        {
            KRG_ASSERT( compileDep.IsValid() );

            ResourceTypeID const extension( compileDep.GetExtension() );
            if ( IsCompileableResourceType( extension ) && !IsResourceUpToDate( ResourceID( compileDep ) ) )
            {
                areCompileDependenciesAreUpToDate = false;
                break;
            }

            auto const compileDependencyPath = ResourcePath::ToFileSystemPath( m_pSettings->m_rawResourcePath, compileDep );
            if ( !FileSystem::Exists( compileDependencyPath ) )
            {
                areCompileDependenciesAreUpToDate = false;
                break;
            }

            pRequest->m_sourceTimestampHash += FileSystem::GetFileModifiedTime( compileDependencyPath );
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
        if ( isResourceUpToDate && !FileSystem::Exists( pRequest->m_destinationFile ) )
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

    bool ResourceServer::TryReadCompileDependencies( FileSystem::Path const& resourceFilePath, TVector<ResourcePath>& outDependencies, String* pErrorLog ) const
    {
        KRG_ASSERT( resourceFilePath.IsValid() );

        // Read JSON descriptor file - we do this by hand since we dont want to create a type registry in the resource server
        if ( FileSystem::Exists( resourceFilePath ) )
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
        // Check that the target file exists
        //-------------------------------------------------------------------------

        if ( !FileSystem::Exists( ResourcePath::ToFileSystemPath( m_pSettings->m_compiledResourcePath, resourceID.GetResourcePath() ) ) )
        {
            return false;
        }

        // Check compile dependencies
        //-------------------------------------------------------------------------

        int32 const compilerVersion = m_compilerRegistry.GetVersionForType( resourceID.GetResourceTypeID() );
        KRG_ASSERT( compilerVersion >= 0 );

        FileSystem::Path const sourceFilePath = ResourcePath::ToFileSystemPath( m_pSettings->m_rawResourcePath, resourceID.GetResourcePath() );
        if ( !FileSystem::Exists( sourceFilePath ) )
        {
            return false;
        }

        uint64 const fileTimestamp = FileSystem::GetFileModifiedTime( sourceFilePath );
        uint64 sourceTimestampHash = 0;

        TVector<ResourcePath> compileDependencies;
        if ( !TryReadCompileDependencies( sourceFilePath, compileDependencies ) )
        {
            return false;
        }

        for ( auto const& compileDep : compileDependencies )
        {
            sourceTimestampHash += FileSystem::GetFileModifiedTime( ResourcePath::ToFileSystemPath( m_pSettings->m_rawResourcePath, compileDep ) );

            ResourceTypeID const extension( compileDep.GetExtension() );
            if ( IsCompileableResourceType( extension ) && !IsResourceUpToDate( ResourceID( compileDep ) ) )
            {
                return false;
            }
        }

        // Check source file for changes
        //-------------------------------------------------------------------------

        // Check against previous compilation result
        auto existingRecord = m_compiledResourceDatabase.GetRecord( resourceID );
        if ( existingRecord.IsValid() )
        {
            if ( compilerVersion != existingRecord.m_compilerVersion )
            {
                return false;
            }

            if ( fileTimestamp != existingRecord.m_fileTimestamp )
            {
                return false;
            }

            if ( sourceTimestampHash != existingRecord.m_sourceTimestampHash )
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        return true;
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
        return ( ID.IsValid() && m_compilerRegistry.HasCompilerForType( ID ) );
    }
}