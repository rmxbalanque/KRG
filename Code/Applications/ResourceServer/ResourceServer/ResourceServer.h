#pragma once
#include "CompilerInfo.h"
#include "CompilationWorker.h"
#include "CompilationRequest.h"
#include "CompiledResourceDatabase.h"
#include "System/Network/IPC/IPCMessageServer.h"
#include "System/Core/Settings/SettingsRegistry.h"

#include <QTimer>
#include "Tools/Core/FileSystem/FileSystemWatcher.h"

//-------------------------------------------------------------------------
// The network resource server
//-------------------------------------------------------------------------
// Receives resource requests, triggers the compilation of said requests and returns the results
// Runs in a separate thread from the main UI
//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class ResourceServer : public QObject, public FileSystem::IFileSystemChangeListener
        {
            Q_OBJECT

            struct ClientRecord
            {
                explicit ClientRecord( U64 clientID ) : m_clientID( clientID ) { KRG_ASSERT( clientID != 0 ); }

                Seconds                             m_lastUpdateTime = Seconds::Now();
                U64                                 m_clientID = 0;
            };

        signals:

            // Has the active request list been updated
            void ActiveRequestsUpdated( TVector<CompilationRequest const*> const* pendingRequests, TVector<CompilationRequest const*> const* activeRequests );
            
            // Has the completed requests list been updated
            void CompletedRequestsUpdated( TVector<CompilationRequest const*> const* completedRequests );

            // Has the overall busy state of the resource server changed
            void BusyStateChanged();

        public slots:

            void OnThreadStarted();
            void OnThreadStopped();
            void OnCleanupOldRequests() { m_cleanupRequested = true; }

        public:

            ResourceServer();

            bool Initialize();
            void Shutdown();

            inline String const& GetErrorMessage() const { return m_errorMessage; }
            inline String const& GetNetworkAddress() const { return m_networkAddress; }
            inline FileSystemPath const& GetSourceDataDir() const { return m_sourceDataDir; }
            inline FileSystemPath const& GetCompiledDataDir() const { return m_compiledDataDir; }

            inline TVector<CompilerInfo> const& GetKnownCompilers() const { return m_compilers; }

            TVector<CompilationRequest const*> const& GetRequests() const { return ( TVector<CompilationRequest const*>& ) m_completedRequests; }

            inline bool IsBusy() const { return m_pendingRequests.size() + m_activeRequests.size() > 0; }

            inline S32 GetNumWorkers() const { return (S32) m_workers.size(); }
            inline CompilationRequest* GetActiveRequestForWorker( U32 workerIdx ) const { KRG_ASSERT( workerIdx < (U32) GetNumWorkers() ); return m_workers[workerIdx]->GetActiveRequest(); }

        private slots:

            void Execute();

        private:

            void CleanupCompletedRequests();

            // Compiler Info
            bool HasCompilerForType( ResourceTypeID typeID ) const;
            S32 GetCompilerVersion( ResourceTypeID typeID ) const;

            // Request Actions
            CompilationRequest const* ProcessResourceRequest( ResourceID const& resourceID, U64 clientID = 0 );
            void NotifyClientOnCompletedRequest( CompilationRequest* pRequest );

            // Up-to-date system
            void PerformResourceUpToDateCheck( CompilationRequest* pRequest, TVector<DataPath> const& compileDependencies ) const;
            bool TryReadCompileDependencies( FileSystemPath const& resourceFilePath, TVector<DataPath>& outDependencies, String* pErrorLog = nullptr ) const;
            bool IsResourceUpToDate( ResourceID const& resourceID ) const;
            void WriteCompiledResourceRecord( CompilationRequest* pRequest );
            bool IsCompileableResourceType( ResourceTypeID ID ) const;

            // File system listener
            virtual void OnFileModified( FileSystemPath const& filePath ) override final;

        private:

            QTimer*                                 m_pLoopTimer = nullptr;

            String                                  m_errorMessage;
            bool                                    m_cleanupRequested = false;
            Network::IPC::Server                    m_networkServer;
            TVector<ClientRecord>                   m_knownClients;

            // Settings
            SettingsRegistry                        m_settingsRegistry;
            FileSystemPath                          m_workingDir;
            FileSystemPath                          m_sourceDataDir;
            FileSystemPath                          m_compiledDataDir;
            FileSystemPath                          m_compiledResourceDatabasePath;
            String                                  m_networkAddress;
            String                                  m_workerFullPath;
            U32                                     m_maxSimultaneousCompilationTasks = 16;

            // Compiler list
            TVector<CompilerInfo>                   m_compilers;

            // Compilation Requests
            CompiledResourceDatabase                m_compiledResourceDatabase;
            TVector<CompilationRequest*>            m_completedRequests;
            TVector<CompilationRequest*>            m_pendingRequests;
            TVector<CompilationRequest*>            m_activeRequests;
            TVector<CompilationWorker*>             m_workers;

            // File System Watcher
            FileSystem::FileSystemWatcher           m_fileSystemWatcher;
        };
    }
}