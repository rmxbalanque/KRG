#pragma once
#include "Game/Core/GameplayPhysics/GameplayPhysicsState.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerLocomotionPhysicsState final : public GameplayPhysics::PhysicsState
    {
    public:

        KRG_GAMEPLAY_PHYSICS_STATE_ID( PlayerLocomotionPhysicsState );

    public:

        void PredictMovement();
        virtual Transform TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CapsuleComponent const* pCapsuleComponent, Quaternion const& deltaRotation, Vector const& deltaTranslation ) override;

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;

    private:

        float   m_verticalVelocity = 0.0f;
    };
}