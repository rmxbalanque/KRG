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
            , m_asyncProcessingTask( [this] ( TaskSetPartition range, U32 threadnum ) { Update(); } )
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
            WaitForTasksToComplete();

            m_pResourceProvider->OnResourceExternallyUpdated().Unbind( m_resourceExternalUpdateEventBinding );
            m_pResourceProvider = nullptr;
        }

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

        void ResourceSystem::LoadResourceInternal( UUID const& requesterID, ResourcePtr& resourcePtr )
        {
            KRG_ASSERT( m_pResourceProvider != nullptr && resourcePtr.GetResourceID().IsValid() );
            ResourceID const& resourceID = resourcePtr.GetResourceID();
            ResourceRecord* pRecord = nullptr;

            S32 const t = resourceID.GetID();

            // Create/Lookup record for this resource
            //-------------------------------------------------------------------------

            bool resourceLoadRequired = false;

            // Check if we already have a record for this resource
            auto const recordIter = m_resourceRecords.find( resourceID );
            if ( recordIter != m_resourceRecords.end() )
            {
                pRecord = recordIter->second;

                // If this record is unloaded or is unloading/pending unload (no references), we need to reload it
                if ( pRecord->IsUnloaded() )
                {
                    resourceLoadRequired = true;
                }
                else if ( !pRecord->HasReferences() )
                {
                    // Find the active request and switch it to a load
                    bool foundRequest = false;
                    for ( auto pRequest : m_activeRequests )
                    {
                        if ( pRequest->GetResourceID() == resourceID )
                        {
                            KRG_ASSERT( pRequest->m_stage == ResourceRequest::Stage::UninstallResource );
                            if ( VectorContains( m_externallyUpdatedResources, resourceID ) )
                            {
                                pRequest->SwitchToReloadTask();
                            }
                            else
                            {
                                pRequest->SwitchToLoadTask();
                            }

                            foundRequest = true;
                            break;
                        }
                    }

                    KRG_ASSERT( foundRequest );
                }
            }
            else // Create a new entry for the resource and request it
            {
                pRecord = KRG::New<ResourceRecord>( resourceID );
                m_resourceRecords[resourceID] = pRecord;
                resourceLoadRequired = true;
            }

            //-------------------------------------------------------------------------

            // Check that this is a valid resource type
            auto loaderIter = m_resourceLoaders.find( resourcePtr.GetResourceTypeID() );
            if ( loaderIter == m_resourceLoaders.end() )
            {
                KRG_LOG_ERROR( "Resource", "Unknown resource type (%s) for resource request: %s", resourcePtr.GetResourceTypeID().ToString().c_str(), resourcePtr.GetResourceID().ToString().c_str() );
                pRecord->SetLoadingStatus( LoadingStatus::Failed );
            }
            else // If we need to load the resource, create a new request for it
            {
                if ( resourceLoadRequired )
                {
                    auto& pRequest = m_activeRequests.emplace_back( KRG::New<ResourceRequest>( requesterID, pRecord, loaderIter->second ) );
                }
            }

            // Add a reference and return the record
            KRG_ASSERT( pRecord != nullptr );
            pRecord->AddReference( requesterID );
            resourcePtr.m_pResource = pRecord;
        }

        void ResourceSystem::UnloadResourceInternal( UUID const& requesterID, ResourcePtr& resourcePtr )
        {
            KRG_ASSERT( m_pResourceProvider != nullptr && resourcePtr.GetResourceID().IsValid() );
            auto resourceID = resourcePtr.GetResourceID();

            auto const recordIter = m_resourceRecords.find( resourceID );
            KRG_ASSERT( recordIter != m_resourceRecords.end() );

            S32 const t = resourceID.GetID();

            // Remove this resource ptr from the list of references
            auto pRecord = recordIter->second;
            KRG_ASSERT( !pRecord->IsUnloaded() );
            pRecord->RemoveReference( requesterID );
            resourcePtr.m_pResource = nullptr;

            // If we have no references to this resource, unload it
            if ( !pRecord->HasReferences() )
            {
                // Cancel currently loading tasks - switch load task to unload task
                if ( pRecord->IsLoading() )
                {
                    // Find the active request and switch it to a load
                    bool foundRequest = false;
                    for ( auto pRequest : m_activeRequests )
                    {
                        if ( pRequest->GetResourceID() == resourceID )
                        {
                            KRG_ASSERT( pRequest->IsLoadRequest() );
                            pRequest->SwitchToUnloadTask();
                            foundRequest = true;
                            break;
                        }
                    }

                    KRG_ASSERT( foundRequest );
                }
                // Schedule unload task
                else if ( pRecord->IsLoaded() )
                {
                    KRG_ASSERT( !pRecord->IsBeingProcessed() ); // Why are we trying to load/unload a resource that we are currently processing

                    // Create the unload request
                    auto loaderIter = m_resourceLoaders.find( resourceID.GetResourceTypeID() );
                    KRG_ASSERT( loaderIter != m_resourceLoaders.end() );
                    m_activeRequests.emplace_back( KRG::New<ResourceRequest>( requesterID, pRecord, loaderIter->second ) );
                }
                else if ( pRecord->HasLoadingFailed() )
                {
                    // Immediately clear the loading status for resources that failed to load and remove the record
                    pRecord->SetLoadingStatus( LoadingStatus::Unloaded );
                    KRG::Delete( recordIter->second );
                    m_resourceRecords.erase( recordIter );
                }
                else
                {
                    KRG_HALT();
                }
            }
        }

        //-------------------------------------------------------------------------

        void ResourceSystem::ExecuteTasks()
        {
            KRG_ASSERT( m_pResourceProvider != nullptr && Threading::IsMainThread() );
            KRG_ASSERT( !m_asyncTaskScheduled );

            m_taskSystem.ScheduleTask( &m_asyncProcessingTask );
            m_asyncTaskScheduled = true;
        }

        void ResourceSystem::ExecuteTasksSynchronously()
        {
            WaitForTasksToComplete();
            Update();
        }

        void ResourceSystem::WaitForTasksToComplete()
        {
            if ( m_asyncTaskScheduled )
            {
                m_taskSystem.WaitForTask( &m_asyncProcessingTask );
                m_asyncTaskScheduled = false;
            }
        }

        //-------------------------------------------------------------------------

        void ResourceSystem::Update()
        {
            KRG_PROFILE_GROUPED_SCOPE_COLOR( "Resource", "ResourceSystem::Update", MP_CYAN );
            KRG_ASSERT( m_pResourceProvider != nullptr );

            // Clear any hot-reload data
            //-------------------------------------------------------------------------

            m_usersThatRequireReload.clear();
            m_externallyUpdatedResources.clear();

            // Update resource provider
            //-------------------------------------------------------------------------
            // This will load any raw resource data requested

            m_pResourceProvider->Update();

            // Process active requests
            //-------------------------------------------------------------------------

            for ( S32 i = (S32) m_activeRequests.size() - 1; i >= 0; i-- )
            {
                ResourceRequest::RequestContext context;
                context.m_createRawRequestRequestFunction = [this] ( ResourceRequest* pRequest ) { m_pResourceProvider->RequestRawResource( pRequest ); };
                context.m_cancelRawRequestRequestFunction = [this] ( ResourceRequest* pRequest ) { m_pResourceProvider->CancelRequest( pRequest ); };
                context.m_loadResourceFunction = [this] ( UUID const& requesterID, ResourcePtr& resourcePtr ) { LoadResourceInternal( requesterID, resourcePtr ); };
                context.m_unloadResourceFunction = [this] ( UUID const& requesterID, ResourcePtr& resourcePtr ) { UnloadResourceInternal( requesterID, resourcePtr ); };

                auto pRequest = m_activeRequests[i];
                if ( pRequest->IsActive() )
                {
                    pRequest->Update( context );
                }

                //-------------------------------------------------------------------------

                if ( pRequest->IsComplete() )
                {
                    if ( pRequest->IsLoadRequest() )
                    {
                        KRG_LOG_MESSAGE( "Resource", "Load Request Complete: %s", pRequest->GetResourceID().GetDataPath().c_str() );
                    }
                    else // Unload request
                    {
                        ResourceID const resourceID = pRequest->GetResourceID();

                        // Find the resource record and erase it
                        auto recordIter = m_resourceRecords.find( resourceID );
                        KRG_ASSERT( recordIter != m_resourceRecords.end() && recordIter->second == pRequest->m_pResourceRecord );
                        KRG::Delete( recordIter->second );
                        m_resourceRecords.erase( recordIter );

                        KRG_LOG_MESSAGE( "Resource", "Unload Request Complete: %s", resourceID.GetDataPath().c_str() );
                    }

                    // Delete request and remove it from the active list
                    KRG::Delete( pRequest );
                    VectorEraseUnsorted( m_activeRequests, i );
                }
            }
        }

        void ResourceSystem::GetUsersForResource( ResourceRecord const* pResourceRecord, TVector<UUID>& userIDs ) const
        {
            KRG_ASSERT( pResourceRecord != nullptr );

            for ( auto const& requesterID : pResourceRecord->m_references )
            {
                auto IsValidInstallDependencyUUID = [] ( UUID const& ID )
                {
                    return ID.GetValueU32( 0 ) == 0 && ID.GetValueU32( 1 ) == 0 && ID.GetValueU32( 2 ) == 0 && ID.GetValueU32( 3 ) != 0;
                };

                // Internal user i.e. install dependency
                if ( IsValidInstallDependencyUUID( requesterID ) )
                {
                    U32 const resourceDataPathID( requesterID.GetValueU32( 3 ) );
                    auto const recordIter = m_resourceRecords.find_as( resourceDataPathID );
                    KRG_ASSERT( recordIter != m_resourceRecords.end() );

                    ResourceRecord* pFoundRecord = recordIter->second;
                    GetUsersForResource( pFoundRecord, userIDs );
                }
                else // Actual external user
                {
                    // Add unique users to the list
                    if ( requesterID.IsValid() && !VectorContains( userIDs, requesterID ) )
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
    }
}