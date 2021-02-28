#pragma once

#include "_Module/API.h"
#include "PhysicsScene.h"
#include "System/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsGeometryComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsWorldSystem : public IWorldEntitySystem
    {
        struct RegisteredGeometryComponent : public EntityRegistryRecord
        {
            PhysicsGeometryComponent*                   m_pComponent = nullptr;
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

    private:

        PhysicsSystem&                                  m_physicsSystem;
        PhysicsScene*                                   m_pScene = nullptr;
        EntityRegistry<RegisteredGeometryComponent>     m_geometryComponents;
    };
}