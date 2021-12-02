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
        virtual Transform TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CapsuleComponent const* pCapsuleComponent, float const deltaTime, Quaternion const& deltaRotation, Vector const& deltaTranslation ) override;

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;

        Vector SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CapsuleComponent const* pCapsuleComponent, Quaternion const& rotation, Vector const& startPos, Vector const& endPos, int32 Idx );

    public:

        Vector m_deltaMovementHack = Vector::Zero;
        Quaternion m_deltaRotationHack = Quaternion::Identity;
    };
}