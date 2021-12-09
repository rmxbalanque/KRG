#include "EntitySystem_AIController.h"
#include "Game/Core/AI/AIPhysicsController.h"
#include "Game/Core/AI/AIAnimationController.h"
#include "Game/Core/AI/Components/Component_AI.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Core/Types/ScopedValue.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    void AIController::Activate()
    {
        m_behaviorContext.m_pCharacterPhysicsController = KRG::New<CharacterPhysicsController>();

        if ( m_pAnimGraphComponent != nullptr && m_pCharacterMeshComponent != nullptr )
        {
            m_behaviorContext.m_pAnimationController = KRG::New<AIAnimationController>( m_pAnimGraphComponent, m_pCharacterMeshComponent );
        }
    }

    void AIController::Deactivate()
    {
        KRG::Delete( m_behaviorContext.m_pAnimationController );
        KRG::Delete( m_behaviorContext.m_pCharacterPhysicsController );
    }

    //-------------------------------------------------------------------------

    void AIController::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pCharacterComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            KRG_ASSERT( m_behaviorContext.m_pCharacterPhysicsComponent == nullptr );
            m_behaviorContext.m_pCharacterPhysicsComponent = pCharacterComponent;
        }

        else if ( auto pAIComponent = TryCast<AIComponent>( pComponent ) )
        {
            KRG_ASSERT( m_behaviorContext.m_pAIComponent == nullptr );
            m_behaviorContext.m_pAIComponent = pAIComponent;
        }

        else if ( auto pCharacterMeshComponent = TryCast<Animation::CharacterMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pCharacterMeshComponent == nullptr );
            m_pCharacterMeshComponent = pCharacterMeshComponent;
        }

        else if ( auto pGraphComponent = TryCast<Animation::AnimationGraphComponent>( pComponent ) )
        {
            // We only support one component atm - animgraph comps are not singletons
            KRG_ASSERT( m_pAnimGraphComponent == nullptr );
            m_pAnimGraphComponent = pGraphComponent;
        }

        //-------------------------------------------------------------------------

        m_behaviorContext.m_components.emplace_back( pComponent );
    }

    void AIController::UnregisterComponent( EntityComponent* pComponent )
    {
        m_behaviorContext.m_components.erase_first( pComponent );

        //-------------------------------------------------------------------------

        if ( auto pCharacterComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            KRG_ASSERT( m_behaviorContext.m_pCharacterPhysicsComponent == pCharacterComponent );
            m_behaviorContext.m_pCharacterPhysicsComponent = nullptr;
        }

        else if ( auto pAIComponent = TryCast<AIComponent>( pComponent ) )
        {
            KRG_ASSERT( m_behaviorContext.m_pAIComponent == pAIComponent );
            m_behaviorContext.m_pAIComponent = nullptr;
        }

        else if ( auto pCharacterMeshComponent = TryCast<Animation::CharacterMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pCharacterMeshComponent != nullptr );
            m_pCharacterMeshComponent = nullptr;
        }

        else if ( auto pGraphComponent = TryCast<Animation::AnimationGraphComponent>( pComponent ) )
        {
            // We only support one component atm - animgraph comps are not singletons
            KRG_ASSERT( m_pAnimGraphComponent != nullptr );
            m_pAnimGraphComponent = nullptr;
        }
    }

    //-------------------------------------------------------------------------

    void AIController::Update( EntityUpdateContext const& ctx )
    {
        TScopedGuardValue const contextGuardValue( m_behaviorContext.m_pEntityUpdateContext, &ctx );
        TScopedGuardValue const navmeshSystemGuardValue( m_behaviorContext.m_pNavmeshSystem, ctx.GetWorldSystem<Navmesh::NavmeshWorldSystem>() );
        TScopedGuardValue const physicsSystemGuard( m_behaviorContext.m_pPhysicsWorld, ctx.GetWorldSystem<Physics::PhysicsWorldSystem>() );

        if ( !m_behaviorContext.IsValid() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        UpdateStage const updateStage = ctx.GetUpdateStage();
        if ( updateStage == UpdateStage::PrePhysics )
        {
            m_behaviorSelector.Update();

            // Update animation and get root motion delta
            m_pAnimGraphComponent->PrePhysicsUpdate( ctx.GetDeltaTime(), m_pCharacterMeshComponent->GetWorldTransform() );
            //Vector const& deltaTranslation = m_actionContext.m_pCharacterPhysicsComponent->m_deltaTranslationHACK; // m_actionContext.m_pGraphComponent->GetRootMotionDelta().GetTranslation();
            //Quaternion const& deltaRotation = m_actionContext.m_pCharacterPhysicsComponent->m_deltaRotationHACK; // m_actionContext.m_pGraphComponent->GetRootMotionDelta().GetRotation();

            // Move character
            //m_actionContext.m_pPhysicsController->GetActivePhysicsState()->TryMoveCapsule( m_actionContext.m_pPhysicsWorld, m_actionContext.m_pCharacterPhysicsComponent, ctx.GetDeltaTime(), deltaTranslation, deltaRotation );
        }
        else if ( updateStage == UpdateStage::PostPhysics )
        {
            m_pAnimGraphComponent->PostPhysicsUpdate( ctx.GetDeltaTime(), m_pCharacterMeshComponent->GetWorldTransform() );
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }
    }
}