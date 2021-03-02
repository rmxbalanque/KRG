#pragma once

#include "_Module/API.h"
#include "PhysicsScene.h"
#include "System/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxShape;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsShapeComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsWorldSystem : public IWorldEntitySystem
    {
        struct EntityPhysicsRecord : public EntityRegistryRecord
        {
            inline bool IsEmpty() const { return m_shapeComponents.empty(); }

            TVector<PhysicsShapeComponent*>         m_shapeComponents;
        };

    public:

        KRG_ENTITY_WORLD_SYSTEM( PhysicsWorldSystem );

    public:

        PhysicsWorldSystem( PhysicsSystem& physicsSystem );
        ~PhysicsWorldSystem();

        inline PhysicsScene* GetScene() const { return m_pScene; }

    private:

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void RegisterShapeComponent( Entity const* pEntity, PhysicsShapeComponent* pComponent );
        void UnregisterShapeComponent( Entity const* pEntity, PhysicsShapeComponent* pComponent );

        physx::PxShape* CreateShape( PhysicsShapeComponent* pComponent );

    private:

        PhysicsSystem&                                  m_physicsSystem;
        PhysicsScene*                                   m_pScene = nullptr;
        EntityRegistry<EntityPhysicsRecord>             m_registeredEntities;
    };
}