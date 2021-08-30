#include "CameraWorldSystem.h"
#include "Components/CameraComponent.h"
#include "System/Entity/Entity.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG::Camera
{
    CameraWorldSystem::RegisteredCamera::RegisteredCamera( UUID entityID, CameraComponent* pComponent )
        : m_entityID( entityID )
        , m_pComponent( pComponent )
    {
        KRG_ASSERT( entityID.IsValid() && pComponent != nullptr );
    }

    //-------------------------------------------------------------------------

    void CameraWorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        auto pCameraComponent = ComponentCast<CameraComponent>( pComponent );
        if ( pCameraComponent != nullptr )
        {
            // Add to registered components list
            m_cameras.emplace_back( RegisteredCamera( pEntity->GetID(), pCameraComponent ) );
        }
    }

    void CameraWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        for ( auto i = 0; i < m_cameras.size(); i++ )
        {
            // Skip any components not on this entity
            if ( m_cameras[i].m_entityID != pEntity->GetID() )
            {
                continue;
            }

            // Remove component from registered component list
            m_cameras.erase_unsorted( m_cameras.begin() + i );
            i--;
        }
    }

    //-------------------------------------------------------------------------

    bool CameraWorldSystem::HasActiveCameras() const
    {
        return m_cameras.size() > 0;
    }

    void CameraWorldSystem::UpdateEntitySystem( UpdateContext const& ctx )
    {
        if ( !HasActiveCameras() )
        {
            return;
        }

        auto pViewportManager = ctx.GetSystem<Render::ViewportManager>();
        KRG_ASSERT( pViewportManager != nullptr );

        //-------------------------------------------------------------------------

        // For now we only support one camera and one viewport
        Render::Viewport& primaryViewport = pViewportManager->GetActiveViewports()[0];
        Math::ViewVolume const& cameraViewVolume = m_cameras[0].m_pComponent->CalculateViewVolume( primaryViewport.GetSize() );
        primaryViewport.SetViewVolume( cameraViewVolume );
    }
}