#pragma once
#include "Engine/Physics/PhysicsStateController.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerLocomotionPhysicsState final : public Physics::PhysicsState
    {
    public:

        KRG_PHYSICS_STATE_ID( PlayerLocomotionPhysicsState );

    public:

        void PredictMovement();
        virtual bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, float const deltaTime, Transform const& deltaTransform ) override;

        // HACK
        void SetHackDesires( Vector const& headingVelocity, Quaternion const& facing )
        {
            m_headingHack = headingVelocity;
            m_facingHack = facing;
        }

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;

        Vector SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent const* pCharacterComponent, Quaternion const& rotation, Vector const& startPos, Vector const& endPos, int32& Idx );

    public:

        Vector m_headingHack = Vector::Zero;
        Quaternion m_facingHack = Quaternion::Identity;
    };
}