#include "CameraWorldSystem.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG
{
    UpdatePriorityList const CameraWorldSystem::PriorityList = UpdatePriorityList();

    //-------------------------------------------------------------------------

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
            // Update camera view dimensions
            pCameraComponent->OnViewResized( m_viewDimensions );

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

    void CameraWorldSystem::SetViewDimensions( Int2 newWindowDimensions )
    {
        m_viewDimensions = newWindowDimensions;
        for ( auto& camera : m_cameras )
        {
            camera.m_pComponent->OnViewResized( m_viewDimensions );
        }
    }

    void CameraWorldSystem::Update()
    {
        UpdateActiveViewports();
    }

    void CameraWorldSystem::UpdateActiveViewports()
    {
        m_activeViewports.clear();

        //-------------------------------------------------------------------------

        for ( auto pCamera : m_cameras )
        {
            Math::Rectangle const viewportRect = pCamera.m_pComponent->GetViewportRect();

            float const TLX = viewportRect.GetTopLeft().x * m_viewDimensions.x;
            float const TLY = viewportRect.GetTopLeft().y * m_viewDimensions.y;
            Int2 const topLeft = Int2( (int32) TLX, (int32) TLY );

            float const sizeX = viewportRect.GetSize().x * m_viewDimensions.x;
            float const sizeY = viewportRect.GetSize().y * m_viewDimensions.y;
            Int2 const size = Int2( (int32) sizeX, (int32) sizeY );
            m_activeViewports.emplace_back( Math::Viewport( topLeft, size, pCamera.m_pComponent->GetViewVolume() ) );
        }
    }
}