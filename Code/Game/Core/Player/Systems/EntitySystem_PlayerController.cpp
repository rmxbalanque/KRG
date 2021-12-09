#include "EntitySystem_PlayerController.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerAnimationController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Animation/Graph/AnimationGraphController.h"
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
        m_actionContext.m_pCharacterPhysicsController = KRG::New<CharacterPhysicsController>();

        if ( m_pAnimGraphComponent != nullptr && m_pCharacterMeshComponent != nullptr )
        {
            m_actionContext.m_pAnimationController = KRG::New<PlayerAnimationController>( m_pAnimGraphComponent, m_pCharacterMeshComponent );
        }

        if ( m_pCameraComponent != nullptr )
        {
            m_actionContext.m_pCameraController = KRG::New<CameraController>( m_pCameraComponent );
        }
    }

    void PlayerController::Deactivate()
    {
        KRG::Delete( m_actionContext.m_pCameraController );
        KRG::Delete( m_actionContext.m_pAnimationController );
        KRG::Delete( m_actionContext.m_pCharacterPhysicsController );
    }

    //-------------------------------------------------------------------------

    void PlayerController::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pCharacterComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pCharacterPhysicsComponent == nullptr );
            m_actionContext.m_pCharacterPhysicsComponent = pCharacterComponent;
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

        m_actionContext.m_components.emplace_back( pComponent );
    }

    void PlayerController::UnregisterComponent( EntityComponent* pComponent )
    {
        m_actionContext.m_components.erase_first( pComponent );

        //-------------------------------------------------------------------------

        if ( auto pCharacterComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pCharacterPhysicsComponent == pCharacterComponent );
            m_actionContext.m_pCharacterPhysicsComponent = nullptr;
        }

        else if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pCameraComponent == pCameraComponent );
            m_pCameraComponent = nullptr;
        }

        else if ( auto pPlayerComponent = TryCast<MainPlayerComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pPlayerComponent == pPlayerComponent );
            m_actionContext.m_pPlayerComponent = nullptr;
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

    void PlayerController::Update( EntityUpdateContext const& ctx )
    {
        TScopedGuardValue const contextGuardValue( m_actionContext.m_pEntityUpdateContext, &ctx );
        TScopedGuardValue const inputSystemGuardValue( m_actionContext.m_pInputSystem, ctx.GetSystem<Input::InputSystem>() );
        TScopedGuardValue const physicsSystemGuard( m_actionContext.m_pPhysicsWorld, ctx.GetWorldSystem<Physics::PhysicsWorldSystem>() );

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

            // Update animation and get root motion delta (remember that root motion is in character space, so we need to convert the displacement to world space)
            m_pAnimGraphComponent->PrePhysicsUpdate( ctx.GetDeltaTime(), m_pCharacterMeshComponent->GetWorldTransform() );
            Vector const& deltaTranslation = m_pCharacterMeshComponent->GetWorldTransform().RotateVector( m_pAnimGraphComponent->GetRootMotionDelta().GetTranslation() );
            Quaternion const& deltaRotation = m_pAnimGraphComponent->GetRootMotionDelta().GetRotation();

            // Move character
            m_actionContext.m_pCharacterPhysicsController->TryMoveCapsule( m_actionContext.m_pPhysicsWorld, m_actionContext.m_pCharacterPhysicsComponent, ctx.GetDeltaTime(), deltaTranslation, deltaRotation );

            // Update camera position relative to new character position
            m_actionContext.m_pCameraController->FinalizeCamera();
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