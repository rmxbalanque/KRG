#pragma once

#include "Engine/Navmesh/_Module/API.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "Engine/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------
// Navmesh World System
//-------------------------------------------------------------------------
// This is the main system responsible for managing navmesh within a specific world
// Manages navmesh registration, obstacles creation/destruction, etc...
// Primarily also needed to get the space handle needed for any queries ( GetSpaceHandle )

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

        AABB GetNavmeshBounds( uint32 layerIdx ) const;

        #if KRG_ENABLE_NAVPOWER
        KRG_FORCE_INLINE bfx::SpaceHandle GetSpaceHandle() const { return bfx::GetDefaultSpaceHandle( m_pInstance ); }
        #endif

    private:

        virtual void InitializeSystem( SystemRegistry const& systemRegistry ) override;
        virtual void ShutdownSystem() override;

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void RegisterNavmesh( NavmeshComponent* pComponent );
        void UnregisterNavmesh( NavmeshComponent* pComponent );

        void UpdateSystem( EntityWorldUpdateContext const& ctx ) override;

    private:

        #if KRG_ENABLE_NAVPOWER
        bfx::Instance*                                  m_pInstance = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        NavpowerRenderer                                m_renderer;
        #endif

        #endif

        //-------------------------------------------------------------------------

        TVector<NavmeshComponent*>                      m_navmeshComponents;
        TVector<RegisteredNavmesh>                      m_registeredNavmeshes;
    };
}