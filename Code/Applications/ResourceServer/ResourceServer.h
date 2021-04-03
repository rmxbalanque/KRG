#pragma once
#include "ResourceCompilerInfo.h"
#include "ResourceServerWorker.h"
#include "ResourceCompilationRequest.h"
#include "CompiledResourceDatabase.h"
#include "Tools/Core/FileSystem/FileSystemWatcher.h"
#include "System/Network/IPC/IPCMessageServer.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Threading/TaskSystem.h"

//-------------------------------------------------------------------------
// The network resource server
//-------------------------------------------------------------------------
// Receives resource requests, triggers the compilation of said requests and returns the results
// Runs in a separate thread from the main UI
//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceServer : public FileSystem::IFileSystemChangeListener
    {
        struct ClientRecord
        {
            explicit ClientRecord( uint64 clientID ) : m_clientID( clientID ) { KRG_ASSERT( clientID != 0 ); }

            Seconds                             m_lastUpdateTime = SystemClock::GetTimeInSeconds();
            uint64                              m_clientID = 0;
        };

    public:

        ResourceServer();

        bool Initialize();
        void Shutdown();
        void Update();

        inline bool IsBusy() const { return m_pendingRequests.size() + m_activeRequests.size() > 0; }

        inline String const& GetErrorMessage() const { return m_errorMessage; }
        inline String const& GetNetworkAddress() const { return m_networkAddress; }
        inline FileSystem::Path const& GetSourceDataDir() const { return m_sourceDataDir; }
        inline FileSystem::Path const& GetCompiledDataDir() const { return m_compiledDataDir; }

        // Compilers
        inline TVector<CompilerInfo> const& GetRegisteredCompilers() const { return m_compilers; }

        // Requests
        TVector<CompilationRequest const*> const& GetActiveRequests() const { return ( TVector<CompilationRequest const*>& ) m_activeRequests; }
        TVector<CompilationRequest const*> const& GetPendingRequests() const { return ( TVector<CompilationRequest const*>& ) m_pendingRequests; }
        TVector<CompilationRequest const*> const& GetCompletedRequests() const { return ( TVector<CompilationRequest const*>& ) m_completedRequests; }
        inline void RequestCleanupOfCompletedRequests() { m_cleanupRequested = true; }

        // Workers
        inline int32 GetNumWorkers() const { return (int32) m_workers.size(); }
        inline ResourceServerWorker::Status GetWorkerStatus( int32 workerIdx ) const { return m_workers[workerIdx]->GetStatus(); }
        inline ResourceID const& GetCompilationTaskResourceID( int32 workerIdx ) const { return m_workers[workerIdx]->GetRequestResourceID(); }

    private:

        void CleanupCompletedRequests();

        // Compiler Info
        bool HasCompilerForType( ResourceTypeID typeID ) const;
        bool IsVirtualResource( ResourceTypeID typeID ) const;
        int32 GetCompilerVersion( ResourceTypeID typeID ) const;

        // Request Actions
        CompilationRequest const* ProcessResourceRequest( ResourceID const& resourceID, uint64 clientID = 0 );
        void NotifyClientOnCompletedRequest( CompilationRequest* pRequest );

        // Up-to-date system
        void PerformResourceUpToDateCheck( CompilationRequest* pRequest, TVector<DataPath> const& compileDependencies ) const;
        bool TryReadCompileDependencies( FileSystem::Path const& resourceFilePath, TVector<DataPath>& outDependencies, String* pErrorLog = nullptr ) const;
        bool IsResourceUpToDate( ResourceID const& resourceID ) const;
        void WriteCompiledResourceRecord( CompilationRequest* pRequest );
        bool IsCompileableResourceType( ResourceTypeID ID ) const;

        // File system listener
        virtual void OnFileModified( FileSystem::Path const& filePath ) override final;

    private:

        String                                  m_errorMessage;
        bool                                    m_cleanupRequested = false;
        Network::IPC::Server                    m_networkServer;
        TVector<ClientRecord>                   m_knownClients;

        // Settings
        SettingsRegistry                        m_settingsRegistry;
        FileSystem::Path                          m_workingDir;
        FileSystem::Path                          m_sourceDataDir;
        FileSystem::Path                          m_compiledDataDir;
        FileSystem::Path                          m_compiledResourceDatabasePath;
        String                                  m_networkAddress;
        String                                  m_resourceCompilerFullPath;
        uint32                                  m_maxSimultaneousCompilationTasks = 16;

        // Compiler list
        TVector<CompilerInfo>                   m_compilers;

        // Compilation Requests
        CompiledResourceDatabase                m_compiledResourceDatabase;
        TVector<CompilationRequest*>            m_completedRequests;
        TVector<CompilationRequest*>            m_pendingRequests;
        TVector<CompilationRequest*>            m_activeRequests;

        // Workers
        TaskSystem                              m_taskSystem;
        TVector<ResourceServerWorker*>          m_workers;

        // File System Watcher
        FileSystem::FileSystemWatcher           m_fileSystemWatcher;
    };
}