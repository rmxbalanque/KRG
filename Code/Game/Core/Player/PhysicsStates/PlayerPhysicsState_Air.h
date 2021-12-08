#pragma once
#include "Engine/Physics/PhysicsState.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class AirPhysicsState final : public Physics::PhysicsState
    {
    public:

        KRG_PHYSICS_STATE_ID( AirPhysicsState );

    public:

        inline void AddImpulse( Vector const& impulseVelocity ) { m_impulseVelocity = impulseVelocity; }
        virtual bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation ) override;

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;

        Vector SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent const* pCapsuleComponent, Vector const& startPosition, Vector const& deltaTranslation, int32& Idx );

    private:

        float       m_gravityAcceleration = 30.f;
        Radians     m_wallSlideAngle = Degrees( 60 );
        Vector      m_impulseVelocity = Vector::Zero;
    };
}