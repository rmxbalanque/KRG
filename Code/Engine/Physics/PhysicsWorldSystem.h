#pragma once

#include "_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxShape;
    class PxRigidActor;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsWorldSystem : public IWorldEntitySystem
    {
        friend class PhysicsDebugViewController;

        struct EntityPhysicsRecord : public EntityRegistryRecord
        {
            inline bool IsEmpty() const { return m_components.empty(); }

            TVector<PhysicsComponent*>                  m_components;
        };

    public:

        KRG_ENTITY_WORLD_SYSTEM( PhysicsWorldSystem, RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        PhysicsWorldSystem( PhysicsSystem& physicsSystem );

    private:

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateEntitySystem( UpdateContext const& ctx ) override final;

        physx::PxRigidActor* CreateActor( PhysicsComponent* pComponent ) const;
        physx::PxShape* CreateShape( PhysicsComponent* pComponent, physx::PxRigidActor* pActor ) const;

    private:

        PhysicsSystem&                                  m_physicsSystem;
        EntityRegistry<EntityPhysicsRecord>             m_registeredEntities;
        THashMap<UUID, PhysicsComponent*>               m_dynamicComponents; // TODO: profile and see if we need to use a dynamic pool

        #if KRG_DEVELOPMENT_TOOLS
        bool                                            m_drawDynamicActorBounds = false;
        bool                                            m_drawKinematicActorBounds = false;
        #endif
    };
}