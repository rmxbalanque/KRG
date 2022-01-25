#include "EntitySystem_PlayerController.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerAnimationController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Controller.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Components/Component_Player.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Types/ScopedValue.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    void PlayerController::Activate()
    {
        if ( m_actionContext.m_pCharacterComponent != nullptr )
        {
            m_actionContext.m_pCharacterController = KRG::New<CharacterPhysicsController>( m_actionContext.m_pCharacterComponent );
        }

        if ( m_pAnimGraphComponent != nullptr && m_pCharacterMeshComponent != nullptr )
        {
            m_actionContext.m_pAnimationController = KRG::New<AnimationController>( m_pAnimGraphComponent, m_pCharacterMeshComponent );
        }

        if ( m_pCameraComponent != nullptr )
        {
            m_actionContext.m_pCameraController = KRG::New<CameraController>( m_pCameraComponent );
        }
    }

    void PlayerController::Shutdown()
    {
        KRG_ASSERT( m_actionContext.m_pCameraController == nullptr );
        KRG_ASSERT( m_actionContext.m_pAnimationController == nullptr );
        KRG_ASSERT( m_actionContext.m_pCharacterController == nullptr );
    }

    //-------------------------------------------------------------------------

    void PlayerController::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pCharacterPhysicsComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pCharacterComponent == nullptr );
            m_actionContext.m_pCharacterComponent = pCharacterPhysicsComponent;
        }

        else if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pCameraComponent == nullptr );
            m_pCameraComponent = pCameraComponent;
        }

        else if ( auto pPlayerComponent = TryCast<MainPlayerComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pPlayerComponent == nullptr );
            m_actionContext.m_pPlayerComponent = pPlayerComponent;
        }

        else if ( auto pCharacterMeshComponent = TryCast<Render::CharacterMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pCharacterMeshComponent == nullptr );
            m_pCharacterMeshComponent = pCharacterMeshComponent;
        }

        else if ( auto pGraphComponent = TryCast<Animation::AnimationGraphComponent>( pComponent ) )
        {
            // We only support one component ATM - animation graph comps are not singletons
            KRG_ASSERT( m_pAnimGraphComponent == nullptr );
            m_pAnimGraphComponent = pGraphComponent;
        }

        //-------------------------------------------------------------------------

        m_actionContext.m_components.emplace_back( pComponent );
    }

    void PlayerController::UnregisterComponent( EntityComponent* pComponent )
    {
        m_actionContext.m_components.erase_first( pComponent );

        //-------------------------------------------------------------------------

        if ( auto pCharacterPhysicsComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pCharacterComponent == pCharacterPhysicsComponent );
            m_actionContext.m_pCharacterComponent = nullptr;

            KRG::Delete( m_actionContext.m_pCharacterController );
        }

        else if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pCameraComponent == pCameraComponent );
            m_pCameraComponent = nullptr;

            KRG::Delete( m_actionContext.m_pCameraController );
        }

        else if ( auto pPlayerComponent = TryCast<MainPlayerComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pPlayerComponent == pPlayerComponent );
            m_actionContext.m_pPlayerComponent = nullptr;
        }

        else if ( auto pCharacterMeshComponent = TryCast<Render::CharacterMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pCharacterMeshComponent != nullptr );
            m_pCharacterMeshComponent = nullptr;

            KRG::Delete( m_actionContext.m_pAnimationController );
        }

        else if ( auto pGraphComponent = TryCast<Animation::AnimationGraphComponent>( pComponent ) )
        {
            // We only support one component atm - animgraph comps are not singletons
            KRG_ASSERT( m_pAnimGraphComponent != nullptr );
            m_pAnimGraphComponent = nullptr;

            KRG::Delete( m_actionContext.m_pAnimationController );
        }
    }

    //-------------------------------------------------------------------------

    void PlayerController::Update( EntityWorldUpdateContext const& ctx )
    {
        if ( !ctx.IsGameWorld() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        TScopedGuardValue const contextGuardValue( m_actionContext.m_pEntityWorldUpdateContext, &ctx );
        TScopedGuardValue const physicsSystemGuard( m_actionContext.m_pPhysicsScene, ctx.GetWorldSystem<Physics::PhysicsWorldSystem>()->GetScene() );
        TScopedGuardValue const inputStateGuardValue( m_actionContext.m_pInputState, ctx.GetInputState() );

        if ( !m_actionContext.IsValid() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        UpdateStage const updateStage = ctx.GetUpdateStage();
        if ( updateStage == UpdateStage::PrePhysics )
        {
            // Update camera
            m_actionContext.m_pCameraController->UpdateCamera( ctx );

            // Update player actions
            m_actionStateMachine.Update();

            // Update player component state
            m_actionContext.m_pPlayerComponent->UpdateState( m_actionContext.GetDeltaTime() );

            // Update animation and get root motion delta (remember that root motion is in character space, so we need to convert the displacement to world space)
            m_pAnimGraphComponent->EvaluateGraph( ctx.GetDeltaTime(), m_pCharacterMeshComponent->GetWorldTransform(), m_actionContext.m_pPhysicsScene );
            Vector const& deltaTranslation = m_pCharacterMeshComponent->GetWorldTransform().RotateVector( m_pAnimGraphComponent->GetRootMotionDelta().GetTranslation() );
            Quaternion const& deltaRotation = m_pAnimGraphComponent->GetRootMotionDelta().GetRotation();

            // Move character
            m_actionContext.m_pCharacterController->TryMoveCapsule( ctx, m_actionContext.m_pPhysicsScene, deltaTranslation, deltaRotation );

            // Run animation pose tasks
            m_pAnimGraphComponent->ExecutePrePhysicsTasks( m_pCharacterMeshComponent->GetWorldTransform() );

            // Update camera position relative to new character position
            m_actionContext.m_pCameraController->FinalizeCamera();
        }
        else if ( updateStage == UpdateStage::PostPhysics )
        {
            m_pAnimGraphComponent->ExecutePostPhysicsTasks();
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }
    }
}