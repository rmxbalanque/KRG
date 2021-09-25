#pragma once

#include "_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavmeshComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_NAVMESH_API NavmeshWorldSystem : public IWorldEntitySystem
    {
        struct RegisteredNavmeshComponent : public EntityRegistryRecord
        {
            NavmeshComponent*               m_pComponent = nullptr;
        };

        struct RegisteredNavmesh
        {
            RegisteredNavmesh( UUID ID, char* pNavmesh ) : m_componentID( ID ), m_pNavmesh( pNavmesh ) { KRG_ASSERT( ID.IsValid() && pNavmesh != nullptr ); }

            UUID    m_componentID;
            char*   m_pNavmesh;
        };

    public:

        KRG_ENTITY_WORLD_SYSTEM( NavmeshWorldSystem );

    public:

        NavmeshWorldSystem() = default;

    private:

        virtual void ShutdownEntitySystem() override;

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void RegisterNavmesh( NavmeshComponent* pComponent );
        void UnregisterNavmesh( NavmeshComponent* pComponent );

    private:

        EntityRegistry<RegisteredNavmeshComponent>      m_navmeshComponents;
        TVector<RegisteredNavmesh>                      m_registeredNavmeshes;
    };
}