#include "ResourceSystem.h"
#include "ResourceProvider.h"
#include "ResourceRequest.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        ResourceSystem::ResourceSystem( TaskSystem& taskSystem )
            : m_taskSystem( taskSystem )
            , m_asyncProcessingTask( [this] ( TaskSetPartition range, U32 threadnum ) { ProcessResourceRequests(); } )
        {}

        ResourceSystem::~ResourceSystem()
        {
            KRG_ASSERT( m_pResourceProvider == nullptr && !IsBusy() && m_resourceRecords.empty() );
        }

        void ResourceSystem::Initialize( ResourceProvider* pResourceProvider )
        {
            KRG_ASSERT( pResourceProvider != nullptr && pResourceProvider->IsReady() );
            m_pResourceProvider = pResourceProvider;
            m_resourceExternalUpdateEventBinding = m_pResourceProvider->OnResourceExternallyUpdated().Bind( [this] ( ResourceID const& resourceID ) { OnResourceExternallyUpdated( resourceID ); } );
        }

        void ResourceSystem::Shutdown()
        {
            WaitForAllRequestsToComplete();
            m_pResourceProvider->OnResourceExternallyUpdated().Unbind( m_resourceExternalUpdateEventBinding );
            m_pResourceProvider = nullptr;
        }

        bool ResourceSystem::IsBusy() const
        {
            if ( m_isAsyncTaskRunning )
            {
                return true;
            }

            if ( !m_activeRequests.empty() )
            {
                return true;
            }

            if ( !m_pendingRequests.empty() )
            {
                return true;
            }

            return false;
        }

        //-------------------------------------------------------------------------

        void ResourceSystem::GetUsersForResource( ResourceRecord const* pResourceRecord, TVector<ResourceRequesterID>& userIDs ) const
        {
            KRG_ASSERT( pResourceRecord != nullptr );

            for ( auto const& requesterID : pResourceRecord->m_references )
            {
                // Internal user i.e. install dependency
                if ( requesterID.IsInstallDependencyRequest() )
                {
                    U32 const resourceDataPathID( requesterID.GetInstallDependencyDataPathID() );
                    auto const recordIter = m_resourceRecords.find_as( resourceDataPathID );
                    KRG_ASSERT( recordIter != m_resourceRecords.end() );

                    ResourceRecord* pFoundRecord = recordIter->second;
                    GetUsersForResource( pFoundRecord, userIDs );
                }
                else // Actual external user
                {
                    // Skip manual requests
                    if ( requesterID.IsManualRequest() )
                    {
                        continue;
                    }

                    // Add unique users to the list
                    if ( !VectorContains( userIDs, requesterID ) )
                    {
                        userIDs.emplace_back( requesterID );
                    }
                }
            }
        }

        void ResourceSystem::OnResourceExternallyUpdated( ResourceID const& resourceID )
        {
            KRG_ASSERT( resourceID.IsValid() );

            // If the resource is not currently in use then just early-out
            auto const recordIter = m_resourceRecords.find( resourceID );
            if ( recordIter == m_resourceRecords.end() )
            {
                return;
            }

            // Add to the list of resource that will require a force reload
            m_externallyUpdatedResources.emplace_back( resourceID );

            // Generate a list of users for this resource
            ResourceRecord* pRecord = recordIter->second;
            GetUsersForResource( pRecord, m_usersThatRequireReload );
        }

        //-------------------------------------------------------------------------

        void ResourceSystem::RegisterResourceLoader( ResourceLoader* pLoader )
        {
            auto& loadableTypes = pLoader->GetLoadableTypes();
            for ( auto& type : loadableTypes )
            {
                auto loaderIter = m_resourceLoaders.find( type );
                KRG_ASSERT( loaderIter == m_resourceLoaders.end() ); // Catch duplicate registrations
                m_resourceLoaders[type] = pLoader;
            }
        }

        void ResourceSystem::UnregisterResourceLoader( ResourceLoader* pLoader )
        {
            auto& loadableTypes = pLoader->GetLoadableTypes();
            for ( auto& type : loadableTypes )
            {
                auto loaderIter = m_resourceLoaders.find( type );
                KRG_ASSERT( loaderIter != m_resourceLoaders.end() ); // Catch invalid unregistrations
                m_resourceLoaders.erase( loaderIter );
            }
        }

        //-------------------------------------------------------------------------

        ResourceRecord* ResourceSystem::FindOrCreateResourceRecord( ResourceID const& resourceID )
        {
            KRG_ASSERT( resourceID.IsValid() );
            Threading::RecursiveScopeLock lock( m_accessLock );

            ResourceRecord* pRecord = nullptr;
            auto const recordIter = m_resourceRecords.find( resourceID );
            if ( recordIter == m_resourceRecords.end() )
            {
                pRecord = KRG::New<ResourceRecord>( resourceID );
                m_resourceRecords[resourceID] = pRecord;
            }
            else
            {
                pRecord = recordIter->second;
            }

            return pRecord;
        }

        ResourceRecord* ResourceSystem::FindExistingResourceRecord( ResourceID const& resourceID )
        {
            KRG_ASSERT( resourceID.IsValid() );
            Threading::RecursiveScopeLock lock( m_accessLock );

            auto const recordIter = m_resourceRecords.find( resourceID );
            KRG_ASSERT( recordIter != m_resourceRecords.end() );
            return recordIter->second;
        }

        void ResourceSystem::AddPendingRequest( PendingRequest&& request )
        {
            Threading::RecursiveScopeLock lock( m_accessLock );

            // Try find a pending request for this resource ID
            auto predicate = [] ( PendingRequest const& request, ResourceID const& resourceID ) { return request.m_pRecord->GetResourceID() == resourceID; };
            S32 const foundIdx = VectorFindIndex( m_pendingRequests, request.m_pRecord->GetResourceID(), predicate );

            // If we dont have a request for this resource ID create one
            if ( foundIdx == InvalidIndex )
            {
                m_pendingRequests.emplace_back( eastl::move( request ) );
            }
            else // overwrite exiting request
            {
                m_pendingRequests[foundIdx] = request;
            }
        }

        ResourceRequest* ResourceSystem::TryFindActiveRequest( ResourceRecord const* pResourceRecord ) const
        {
            KRG_ASSERT( pResourceRecord != nullptr );

            Threading::RecursiveScopeLock lock( m_accessLock );
            auto predicate = [] ( ResourceRequest const* pRequest, ResourceRecord const* pResourceRecord ) { return pRequest->GetResourceRecord() == pResourceRecord; };
            S32 const foundIdx = VectorFindIndex( m_activeRequests, pResourceRecord, predicate );

            if ( foundIdx != InvalidIndex )
            {
                return m_activeRequests[foundIdx];
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------

        void ResourceSystem::Update()
        {
            KRG_PROFILE_FUNCTION_RESOURCE();
            KRG_ASSERT( Threading::IsMainThread() );
            KRG_ASSERT( m_pResourceProvider != nullptr );

            // Clear any hot-reload data
            //-------------------------------------------------------------------------

            m_usersThatRequireReload.clear();
            m_externallyUpdatedResources.clear();

            // Update resource provider
            //-------------------------------------------------------------------------
            // This will potentially fill the hot-reload data

            m_pResourceProvider->Update();

            // Wait for async task to complete
            //-------------------------------------------------------------------------

            if ( m_isAsyncTaskRunning && !m_asyncProcessingTask.GetIsComplete() )
            {
                return;
            }

            m_isAsyncTaskRunning = false;

            // Process and Update requests
            //-------------------------------------------------------------------------

            {
                Threading::RecursiveScopeLock lock( m_accessLock );

                for ( auto& pendingRequest : m_pendingRequests )
                {
                    // Get existing active request
                    auto pActiveRequest = TryFindActiveRequest( pendingRequest.m_pRecord );

                    // Load request
                    if ( pendingRequest.m_type == PendingRequest::Type::Load )
                    {
                        if ( pActiveRequest != nullptr )
                        {
                            if ( pActiveRequest->IsUnloadRequest() )
                            {
                                pActiveRequest->SwitchToLoadTask();
                            }
                        }
                        else // Create new request
                        {
                            auto loaderIter = m_resourceLoaders.find( pendingRequest.m_pRecord->GetResourceTypeID() );
                            KRG_ASSERT( loaderIter != m_resourceLoaders.end() );
                            m_activeRequests.emplace_back( KRG::New<ResourceRequest>( pendingRequest.m_requesterID, pendingRequest.m_pRecord, loaderIter->second ) );
                        }
                    }
                    else // Unload request
                    {
                        if ( pActiveRequest != nullptr )
                        {
                            pActiveRequest->SwitchToUnloadTask();
                        }
                        else // Create new request
                        {
                            auto loaderIter = m_resourceLoaders.find( pendingRequest.m_pRecord->GetResourceTypeID() );
                            KRG_ASSERT( loaderIter != m_resourceLoaders.end() );
                            m_activeRequests.emplace_back( KRG::New<ResourceRequest>( pendingRequest.m_requesterID, pendingRequest.m_pRecord, loaderIter->second ) );
                        }
                    }
                }

                m_pendingRequests.clear();

                // Process completed requests
                //-------------------------------------------------------------------------

                for ( auto pCompletedRequest : m_completedRequests )
                {
                    ResourceID const resourceID = pCompletedRequest->GetResourceID();
                    KRG_ASSERT( pCompletedRequest->IsComplete() );

                    #if KRG_DEVELOPMENT_TOOLS
                    m_history.emplace_back( CompletedRequestLog( pCompletedRequest->IsLoadRequest() ? PendingRequest::Type::Load : PendingRequest::Type::Unload, resourceID ) );
                    #endif

                    if ( pCompletedRequest->IsUnloadRequest() )
                    {
                        // Find the resource record and erase it
                        auto recordIter = m_resourceRecords.find( resourceID );
                        KRG_ASSERT( recordIter != m_resourceRecords.end() );

                        // Check if we can remove the record, we may have had a load request for it in the meantime
                        auto pRecord = recordIter->second;
                        if ( !pRecord->HasReferences() )
                        {
                            KRG::Delete( pRecord );
                            m_resourceRecords.erase( recordIter );
                        }
                    }

                    // Delete request
                    KRG::Delete( pCompletedRequest );
                }

                m_completedRequests.clear();
            }

            // Kick off new async task
            //-------------------------------------------------------------------------

            if ( !m_activeRequests.empty() )
            {
                m_taskSystem.ScheduleTask( &m_asyncProcessingTask );
                m_isAsyncTaskRunning = true;
            }
        }

        void ResourceSystem::WaitForAllRequestsToComplete()
        {
            while ( IsBusy() )
            {
                Update();
            }
        }

        void ResourceSystem::ProcessResourceRequests()
        {
            KRG_PROFILE_FUNCTION_RESOURCE();

            //-------------------------------------------------------------------------

            // We dont have to worry about this loop even if the m_activeRequests array is modified from another thread since we only access the array in 2 places and both use locks
            for ( S32 i = (S32) m_activeRequests.size() - 1; i >= 0; i-- )
            {
                ResourceRequest::RequestContext context;
                context.m_createRawRequestRequestFunction = [this] ( ResourceRequest* pRequest ) { m_pResourceProvider->RequestRawResource( pRequest ); };
                context.m_cancelRawRequestRequestFunction = [this] ( ResourceRequest* pRequest ) { m_pResourceProvider->CancelRequest( pRequest ); };
                context.m_loadResourceFunction = [this] ( ResourceRequesterID const& requesterID, ResourcePtr& resourcePtr ) { LoadResource( resourcePtr, requesterID ); };
                context.m_unloadResourceFunction = [this] ( ResourceRequesterID const& requesterID, ResourcePtr& resourcePtr ) { UnloadResource( resourcePtr, requesterID ); };

                //-------------------------------------------------------------------------

                ResourceRequest* pRequest = m_activeRequests[i];
                if ( pRequest->IsActive() )
                {
                    if ( pRequest->Update( context ) )
                    {
                        // We need to process and remove completed requests at the next update stage since unload task may have queued unload requests which refer to the request's allocated memory
                        m_completedRequests.emplace_back( pRequest );
                        m_activeRequests.erase_unsorted( m_activeRequests.begin() + i );
                    }
                }
            }
        }
    }
}