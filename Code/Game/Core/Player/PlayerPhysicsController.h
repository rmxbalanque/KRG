#pragma once
#include "System/Core/Time/Time.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/Containers.h"
#include "Engine/Core/Entity/EntityIDs.h"
#include "Engine/Physics/PhysicsLayers.h"
#include "Engine/Physics/PhysicsQuery.h"

//-------------------------------------------------------------------------

namespace KRG 
{
    class EntityWorldUpdateContext;
}

namespace KRG::Physics 
{
    class CharacterComponent;
    class Scene;
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class CharacterPhysicsController final
    {
        struct Settings
        {
            float                               m_gravitationalAcceleration = 30.f;
            float                               m_stepHeight = 0.4f;

            Radians                             m_wallSlideAngle = Radians( Degrees( 60 ) );
            Radians                             m_maxNavigableSlopeAngle = Radians( Degrees( 45 ) );

            uint32                              m_physicsLayerMask = Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters );
            TInlineVector<EntityID, 5>          m_ignoredActors;
        };

        struct MoveResult
        {
        public:

            MoveResult( Vector const& position )
                : m_initialPosition( position )
                , m_correctedPosition( position )
                , m_finalPosition( position )
                , m_remainingDistance( 0.0f )
            {}

            inline Vector const& GetInitialPosition() const { return m_initialPosition; }
            inline Vector const& GetCorrectedPosition() const { return m_correctedPosition; }
            inline Vector const& GetFinalPosition() const { return m_finalPosition; }
            inline float GetRemainingDistance() const { return m_remainingDistance; }
            inline Physics::SweepResults const& GetSweepResults() const { return m_sweepResults; }

            inline void FinalisePosition( Physics::SweepResults sweepResults, Vector Offset = Vector::Zero )
            {
                m_sweepResults = sweepResults;
                if( sweepResults.hasBlock )
                {
                    m_finalPosition = sweepResults.GetShapePosition() + Offset;
                    m_remainingDistance = sweepResults.GetRemainingDistance();
                }
                else
                {
                    m_finalPosition = sweepResults.m_sweepEnd + Offset;
                    m_remainingDistance = 0.0f;
                }
            }

            inline void ApplyCorrectiveMove( MoveResult const& CorrectiveMove )
            {
                m_correctedPosition     = CorrectiveMove.m_correctedPosition;
                m_finalPosition         = CorrectiveMove.m_finalPosition;
                m_remainingDistance     = CorrectiveMove.m_remainingDistance;
                m_sweepResults          = CorrectiveMove.m_sweepResults;
            }

            inline void ApplySubsequentMove( MoveResult const& SubsequentMove, Vector Offset = Vector::Zero )
            {
                m_finalPosition         = SubsequentMove.m_finalPosition + Offset;
                m_remainingDistance     = SubsequentMove.m_remainingDistance;
                m_sweepResults          = SubsequentMove.m_sweepResults;
            }

        private:

            Vector                  m_initialPosition = Vector::Zero;
            Vector                  m_correctedPosition = Vector::Zero;
            Vector                  m_finalPosition = Vector::Zero;
            float                   m_remainingDistance = 0.0f;
            Physics::SweepResults   m_sweepResults;
        };

    public:

        CharacterPhysicsController( Physics::CharacterComponent* pCharacterComponent )
            : m_pCharacterComponent( pCharacterComponent )
        {
            KRG_ASSERT( m_pCharacterComponent != nullptr );
        }

        // Controller Settings
        //-------------------------------------------------------------------------

        inline Settings& GetSettings() { return m_settings; }
        inline void SetSettings( Settings const& settings ) { m_settings = settings; }

        inline void EnableGravity( float currentVerticalSpeed ) { m_isGravityEnabled = true; m_verticalSpeed = currentVerticalSpeed; }
        inline void DisableGravity() { m_isGravityEnabled = false; m_verticalSpeed = 0.f; }
        inline void ResetGravityMomentum() { m_verticalSpeed = 0.f; }

        inline void EnableStepHeight() { m_isStepHeightEnabled = true; }
        inline void DisableStepHeight() { m_isStepHeightEnabled = false; }

        inline void EnableProjectionOntoFloor() { m_ProjectOntoFloor = true; }
        inline void DisableProjectionOntoFloor() { m_ProjectOntoFloor = false; }

        // Controller Move
        //-------------------------------------------------------------------------

        inline bool IsOnGround() const { return m_isOnGround; }

        bool TryMoveCapsule( EntityWorldUpdateContext const& ctx, Physics::Scene* pPhysicsScene, Vector const& deltaTranslation, Quaternion const& deltaRotation );

    private:

        MoveResult SweepCylinder( Physics::Scene* pPhysicsScene, float cylinderHalfHeight, float cylinderRadius, Vector const& startPosition, Vector const& deltaTranslation, int32& Idx );

        MoveResult SweepCylinderVertical( Physics::Scene* pPhysicsScene, float cylinderHalfHeight, float cylinderRadius, Vector const& startPosition, Vector const& deltaTranslation, Vector const& stepHeightOffset, int32& Idx );

        MoveResult AdjustCapsuleToGround( Physics::Scene* pPhysicsScene, float capsuleCylinderPortionHalfHeight, float capsuleRadius, Transform const& transform, float distance );

    private:

        Settings                            m_settings;

        Physics::CharacterComponent*        m_pCharacterComponent = nullptr;

        bool                                m_isStepHeightEnabled = true;
        bool                                m_isGravityEnabled = true;
        bool                                m_ProjectOntoFloor = true;

        bool                                m_isOnGround = false;
        float                               m_verticalSpeed = 0.0f;
        Vector                              m_floorNormal = Vector::Zero;
    };
}