#include "CameraWorldSystem.h"
#include "Engine/Core/Camera/CameraComponent.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
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

    void CameraWorldSystem::UpdateSystem( EntityUpdateContext const& ctx )
    {
        // Temp: always set the first camera as the active one
        m_pActiveCamera = m_cameras.empty() ? nullptr : m_cameras[0].m_pComponent;
    }
}