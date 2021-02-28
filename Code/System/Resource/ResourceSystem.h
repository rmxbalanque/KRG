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

namespace KRG
{
    class TaskScheduler;

    //-------------------------------------------------------------------------

    namespace Resource
    {
        class ResourceProvider;
        class ResourceLoader;
        class ResourceRequest;
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RESOURCE_API ResourceSystem : public ISystem
        {
            friend class ResourceDebugViewController;

            struct PendingRequest
            {
                enum class Type { Load, Unload };

            public:

                PendingRequest() = default;

                PendingRequest( Type type, ResourceRecord* pRecord, UUID const& requesterID )
                    : m_pRecord( pRecord )
                    , m_requesterID( requesterID )
                    , m_type( type )
                {
                    KRG_ASSERT( m_pRecord != nullptr );
                }

                ResourceRecord*     m_pRecord = nullptr;
                UUID                m_requesterID;
                Type                m_type = Type::Load;
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

            // Update all active requests
            void Update();

            // Blocking wait for all requests to be completed
            void WaitForAllRequestsToComplete();

            //-------------------------------------------------------------------------

            void RegisterResourceLoader( ResourceLoader* pLoader );
            void UnregisterResourceLoader( ResourceLoader* pLoader );

            //-------------------------------------------------------------------------

            // Request a load of a resource, can optionally provide a UUID for identification of the request source
            inline void LoadResource( ResourcePtr& resourcePtr, UUID const& requesterID = UUID() )
            {
                Threading::RecursiveScopeLock lock( m_accessLock );

                // Immediately update the resource ptr
                auto pRecord = FindOrCreateResourceRecord( resourcePtr.GetResourceID() );
                resourcePtr.m_pResource = pRecord;

                //-------------------------------------------------------------------------

                if ( !pRecord->HasReferences() )
                {
                    AddPendingRequest( PendingRequest( PendingRequest::Type::Load, pRecord, requesterID ) );
                }

                pRecord->AddReference( requesterID );
            }

            // Request an unload of a resource, can optionally provide a UUID for identification of the request source
            inline void UnloadResource( ResourcePtr& resourcePtr, UUID const& requesterID = UUID() )
            {
                Threading::RecursiveScopeLock lock( m_accessLock );

                // Immediately update the resource ptr
                resourcePtr.m_pResource = nullptr;

                //-------------------------------------------------------------------------

                auto pRecord = FindExistingResourceRecord( resourcePtr.GetResourceID() );
                pRecord->RemoveReference( requesterID );

                if ( !pRecord->HasReferences() )
                {
                    AddPendingRequest( PendingRequest( PendingRequest::Type::Unload, pRecord, requesterID ) );
                }
            }

            template<typename T> 
            inline void LoadResource( TResourcePtr<T>& resourcePtr, UUID const& userID = UUID() ) { LoadResource( (ResourcePtr&) resourcePtr, userID ); }

            template<typename T> 
            inline void UnloadResource( TResourcePtr<T>& resourcePtr, UUID const& userID = UUID() ) { UnloadResource( (ResourcePtr&) resourcePtr, userID ); }

            //-------------------------------------------------------------------------

            inline TVector<UUID> const& GetUsersThatRequireReload() const { return m_usersThatRequireReload; }

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
            void GetUsersForResource( ResourceRecord const* pResourceRecord, TVector<UUID>& userIDs ) const;

            // Called whenever a resource is changed externally and requires reloading
            void OnResourceExternallyUpdated( ResourceID const& resourceID );

            // Process all queued resource requests
            void ProcessResourceRequests();

        private:

            TaskSystem&                                             m_taskSystem;
            ResourceProvider*                                       m_pResourceProvider = nullptr;
            THashMap<ResourceTypeID, ResourceLoader*>               m_resourceLoaders;
            THashMap<ResourceID, ResourceRecord*>                   m_resourceRecords;
            mutable Threading::RecursiveMutex                       m_accessLock;

            // Hot reload
            TVector<UUID>                                           m_usersThatRequireReload;
            TVector<ResourceID>                                     m_externallyUpdatedResources;
            EventBindingID                                          m_resourceExternalUpdateEventBinding;

            // Requests
            TVector<PendingRequest>                                 m_pendingRequests;
            TVector<ResourceRequest*>                               m_activeRequests;
            TVector<ResourceRequest*>                               m_completedRequests;

            // ASync
            AsyncTask                                               m_asyncProcessingTask;
            bool                                                    m_isAsyncTaskRunning = false;

            #if KRG_DEVELOPMENT_TOOLS
            TVector<CompletedRequestLog>                            m_history;
            #endif
        };
    }
}