#include "PlayerPhysicsState_Air.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"

//------------------------------------------------------------------------- 

namespace KRG::Player
{
    void AirPhysicsState::Activate()
    {
    }

    void AirPhysicsState::Deactivate()
    {
    }

    bool AirPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation )
    {
        // Be careful that deltaTransform only rotate the Z axis !

        Transform const capsuleWorldTransform = pCharacterComponent->GetCapsuleWorldTransform();
        Vector const CurrentVelocity = pCharacterComponent->GetCharacterVelocity();

        float const VerticalVelocity = CurrentVelocity.m_z - ( m_gravityAcceleration * deltaTime );
        Vector const deltaTranslationWithGravity = deltaTranslation + Vector( 0.f, 0.f, VerticalVelocity * deltaTime ) + ( m_impulseVelocity * deltaTime );

        // clear the impulse 
        m_impulseVelocity = Vector::Zero;

        //-------------------------------------------------------------------------

        pPhysicsWorld->AcquireReadLock();

        int32 recursion = 0;
        Vector const adjustedDeltaTranslation = SweepCapsule( pPhysicsWorld, pCharacterComponent, capsuleWorldTransform.GetTranslation(), deltaTranslationWithGravity, recursion );

        pPhysicsWorld->ReleaseReadLock();

        //-------------------------------------------------------------------------

        Transform finalCapsuleWorldTransform = capsuleWorldTransform;
        finalCapsuleWorldTransform.AddTranslationOffset( adjustedDeltaTranslation );

        auto CharacterWorldTransform = pCharacterComponent->CalculateWorldTransformFromCapsuleTransform( finalCapsuleWorldTransform );
        CharacterWorldTransform.SetRotation( deltaRotation * CharacterWorldTransform.GetRotation() );
        pCharacterComponent->MoveCharacter( deltaTime, CharacterWorldTransform );

        return true;
    }

    KRG::Vector AirPhysicsState::SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent const* pCapsuleComponent, Vector const& startPosition, Vector const& deltaTranslation, int32& Idx )
    {
        Idx++;

        Vector moveDirection;
        float distance = 0.0f;
        deltaTranslation.ToDirectionAndLength3( moveDirection, distance );

        // Don't sweep if the distance is insignificant
        if( distance < Math::LargeEpsilon )
        {
            // No movement applied
            return Vector::Zero;
        }

        Vector adjustedDeltaTranslation = deltaTranslation;

        Physics::QueryFilter filter;
        filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
        filter.AddIgnoredEntity( pCapsuleComponent->GetEntityID() );

        Physics::SweepResults sweepResults;
        if( pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCapsuleCylinderPortionHalfHeight(), pCapsuleComponent->GetCapsuleRadius(), pCapsuleComponent->GetCapsuleOrientation(), startPosition, moveDirection, distance, filter, sweepResults ) )
        {
            if( sweepResults.hasBlock && sweepResults.block.hadInitialOverlap() )
            {
                // Sometime PhysX will detect an initial overlap with 0.0 penetration depth,
                // this is due to the discrepancy between math of the scene query (sweep) and the geo query (use for penetration dept calculation).
                // Read this for more info : 
                // https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/BestPractices.html#character-controller-systems-using-scene-queries-and-penetration-depth-computation

                Vector const normal = Physics::FromPx( sweepResults.block.normal );
                float const penetrationDistance = Math::Abs( sweepResults.block.distance ) + Math::HugeEpsilon;
                Vector const AjustedStartPosition = startPosition + ( normal * penetrationDistance );
                // Reduce the delta translation to prevent stack overflow
                Vector const ReducedDeltaTranslation = moveDirection * Math::Max( 0.f, distance - Math::HugeEpsilon );

                adjustedDeltaTranslation = SweepCapsule( pPhysicsSystem, pCapsuleComponent, AjustedStartPosition, ReducedDeltaTranslation, Idx );
            }
            else
            {
                KRG_ASSERT( sweepResults.hasBlock );
                adjustedDeltaTranslation = sweepResults.GetShapePosition() - pCapsuleComponent->GetPosition();
            }
        }

        return adjustedDeltaTranslation;
    }
}
