#include "ResourceRequest.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        ResourceRequest::ResourceRequest( UUID const& userID, ResourceRecord* pRecord, ResourceLoader* pResourceLoader )
            : m_userID( userID )
            , m_pResourceRecord( pRecord )
            , m_pResourceLoader( pResourceLoader )
        {
            KRG_ASSERT( m_pResourceRecord != nullptr && m_pResourceRecord->IsValid() );
            KRG_ASSERT( m_pResourceRecord->IsLoaded() || m_pResourceRecord->IsUnloaded() );
            KRG_ASSERT( !m_pResourceRecord->HasReferences() );
            KRG_ASSERT( pResourceLoader != nullptr );

            if ( m_pResourceRecord->IsLoaded() )
            {
                m_type = Type::Unload;
                m_stage = Stage::UninstallResource;
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Unloading );
            }
            else if( m_pResourceRecord->IsUnloaded() )
            {
                m_type = Type::Load;
                m_stage = Stage::RequestRawResource;
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Loading );
            }
        }

        void ResourceRequest::OnRawResourceRequestComplete( TVector<Byte>&& rawResourceData )
        {
            // Raw resource failed to load
            if ( rawResourceData.empty() )
            {
                m_stage = ResourceRequest::Stage::Complete;
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
            }
            else // Continue the load operation
            {
                m_rawResourceData.swap( rawResourceData );
                m_stage = ResourceRequest::Stage::LoadResource;
            }
        }

        void ResourceRequest::SwitchToLoadTask()
        {
            KRG_ASSERT( m_type == Type::Unload );
            KRG_ASSERT( !m_isReloadRequest );

            m_type = Type::Load;
            m_pResourceRecord->SetLoadingStatus( LoadingStatus::Loading );

            //-------------------------------------------------------------------------

            switch ( m_stage )
            {
                case Stage::Complete:
                {
                    m_stage = Stage::RequestRawResource;
                }
                break;

                case Stage::UninstallResource:
                {
                    m_stage = Stage::Complete;
                    m_pResourceRecord->SetLoadingStatus( LoadingStatus::Loaded );
                }
                break;

                case Stage::CancelWaitForLoadDependencies:
                {
                    m_stage = Stage::WaitForLoadDependencies;
                }
                break;

                case Stage::UnloadDependencies:
                {
                    m_stage = Stage::InstallResource;
                }
                break;

                case Stage::UnloadResource:
                {
                    m_stage = Stage::LoadDependencies;
                }
                break;

                default:
                KRG_HALT();
                break;
            }
        }

        void ResourceRequest::SwitchToUnloadTask()
        {
            KRG_ASSERT( m_type == Type::Load );
            KRG_ASSERT( !m_pResourceRecord->HasReferences() );

            m_type = Type::Unload;
            m_pResourceRecord->SetLoadingStatus( LoadingStatus::Unloading );

            //-------------------------------------------------------------------------

            switch ( m_stage )
            {
                case Stage::WaitForRawResourceRequest:
                {
                    m_stage = Stage::CancelRawResourceRequest;
                }
                break;

                case Stage::LoadResource:
                {
                    m_rawResourceData.clear();
                    m_stage = Stage::Complete;
                    m_pResourceRecord->SetLoadingStatus( LoadingStatus::Unloaded );
                }
                break;

                case Stage::LoadDependencies:
                {
                    m_stage = Stage::UnloadResource;
                }
                break;

                case Stage::WaitForLoadDependencies:
                {
                    m_stage = Stage::CancelWaitForLoadDependencies;
                }

                case Stage::InstallResource:
                {
                    m_stage = Stage::UnloadDependencies;
                }
                break;

                case Stage::Complete:
                {
                    m_stage = Stage::UninstallResource;
                }
                break;

                default:
                KRG_HALT();
                break;
            }
        }

        void ResourceRequest::SwitchToReloadTask()
        {
            m_isReloadRequest = true;

            // If we are currently loading, switch to an unload
            if ( m_type == Type::Load )
            {
                SwitchToUnloadTask();
            }
        }

        //-------------------------------------------------------------------------

        void ResourceRequest::Update( RequestContext& requestContext )
        {
            switch ( m_stage )
            {
                case ResourceRequest::Stage::RequestRawResource:
                {
                    RequestRawResource( requestContext );
                }
                break;

                case ResourceRequest::Stage::WaitForRawResourceRequest:
                {
                    // Do Nothing
                }
                break;

                case ResourceRequest::Stage::LoadResource:
                {
                    LoadResource( requestContext );
                }
                break;

                case ResourceRequest::Stage::LoadDependencies:
                {
                    LoadDependencies( requestContext );
                }
                break;

                case ResourceRequest::Stage::WaitForLoadDependencies:
                {
                    WaitForLoadDependencies( requestContext );
                }
                break;

                case ResourceRequest::Stage::InstallResource:
                {
                    InstallResource( requestContext );
                }
                break;

                case ResourceRequest::Stage::UninstallResource:
                {
                    // Execute all unload operations immediately
                    UninstallResource( requestContext );
                    UnloadDependencies( requestContext );
                    UnloadResource( requestContext );
                }
                break;

                case ResourceRequest::Stage::UnloadDependencies:
                {
                    // Execute all unload operations immediately
                    UnloadDependencies( requestContext );
                    UnloadResource( requestContext );
                }
                break;

                case ResourceRequest::Stage::UnloadResource:
                {
                    UnloadResource( requestContext );
                }
                break;

                case ResourceRequest::Stage::CancelWaitForLoadDependencies:
                {
                    // Execute all unload operations immediately
                    m_installDependencies.clear();
                    m_stage = Stage::UnloadDependencies;
                    UnloadDependencies( requestContext );
                    UnloadResource( requestContext );
                }
                break;

                case ResourceRequest::Stage::CancelRawResourceRequest:
                {
                    CancelRawRequestRequest( requestContext );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }

        //-------------------------------------------------------------------------

        void ResourceRequest::RequestRawResource( RequestContext& requestContext )
        {
            requestContext.m_createRawRequestRequestFunction( this );
            m_stage = ResourceRequest::Stage::WaitForRawResourceRequest;
        }

        void ResourceRequest::LoadResource( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::LoadResource );

            if ( m_pResourceLoader->Load( GetResourceID(), m_rawResourceData, m_pResourceRecord ) )
            {
                m_rawResourceData.clear();
                m_stage = ResourceRequest::Stage::LoadDependencies;
            }
            else
            {
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
                m_pResourceLoader->Unload( GetResourceID(), m_pResourceRecord );
                m_stage = ResourceRequest::Stage::Complete;
            }
        }

        void ResourceRequest::LoadDependencies( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::LoadDependencies );

            // Create the resource ptrs for the install dependencies and request the load
            // These resource ptrs are temporary and will be clear upon completion of the request
            U32 const numInstallDependencies = (U32) m_pResourceRecord->m_installDependencyResourceIDs.size();
            m_installDependencies.resize( numInstallDependencies );
            for ( U32 i = 0; i < numInstallDependencies; i++ )
            {
                // Do not use the user ID for install dependencies! Since they are not explicitly loaded by a specific user!
                // Instead we create a UUID from the depending resource's resourceID
                m_installDependencies[i] = ResourcePtr( m_pResourceRecord->m_installDependencyResourceIDs[i] );
                UUID const installDependencyUUID( 0, 0, 0, m_pResourceRecord->GetResourceID().GetDataPath().GetID() );
                requestContext.m_loadResourceFunction( installDependencyUUID, m_installDependencies[i] );
            }
            m_stage = ResourceRequest::Stage::WaitForLoadDependencies;
        }

        void ResourceRequest::WaitForLoadDependencies( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::WaitForLoadDependencies );

            enum class InstallStatus
            {
                Loading,
                ShouldProceed,
                ShouldFail,
            };

            // Check if all dependencies are finished installing
            auto status = InstallStatus::ShouldProceed;
            for ( auto const& installDependency : m_installDependencies )
            {
                KRG_ASSERT( !installDependency.IsUnloaded() );

                if ( installDependency.IsLoading() )
                {
                    status = InstallStatus::Loading;
                    break;
                }
                else if ( installDependency.HasLoadingFailed() )
                {
                    KRG_LOG_ERROR( "Resource", "Failed to load install dependency: %s", installDependency.GetResourceID().ToString().c_str() );
                    status = InstallStatus::ShouldFail;
                    break;
                }
            }

            // If dependency has failed, the resource has failed to load so immediately unload and set status to failed
            if ( status == InstallStatus::ShouldFail )
            {
                // Unload the resource data
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Unloading );
                m_installDependencies.clear();
                m_stage = Stage::UnloadDependencies;
                UnloadDependencies( requestContext );
                UnloadResource( requestContext );

                // Complete request
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
                m_stage = ResourceRequest::Stage::Complete;
            }
            // Install runtime resource
            else if ( status == InstallStatus::ShouldProceed )
            {
                m_stage = ResourceRequest::Stage::InstallResource;
            }
        }

        void ResourceRequest::InstallResource( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::InstallResource );

            if ( m_pResourceLoader->Install( GetResourceID(), m_pResourceRecord, m_installDependencies ) )
            {
                KRG_ASSERT( m_pResourceRecord->GetResourceData() != nullptr );
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Loaded );
            }
            else // Install operation failed, unload resource and set status to failed
            {
                UnloadDependencies( requestContext );
                UnloadResource( requestContext );
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
            }

            m_stage = ResourceRequest::Stage::Complete;
            m_installDependencies.clear();
        }

        void ResourceRequest::UninstallResource( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::UninstallResource );

            if ( m_pResourceRecord->IsLoaded() )
            {
                m_pResourceLoader->Uninstall( GetResourceID(), m_pResourceRecord );
            }

            m_stage = ResourceRequest::Stage::UnloadDependencies;
        }

        void ResourceRequest::UnloadDependencies( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::UnloadDependencies );

            // Create the resource ptrs for the install dependencies and request the unload
            // These resource ptrs are temporary and will be cleared upon completion of the request
            U32 const numInstallDependencies = (U32) m_pResourceRecord->m_installDependencyResourceIDs.size();
            m_installDependencies.resize( numInstallDependencies );
            for ( U32 i = 0; i < numInstallDependencies; i++ )
            {
                // Do not use the user ID for install dependencies! Since they are not explicitly loaded by a specific user!
                // Instead we create a UUID from the depending resource's resourceID
                m_installDependencies[i] = ResourcePtr( m_pResourceRecord->m_installDependencyResourceIDs[i] );
                UUID const installDependencyUUID( 0, 0, 0, m_pResourceRecord->GetResourceID().GetDataPath().GetID() );
                requestContext.m_unloadResourceFunction( installDependencyUUID, m_installDependencies[i] );
            }

            m_installDependencies.clear();
            m_stage = ResourceRequest::Stage::UnloadResource;
        }

        void ResourceRequest::UnloadResource( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::UnloadResource );

            S32 const t = m_pResourceRecord->GetResourceID().GetID();

            KRG_ASSERT( m_pResourceRecord->IsUnloading() );
            m_pResourceLoader->Unload( GetResourceID(), m_pResourceRecord );
            m_pResourceRecord->SetLoadingStatus( LoadingStatus::Unloaded );

            m_stage = ResourceRequest::Stage::Complete;

            if ( m_isReloadRequest )
            {
                // Clear the flag here, in case we re-used this request again
                m_isReloadRequest = false;
                SwitchToLoadTask();
            }
        }

        void ResourceRequest::CancelRawRequestRequest( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::CancelRawResourceRequest );

            requestContext.m_cancelRawRequestRequestFunction( this );
            m_stage = ResourceRequest::Stage::Complete;
        }
    }
}