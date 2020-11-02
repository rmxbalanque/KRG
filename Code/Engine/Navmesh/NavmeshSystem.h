#pragma once

#include "_Module/API.h"
#include "Engine/Navmesh/NavPower.h"
#include "System/Entity/EntityGlobalSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavmeshComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_NAVMESH_API NavmeshSystem : public ISystem, public IGlobalEntitySystem
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

        KRG_SYSTEM_ID( NavmeshSystem );
        KRG_ENTITY_GLOBAL_SYSTEM( NavmeshSystem );

    public:

        NavmeshSystem() = default;

        void Initialize();
        void Shutdown();

    private:

        virtual void InitializeEntitySystem( SystemRegistry const& systemRegistry ) override;
        virtual void ShutdownEntitySystem() override;
        virtual void UpdateEntitySystem( UpdateContext const& ctx ) override;

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void RegisterNavmesh( NavmeshComponent* pComponent );
        void UnregisterNavmesh( NavmeshComponent* pComponent );

    private:

        EntityRegistry<RegisteredNavmeshComponent>      m_navmeshComponents;
        TVector<RegisteredNavmesh>                      m_registeredNavmeshes;
        NavPowerAllocator                               m_allocator;

        #if KRG_DEBUG_INSTRUMENTATION
        NavPowerRenderer                                m_debugRenderer;
        #endif
    };
}