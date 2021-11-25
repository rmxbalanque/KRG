#pragma once

#include "ResourceServerWorker.h"
#include "ResourceCompilationRequest.h"
#include "CompiledResourceDatabase.h"
#include "Tools/Core/FileSystem/FileSystemWatcher.h"
#include "Tools/Core/Resource/Compilers/ResourceCompilerRegistry.h"
#include "Tools/Animation/_Module/Module.h"
#include "Tools/Render/_Module/Module.h"
#include "Tools/Physics/_Module/Module.h"
#include "Tools/Entity/_Module/Module.h"
#include "System/Network/IPC/IPCMessageServer.h"
#include "System/Resource/ResourceSettings.h"
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

    public:

        ResourceServer();

        bool Initialize( Settings const& settings );
        void Shutdown();
        void Update();

        inline bool IsBusy() const { return m_pendingRequests.size() + m_activeRequests.size() > 0; }

        inline String const& GetErrorMessage() const { return m_errorMessage; }
        inline String const& GetNetworkAddress() const { return m_pSettings->m_resourceServerNetworkAddress; }
        inline uint16 GetNetworkPort() const { return m_pSettings->m_resourceServerPort; }
        inline FileSystem::Path const& GetRawResourceDir() const { return m_pSettings->m_rawResourcePath; }
        inline FileSystem::Path const& GetCompiledResourceDir() const { return m_pSettings->m_compiledResourcePath; }

        // Compilers and Compilation
        inline CompilerRegistry const* GetCompilerRegistry() const { return &m_compilerRegistry; }
        inline void RecompileResource( ResourceID const& resourceID ) { ProcessResourceRequest( resourceID, 0, true ); }

        // Requests
        TVector<CompilationRequest const*> const& GetActiveRequests() const { return ( TVector<CompilationRequest const*>& ) m_activeRequests; }
        TVector<CompilationRequest const*> const& GetPendingRequests() const { return ( TVector<CompilationRequest const*>& ) m_pendingRequests; }
        TVector<CompilationRequest const*> const& GetCompletedRequests() const { return ( TVector<CompilationRequest const*>& ) m_completedRequests; }
        inline void RequestCleanupOfCompletedRequests() { m_cleanupRequested = true; }

        // Workers
        inline int32 GetNumWorkers() const { return (int32) m_workers.size(); }
        inline ResourceServerWorker::Status GetWorkerStatus( int32 workerIdx ) const { return m_workers[workerIdx]->GetStatus(); }
        inline ResourceID const& GetCompilationTaskResourceID( int32 workerIdx ) const { return m_workers[workerIdx]->GetRequestResourceID(); }

        // Clients
        inline int32 GetNumConnectedClients() const { return m_networkServer.GetNumConnectedClients(); }
        inline uint32 GetClientID( int32 clientIdx ) const { return m_networkServer.GetClientID( clientIdx ); }
        inline Network::AddressString GetConnectedClientAddress( int32 clientIdx ) const { return m_networkServer.GetConnectedClientAddress( clientIdx ); }

    private:

        void CleanupCompletedRequests();

        // Request Actions
        void ProcessResourceRequest( ResourceID const& resourceID, uint32 clientID = 0, bool forceRecompile = false );
        void NotifyClientOnCompletedRequest( CompilationRequest* pRequest );

        // Up-to-date system
        void PerformResourceUpToDateCheck( CompilationRequest* pRequest, TVector<ResourcePath> const& compileDependencies ) const;
        bool TryReadCompileDependencies( FileSystem::Path const& resourceFilePath, TVector<ResourcePath>& outDependencies, String* pErrorLog = nullptr ) const;
        bool IsResourceUpToDate( ResourceID const& resourceID ) const;
        void WriteCompiledResourceRecord( CompilationRequest* pRequest );
        bool IsCompileableResourceType( ResourceTypeID ID ) const;

        // File system listener
        virtual void OnFileModified( FileSystem::Path const& filePath ) override final;

    private:

        String                                  m_errorMessage;
        bool                                    m_cleanupRequested = false;
        Network::IPC::Server                    m_networkServer;

        // Settings
        Settings const*                         m_pSettings = nullptr;
        uint32                                  m_maxSimultaneousCompilationTasks = 16;

        // Compilers
        Animation::ToolsModule                  m_animationModule;
        Render::ToolsModule                     m_renderModule;
        Physics::ToolsModule                    m_physicsModule;
        EntityModel::ToolsModule                m_entityModule;
        CompilerRegistry                        m_compilerRegistry;

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