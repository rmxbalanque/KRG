#include "WorldSystem_Navmesh.h"
#include "Engine/Navmesh/Components/Component_Navmesh.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Core/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavmeshWorldSystem::ShutdownSystem()
    {
        KRG_ASSERT( m_registeredNavmeshes.empty() );
    }

    //-------------------------------------------------------------------------

    void NavmeshWorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pNavmeshComponent = TryCast<NavmeshComponent>( pComponent ) )
        {
            m_navmeshComponents.emplace_back( pNavmeshComponent );

            if ( pNavmeshComponent->m_pNavmeshData->HasNavmeshData() )
            {
                RegisterNavmesh( pNavmeshComponent );
            }
        }
    }

    void NavmeshWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pNavmeshComponent = TryCast<NavmeshComponent>( pComponent ) )
        {
            if ( pNavmeshComponent->m_pNavmeshData->HasNavmeshData() )
            {
                UnregisterNavmesh( pNavmeshComponent );
            }

            m_navmeshComponents.erase_first_unsorted( pNavmeshComponent );
        }
    }

    void NavmeshWorldSystem::RegisterNavmesh( NavmeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        // Copy resource
        //-------------------------------------------------------------------------
        // NavPower operates on the resource in place so we need to make a copy

        NavmeshData const* pData = pComponent->m_pNavmeshData.GetPtr();
        KRG_ASSERT( pData != nullptr && pData->HasNavmeshData() );

        size_t const requiredMemory = sizeof( char ) * pData->GetGraphImage().size();
        char* pNavmesh = (char*) KRG::Alloc( requiredMemory );
        memcpy( pNavmesh, pData->GetGraphImage().data(), requiredMemory );

        // Add resource
        //-------------------------------------------------------------------------

        Transform const& componentWorldTransform = pComponent->GetWorldTransform();

        bfx::ResourceOffset offset;
        offset.m_positionOffset = ToBfx( componentWorldTransform.GetTranslation() );
        offset.m_rotationOffset = ToBfx( componentWorldTransform.GetRotation() );
        bfx::AddResource( pNavmesh, offset );

        // Add record
        m_registeredNavmeshes.emplace_back( RegisteredNavmesh( pComponent->GetID(), pNavmesh ) );
    }

    void NavmeshWorldSystem::UnregisterNavmesh( NavmeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
        for ( auto i = 0u; i < m_registeredNavmeshes.size(); i++ )
        {
            if ( pComponent->GetID() == m_registeredNavmeshes[i].m_componentID )
            {
                bfx::RemoveResource( m_registeredNavmeshes[i].m_pNavmesh );

                //-------------------------------------------------------------------------

                KRG::Free( m_registeredNavmeshes[i].m_pNavmesh );
                m_registeredNavmeshes.erase_unsorted( m_registeredNavmeshes.begin() + i );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }
}