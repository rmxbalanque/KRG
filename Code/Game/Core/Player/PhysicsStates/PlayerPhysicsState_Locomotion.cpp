#include "PlayerPhysicsState_Locomotion.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Core/Math/MathHelpers.h"
#include "System/Core/Math/Vector.h"

//------------------------------------------------------------------------- 

namespace KRG::Player
{
    void PlayerLocomotionPhysicsState::Activate()
    {
    }

    void PlayerLocomotionPhysicsState::Deactivate()
    {
    }

    void PlayerLocomotionPhysicsState::PredictMovement()
    {
    }

    bool PlayerLocomotionPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, float const deltaTime, Transform const& deltaTransform )
    {
        Transform const capsuleWorldTransform = pCharacterComponent->GetCapsuleWorldTransform();
        Vector const deltaTranslationWithGravity = deltaTransform.GetTranslation() - Vector( 0.f, 0.f, 10.f * deltaTime );

        //-------------------------------------------------------------------------

        pPhysicsSystem->AcquireReadLock();
        int32 recursion = 0;
        Vector const adjustedDeltaTranslation = SweepCapsule( pPhysicsSystem, pCharacterComponent, capsuleWorldTransform.GetRotation(), capsuleWorldTransform.GetTranslation(), deltaTranslationWithGravity, recursion );
        //KRG_LOG_MESSAGE( "Gameplay", "locomotion sweep took %d recursion", recursion );
        pPhysicsSystem->ReleaseReadLock();

        //-------------------------------------------------------------------------

        Transform finalPhysicsWorldTransform = capsuleWorldTransform;
        finalPhysicsWorldTransform.AddTranslationOffset( adjustedDeltaTranslation );

        auto wt = pCharacterComponent->CalculateWorldTransformFromCapsuleTransform( finalPhysicsWorldTransform );
        wt.SetRotation( deltaTransform.GetRotation() * wt.GetRotation() );
        pCharacterComponent->SetWorldTransform( wt );

        return true;
    }

    Vector PlayerLocomotionPhysicsState::SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent const* pCapsuleComponent, Quaternion const& rotation, Vector const& startPosition, Vector const& deltaTranslation, int32& Idx )
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
        if( pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetCapsuleRadius(), pCapsuleComponent->GetCapsuleOrientation(), startPosition, moveDirection, distance, filter, sweepResults ) )
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

                adjustedDeltaTranslation = SweepCapsule(pPhysicsSystem, pCapsuleComponent, rotation, AjustedStartPosition, ReducedDeltaTranslation, Idx);
            }
            else
            {
                KRG_ASSERT( sweepResults.hasBlock );

                // Purely vertical movement
                if( moveDirection.GetLength2() == 0.f )
                {
                    // Sweep down for collision
                    adjustedDeltaTranslation = sweepResults.GetShapePosition() - pCapsuleComponent->GetCapsulePosition();
                }
                else // 2D movement
                {
                    KRG_ASSERT( moveDirection.GetLength2() > 0.f );

                    Vector const collisionPosition = sweepResults.GetShapePosition();
                    Vector const TranslationDone = collisionPosition - startPosition;

                    Vector const normal = Physics::FromPx( sweepResults.block.normal );
                    float const slopeAngle = Math::GetAngleBetweenNormalizedVectors( normal, Vector::UnitZ );

                    // Collision with the floor
                    static float const MaxNavigableSlopeAngle = Math::PiDivFour;
                    if( slopeAngle < MaxNavigableSlopeAngle )
                    {
                        // Re-project the displacement vertically on the new ground
                        float const remainingDistance = sweepResults.GetRemainingDistance();
                        // the vertical projection is required since a simple projection tend to "suck" the direction along the perpendicular side of the slope
                        Vector const ProjectedDeltaTranslation = Plane::FromNormal( normal ).ProjectVectorVertically( deltaTranslation );
                        Vector const unitProjection = ProjectedDeltaTranslation.GetNormalized3();
                        Vector const newDeltaMovement = unitProjection * Math::Max( 0.f, remainingDistance - Math::HugeEpsilon );
                        KRG_ASSERT( !deltaTranslation.IsEqual3( newDeltaMovement ) );

                        adjustedDeltaTranslation = TranslationDone + SweepCapsule( pPhysicsSystem, pCapsuleComponent, rotation, collisionPosition, newDeltaMovement, Idx );
                    }
                    // collision with a wall / unnavigable slope
                    else
                    {
                        // Re-project the displacement along the wall
                        float const remainingDistance = sweepResults.GetRemainingDistance();
                        // use a regular vector projection since we want to keep the vertically momentum when sliding along a wall;
                        Vector const ProjectedDeltaTranslation = Plane::FromNormal( normal ).ProjectVector( deltaTranslation );
                        Vector const unitProjection = ProjectedDeltaTranslation.GetNormalized3();
                        Vector const newDeltaMovement = unitProjection * Math::Max( 0.f, remainingDistance - Math::HugeEpsilon );
                        KRG_ASSERT( !deltaTranslation.IsEqual3( newDeltaMovement ) );

                        adjustedDeltaTranslation = TranslationDone + SweepCapsule( pPhysicsSystem, pCapsuleComponent, rotation, collisionPosition, newDeltaMovement, Idx );
                    }
                }
            }
        }

        return adjustedDeltaTranslation;
    }
}