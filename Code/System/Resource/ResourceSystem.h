#pragma once

#include "_Module/API.h"
#include "ResourcePtr.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Threading/TaskSystem.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Types/Event.h"

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
        public:

            KRG_SYSTEM_ID( ResourceSystem );

        public:

            ResourceSystem( TaskSystem& taskSystem );
            ~ResourceSystem();

            inline bool IsInitialized() const { return m_pResourceProvider != nullptr; }
            void Initialize( ResourceProvider* pResourceProvider );
            void Shutdown();

            // Do we still have work we need ot perform
            bool IsBusy() const { return !m_activeRequests.empty(); }

            // Schedule the resource system update to run asynchronously
            void ExecuteTasks();

            // Execute the resource system update synchronously (blocking)
            void ExecuteTasksSynchronously();

            // Wait for the scheduled work to complete
            void WaitForTasksToComplete();

            //-------------------------------------------------------------------------

            void RegisterResourceLoader( ResourceLoader* pLoader );
            void UnregisterResourceLoader( ResourceLoader* pLoader );

            //-------------------------------------------------------------------------

            // Request a load of a resource, can optionally provide a UUID for identification of the request source
            inline void LoadResource( ResourcePtr& resourcePtr, UUID const& requesterID = UUID() )
            {
                KRG_ASSERT( Threading::IsMainThread() ); // We dont support requesting new resource from outside the main thread
                LoadResourceInternal( requesterID, resourcePtr );
            }

            // Request an unload of a resource, can optionally provide a UUID for identification of the request source
            inline void UnloadResource( ResourcePtr& resourcePtr, UUID const& requesterID = UUID() )
            {
                KRG_ASSERT( Threading::IsMainThread() ); // We dont support unloading resources from outside the main thread
                UnloadResourceInternal( requesterID, resourcePtr );
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

            // The actual loading/unloading functions
            void LoadResourceInternal( UUID const& requesterID, ResourcePtr& resourcePtr );
            void UnloadResourceInternal( UUID const& requesterID, ResourcePtr& resourcePtr );

            // The actual resource system update can be run both synchronously or asynchronously
            void Update();

            // Returns a list of all unique external references for the given resource
            void GetUsersForResource( ResourceRecord const* pResourceRecord, TVector<UUID>& userIDs ) const;

            // Called whenever a resource is changed externally and requires reloading
            void OnResourceExternallyUpdated( ResourceID const& resourceID );

        private:

            TaskSystem&                                             m_taskSystem;
            AsyncTask                                               m_asyncProcessingTask;
            bool                                                    m_asyncTaskScheduled = false;

            ResourceProvider*                                       m_pResourceProvider = nullptr;
            THashMap<ResourceTypeID, ResourceLoader*>               m_resourceLoaders;
            THashMap<ResourceID, ResourceRecord*>                   m_resourceRecords;
            TVector<ResourceRequest*>                               m_activeRequests;

            TVector<UUID>                                           m_usersThatRequireReload;
            TVector<ResourceID>                                     m_externallyUpdatedResources;
            EventBindingID                                          m_resourceExternalUpdateEventBinding;
        };
    }
}