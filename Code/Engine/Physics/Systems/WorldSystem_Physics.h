#pragma once

#include "Engine/Physics/_Module/API.h"
#include "Engine/Physics/PhysicsQuery.h"
#include "Engine/Physics/PhysX.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Types/IDVector.h"
#include "System/Core/Types/ScopedValue.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxPhysics;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsShapeComponent;
    class CharacterComponent;
    class Scene;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsWorldSystem final : public IWorldEntitySystem
    {
        friend class PhysicsDebugView;
        friend class PhysicsRenderer;

        struct EntityPhysicsRecord
        {
            inline bool IsEmpty() const { return m_components.empty(); }

            TVector<PhysicsShapeComponent*>                  m_components;
        };

    public:

        KRG_REGISTER_TYPE( PhysicsWorldSystem );
        KRG_ENTITY_WORLD_SYSTEM( PhysicsWorldSystem, RequiresUpdate( UpdateStage::Physics ), RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        PhysicsWorldSystem() = default;
        PhysicsWorldSystem( PhysicsSystem& physicsSystem );

        // Get the scene
        Scene const* GetScene() const { return m_pScene; }

        // Get the scene
        Scene* GetScene() { return m_pScene; }

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline uint32 GetDebugFlags() const { return m_sceneDebugFlags; }
        void SetDebugFlags( uint32 debugFlags );

        inline bool IsDebugDrawingEnabled() const;
        inline float GetDebugDrawDistance() const { return m_debugDrawDistance; }
        inline void SetDebugDrawDistance( float drawDistance ) { m_debugDrawDistance = Math::Max( drawDistance, 0.0f ); }

        void SetDebugCullingBox( AABB const& cullingBox );
        #endif

    private:

        // Get the physx scene
        physx::PxScene* GetPxScene();

        virtual void InitializeSystem( SystemRegistry const& systemRegistry ) override;
        virtual void ShutdownSystem() override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityWorldUpdateContext const& ctx ) override final;

        physx::PxRigidActor* CreateActor( PhysicsShapeComponent* pComponent ) const;
        physx::PxShape* CreateShape( PhysicsShapeComponent* pComponent, physx::PxRigidActor* pActor ) const;

        bool CreateCharacterActorAndShape( CharacterComponent* pComponent ) const;

    private:

        PhysicsSystem*                                          m_pPhysicsSystem = nullptr;
        Scene*                                                  m_pScene = nullptr;

        TIDVector<ComponentID, CharacterComponent*>             m_characterComponents;
        TIDVector<ComponentID, PhysicsShapeComponent*>          m_physicsShapeComponents;
        TIDVector<ComponentID, PhysicsShapeComponent*>          m_dynamicShapeComponents; // TODO: profile and see if we need to use a dynamic pool

        #if KRG_DEVELOPMENT_TOOLS
        bool                                                    m_drawDynamicActorBounds = false;
        bool                                                    m_drawKinematicActorBounds = false;
        uint32                                                  m_sceneDebugFlags = 0;
        float                                                   m_debugDrawDistance = 10.0f;
        #endif
    };
}