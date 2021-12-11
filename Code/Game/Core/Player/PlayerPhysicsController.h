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

        CharacterPhysicsController( Physics::CharacterComponent* pCharacterComponent )
            : m_pCharacterComponent( pCharacterComponent )
        {
            KRG_ASSERT( m_pCharacterComponent != nullptr );
        }

        // Controller Settings
        //-------------------------------------------------------------------------

        inline void EnableGravity() { m_isGravityEnabled = true; }
        inline void DisableGravity() { m_isGravityEnabled = false; }

        bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation );

    private:

        MoveResult SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Vector const& startPosition, Vector const& deltaTranslation, int32& Idx );

    public:

        Physics::CharacterComponent*        m_pCharacterComponent = nullptr;
        float                               m_gravitationalAcceleration = 30.f;
        Radians                             m_wallSlideAngle = Radians( Degrees( 60 ) );
        Radians                             m_maxNavigableSlopeAngle = Radians( Degrees( 45 ) );
        bool                                m_isGravityEnabled = true;
    };
}