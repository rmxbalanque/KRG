#include "ResourceRequest.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        ResourceRequest::ResourceRequest( ResourceRequesterID const& requesterID, ResourceRecord* pRecord, ResourceLoader* pResourceLoader )
            : m_requesterID( requesterID )
            , m_pResourceRecord( pRecord )
            , m_pResourceLoader( pResourceLoader )
        {
            KRG_ASSERT( Threading::IsMainThread() );
            KRG_ASSERT( m_pResourceRecord != nullptr && m_pResourceRecord->IsValid() );
            KRG_ASSERT( m_pResourceRecord->IsLoaded() || m_pResourceRecord->IsUnloaded() );
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

        void ResourceRequest::OnRawResourceRequestComplete( String const& filePath )
        {
            // Raw resource failed to load
            if ( filePath.empty() )
            {
                m_stage = ResourceRequest::Stage::Complete;
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
            }
            else // Continue the load operation
            {
                m_rawResourcePath = filePath;
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

                case Stage::UnloadResource:
                {
                    m_stage = Stage::InstallResource;
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
                    m_stage = Stage::Complete;
                    m_pResourceRecord->SetLoadingStatus( LoadingStatus::Unloaded );
                }
                break;

                case Stage::WaitForLoadDependencies:
                {
                    m_stage = Stage::CancelWaitForLoadDependencies;
                }

                case Stage::InstallResource:
                {
                    m_stage = Stage::UnloadResource;
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

        //-------------------------------------------------------------------------

        bool ResourceRequest::Update( RequestContext& requestContext )
        {
            // Update loading
            //-------------------------------------------------------------------------

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
                    KRG_PROFILE_SCOPE_RESOURCE( "Wait For Raw Resource Request" );
                }
                break;

                case ResourceRequest::Stage::LoadResource:
                {
                    LoadResource( requestContext );
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

                //-------------------------------------------------------------------------

                case ResourceRequest::Stage::UninstallResource:
                {
                    // Execute all unload operations immediately
                    UninstallResource( requestContext );
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
                    m_stage = Stage::UnloadResource;
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

            //-------------------------------------------------------------------------

            return IsComplete();
        }

        //-------------------------------------------------------------------------

        void ResourceRequest::RequestRawResource( RequestContext& requestContext )
        {
            KRG_PROFILE_FUNCTION_RESOURCE();
            requestContext.m_createRawRequestRequestFunction( this );
            m_stage = ResourceRequest::Stage::WaitForRawResourceRequest;
        }

        void ResourceRequest::LoadResource( RequestContext& requestContext )
        {
            KRG_PROFILE_FUNCTION_RESOURCE();
            KRG_ASSERT( m_stage == ResourceRequest::Stage::LoadResource );
            KRG_ASSERT( m_rawResourcePath.IsValid() );

            // Read file
            //-------------------------------------------------------------------------

            {
                KRG_PROFILE_SCOPE_IO( "Read File" );
                KRG_PROFILE_TAG( "filename", m_rawResourcePath.GetFileName().c_str() );

                if ( !FileSystem::LoadFile( m_rawResourcePath, m_rawResourceData ) )
                {
                    KRG_LOG_ERROR( "Resource", "Failed to load resource file (%s)", m_pResourceRecord->GetResourceID().c_str() );
                    m_stage = ResourceRequest::Stage::Complete;
                    m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
                }
            }

            // Load resource
            //-------------------------------------------------------------------------

            {
                KRG_PROFILE_SCOPE_RESOURCE( "Load Resource" );

                #if KRG_DEVELOPMENT_TOOLS
                char resTypeID[5];
                m_pResourceRecord->GetResourceTypeID().GetString( resTypeID );
                KRG_PROFILE_TAG( "Loader", resTypeID );
                #endif

                // Load the resource
                if ( !m_pResourceLoader->Load( GetResourceID(), m_rawResourceData, m_pResourceRecord ) )
                {
                    KRG_LOG_ERROR( "Resource", "Failed to load compiled resource data (%s)", m_pResourceRecord->GetResourceID().c_str() );
                    m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
                    m_pResourceLoader->Unload( GetResourceID(), m_pResourceRecord );
                    m_stage = ResourceRequest::Stage::Complete;
                    return;
                }

                // Release raw data
                m_rawResourceData.clear();
            }

            // Load dependencies
            //-------------------------------------------------------------------------

            // Create the resource ptrs for the install dependencies and request their load
            // These resource ptrs are temporary and will be clear upon completion of the request
            ResourceRequesterID const installDependencyRequesterID( m_pResourceRecord->GetResourceID() );
            U32 const numInstallDependencies = (U32) m_pResourceRecord->m_installDependencyResourceIDs.size();
            m_installDependencies.resize( numInstallDependencies );
            for ( U32 i = 0; i < numInstallDependencies; i++ )
            {
                // Do not use the requester ID for install dependencies! Since they are not explicitly loaded by a specific user!
                // Instead we create a ResourceRequesterID from the depending resource's resourceID
                m_installDependencies[i] = ResourcePtr( m_pResourceRecord->m_installDependencyResourceIDs[i] );
                requestContext.m_loadResourceFunction( installDependencyRequesterID, m_installDependencies[i] );
            }
            m_stage = ResourceRequest::Stage::WaitForLoadDependencies;
        }

        void ResourceRequest::WaitForLoadDependencies( RequestContext& requestContext )
        {
            KRG_PROFILE_FUNCTION_RESOURCE();
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
                if ( installDependency.HasLoadingFailed() )
                {
                    KRG_LOG_ERROR( "Resource", "Failed to load install dependency: %s", installDependency.GetResourceID().ToString().c_str() );
                    status = InstallStatus::ShouldFail;
                    break;
                }

                if ( !installDependency.IsLoaded() )
                {
                    status = InstallStatus::Loading;
                    break;
                }
            }

            // If dependency has failed, the resource has failed to load so immediately unload and set status to failed
            if ( status == InstallStatus::ShouldFail )
            {
                // Unload the resource data
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Unloading );
                m_installDependencies.clear();
                m_stage = Stage::UnloadResource;
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
            KRG_PROFILE_FUNCTION_RESOURCE();
            KRG_ASSERT( m_stage == ResourceRequest::Stage::InstallResource );

            if ( m_pResourceLoader->Install( GetResourceID(), m_pResourceRecord, m_installDependencies ) )
            {
                KRG_ASSERT( m_pResourceRecord->GetResourceData() != nullptr );
                m_installDependencies.clear();
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Loaded );
            }
            else // Install operation failed, unload resource and set status to failed
            {
                m_stage = ResourceRequest::Stage::UnloadResource;
                UnloadResource( requestContext );
                m_pResourceRecord->SetLoadingStatus( LoadingStatus::Failed );
            }

            m_stage = ResourceRequest::Stage::Complete;
        }

        //-------------------------------------------------------------------------

        void ResourceRequest::UninstallResource( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::UninstallResource );

            if ( m_pResourceRecord->IsLoaded() )
            {
                m_pResourceLoader->Uninstall( GetResourceID(), m_pResourceRecord );
            }

            m_stage = ResourceRequest::Stage::UnloadResource;
        }

        void ResourceRequest::UnloadResource( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::UnloadResource );

            // Unload dependencies
            //-------------------------------------------------------------------------

            // Create the resource ptrs for the install dependencies and request the unload
            // These resource ptrs are temporary and will be cleared upon completion of the request
            ResourceRequesterID const installDependencyRequesterID( m_pResourceRecord->GetResourceID() );
            U32 const numInstallDependencies = (U32) m_pResourceRecord->m_installDependencyResourceIDs.size();
            m_installDependencies.resize( numInstallDependencies );
            for ( U32 i = 0; i < numInstallDependencies; i++ )
            {
                // Do not use the user ID for install dependencies! Since they are not explicitly loaded by a specific user!
                // Instead we create a ResourceRequesterID from the depending resource's resourceID
                m_installDependencies[i] = ResourcePtr( m_pResourceRecord->m_installDependencyResourceIDs[i] );
                requestContext.m_unloadResourceFunction( installDependencyRequesterID, m_installDependencies[i] );
            }

            // Unload resource
            //-------------------------------------------------------------------------

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

        //-------------------------------------------------------------------------

        void ResourceRequest::CancelRawRequestRequest( RequestContext& requestContext )
        {
            KRG_ASSERT( m_stage == ResourceRequest::Stage::CancelRawResourceRequest );
            requestContext.m_cancelRawRequestRequestFunction( this );
            m_stage = ResourceRequest::Stage::Complete;
        }
    }
}