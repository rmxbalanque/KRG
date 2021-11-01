#pragma once

#include "_Module/API.h"
#include "ResourcePtr.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Threading/TaskSystem.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Types/Event.h"
#include "System/Core/Time/TimeStamp.h"
#include <ctime>

//-------------------------------------------------------------------------

namespace KRG { class TaskScheduler; }

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceProvider;
    class ResourceLoader;
    class ResourceRequest;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RESOURCE_API ResourceSystem : public ISystem
    {
        friend class ResourceDebugView;

        struct PendingRequest
        {
            enum class Type { Load, Unload };

        public:

            PendingRequest() = default;

            PendingRequest( Type type, ResourceRecord* pRecord, ResourceRequesterID const& requesterID )
                : m_pRecord( pRecord )
                , m_requesterID( requesterID )
                , m_type( type )
            {
                KRG_ASSERT( m_pRecord != nullptr );
            }

            ResourceRecord*         m_pRecord = nullptr;
            ResourceRequesterID     m_requesterID;
            Type                    m_type = Type::Load;
        };

        #if KRG_DEVELOPMENT_TOOLS
        struct CompletedRequestLog
        {
            CompletedRequestLog( PendingRequest::Type type, ResourceID ID ) : m_type( type ), m_ID( ID ) {}

            PendingRequest::Type    m_type;
            ResourceID              m_ID;
            TimeStamp               m_time;
        };
        #endif

    public:

        KRG_SYSTEM_ID( ResourceSystem );

    public:

        ResourceSystem( TaskSystem& taskSystem );
        ~ResourceSystem();

        inline bool IsInitialized() const { return m_pResourceProvider != nullptr; }
        void Initialize( ResourceProvider* pResourceProvider );
        void Shutdown();

        // Do we still have work we need to perform
        bool IsBusy() const;

        // Update all active requests (if you set the 'waitForAsyncTask' to true, this function will block and wait for the async task to complete)
        void Update( bool waitForAsyncTask = false );

        // Blocking wait for all requests to be completed
        void WaitForAllRequestsToComplete();

        // Resource Loaders
        //-------------------------------------------------------------------------

        void RegisterResourceLoader( ResourceLoader* pLoader );
        void UnregisterResourceLoader( ResourceLoader* pLoader );

        // Loading/Unloading
        //-------------------------------------------------------------------------

        // Request a load of a resource, can optionally provide a ResourceRequesterID for identification of the request source
        void LoadResource( ResourcePtr& resourcePtr, ResourceRequesterID const& requesterID = ResourceRequesterID() );

        // Request an unload of a resource, can optionally provide a ResourceRequesterID for identification of the request source
        void UnloadResource( ResourcePtr& resourcePtr, ResourceRequesterID const& requesterID = ResourceRequesterID() );

        template<typename T>
        inline void LoadResource( TResourcePtr<T>& resourcePtr, ResourceRequesterID const& requesterID = ResourceRequesterID() ) { LoadResource( (ResourcePtr&) resourcePtr, requesterID ); }

        template<typename T>
        inline void UnloadResource( TResourcePtr<T>& resourcePtr, ResourceRequesterID const& requesterID = ResourceRequesterID() ) { UnloadResource( (ResourcePtr&) resourcePtr, requesterID ); }

        // Hot Reload
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline bool RequiresHotReloading() const { return !m_usersThatRequireReload.empty(); }
        inline TVector<ResourceRequesterID> const& GetUsersToBeReloaded() const { return m_usersThatRequireReload; }
        inline TVector<ResourceID> const& GetResourcesToBeReloaded() const { return m_externallyUpdatedResources; }
        void ClearHotReloadRequests();
        #endif

    private:

        // Non-copyable
        ResourceSystem( const ResourceSystem& ) = delete;
        ResourceSystem( const ResourceSystem&& ) = delete;
        ResourceSystem& operator=( const ResourceSystem& ) = delete;
        ResourceSystem& operator=( const ResourceSystem&& ) = delete;

        ResourceRecord* FindOrCreateResourceRecord( ResourceID const& resourceID );
        ResourceRecord* FindExistingResourceRecord( ResourceID const& resourceID );

        void AddPendingRequest( PendingRequest&& request );
        ResourceRequest* TryFindActiveRequest( ResourceRecord const* pResourceRecord ) const;

        // Returns a list of all unique external references for the given resource
        void GetUsersForResource( ResourceRecord const* pResourceRecord, TVector<ResourceRequesterID>& requesterIDs ) const;

        // Process all queued resource requests
        void ProcessResourceRequests();

        #if KRG_DEVELOPMENT_TOOLS
        // Called whenever a resource is changed externally and requires reloading
        void OnResourceExternallyUpdated( ResourceID const& resourceID );
        #endif

    private:

        TaskSystem&                                             m_taskSystem;
        ResourceProvider*                                       m_pResourceProvider = nullptr;
        THashMap<ResourceTypeID, ResourceLoader*>               m_resourceLoaders;
        THashMap<ResourceID, ResourceRecord*>                   m_resourceRecords;
        mutable Threading::RecursiveMutex                       m_accessLock;

        // Requests
        TVector<PendingRequest>                                 m_pendingRequests;
        TVector<ResourceRequest*>                               m_activeRequests;
        TVector<ResourceRequest*>                               m_completedRequests;

        // ASync
        AsyncTask                                               m_asyncProcessingTask;
        bool                                                    m_isAsyncTaskRunning = false;

        #if KRG_DEVELOPMENT_TOOLS
        // Hot reload
        TVector<ResourceRequesterID>                            m_usersThatRequireReload;
        TVector<ResourceID>                                     m_externallyUpdatedResources;
        EventBindingID                                          m_resourceExternalUpdateEventBinding;

        TVector<CompletedRequestLog>                            m_history;
        #endif
    };
}