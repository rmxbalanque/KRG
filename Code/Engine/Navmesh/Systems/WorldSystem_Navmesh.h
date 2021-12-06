#pragma once

#include "Engine/Navmesh/_Module/API.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "bfxMover.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavmeshComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_NAVMESH_API NavmeshWorldSystem : public IWorldEntitySystem
    {
        friend class NavmeshDebugView;
        friend class NavmeshDebugRenderer;

        //-------------------------------------------------------------------------

        struct RegisteredNavmesh
        {
            RegisteredNavmesh( ComponentID const& ID, char* pNavmesh ) : m_componentID( ID ), m_pNavmesh( pNavmesh ) { KRG_ASSERT( ID.IsValid() && pNavmesh != nullptr ); }

            ComponentID     m_componentID;
            char*           m_pNavmesh;
        };

    public:

        KRG_REGISTER_TYPE( NavmeshWorldSystem );
        KRG_ENTITY_WORLD_SYSTEM( NavmeshWorldSystem, RequiresUpdate( UpdateStage::Physics ) );

    public:

        NavmeshWorldSystem() = default;

        KRG_FORCE_INLINE bfx::SpaceHandle GetSpaceHandle() const { return bfx::GetDefaultSpaceHandle( m_pInstance ); }

        // HACK
        bfx::Mover* CreateMover();
        void DestroyMover( bfx::Mover* );
        void SetMoverGoal( bfx::Mover* pMover, Vector const& pos );

    private:

        virtual void InitializeSystem( SystemRegistry const& systemRegistry ) override;
        virtual void ShutdownSystem() override;

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void RegisterNavmesh( NavmeshComponent* pComponent );
        void UnregisterNavmesh( NavmeshComponent* pComponent );

        void UpdateSystem( EntityUpdateContext const& ctx ) override;

    private:

        bfx::Instance*                                  m_pInstance = nullptr;
        TVector<NavmeshComponent*>                      m_navmeshComponents;
        TVector<RegisteredNavmesh>                      m_registeredNavmeshes;

        #if KRG_DEVELOPMENT_TOOLS
        NavpowerRenderer                                m_renderer;
        #endif
    };
}