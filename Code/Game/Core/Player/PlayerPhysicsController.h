#pragma once
#include "System/Core/Math/Quaternion.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG::Physics 
{
    class CharacterComponent;
    class PhysicsWorldSystem;
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class CharacterPhysicsController final
    {
        struct MoveResult
        {
            MoveResult( Vector const& position )
                : m_initialPosition( position )
                , m_correctedPosition( position )
                , m_finalPosition( position )
                , m_remainingDistance( 0.0f )
            {}

            bool NeededDepenetration() const { return !m_initialPosition.IsEqual3( m_correctedPosition ); }

            void ApplyCorrectiveMove( MoveResult const& CorrectiveMove )
            {
                m_correctedPosition = CorrectiveMove.m_correctedPosition;
                m_finalPosition = CorrectiveMove.m_finalPosition;
                m_remainingDistance = CorrectiveMove.m_remainingDistance;
            }

            void ApplySubsequentMove( MoveResult const& SubsequentMove )
            {
                m_finalPosition = SubsequentMove.m_finalPosition;
                m_remainingDistance = SubsequentMove.m_remainingDistance;
            }

            Vector  m_initialPosition = Vector::Zero;
            Vector  m_correctedPosition = Vector::Zero;
            Vector  m_finalPosition = Vector::Zero;
            float   m_remainingDistance = 0.0f;
        };

    public:

        void EnableGravity() { m_isGravityEnable = true; }
        void DisableGravity() { m_isGravityEnable = false; }

        bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation );

    private:

        MoveResult SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent const* pCapsuleComponent, Vector const& startPosition, Vector const& deltaTranslation, int32& Idx );

    public:

        bool        m_isGravityEnable = true;
        float       m_gravityAcceleration = 30.f;
        Radians     m_wallSlideAngle = Radians( Degrees( 60 ) );
        Radians     m_maxNavigableSlopeAngle = Radians( Degrees( 45 ) );
    };
}