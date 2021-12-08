#include "PlayerPhysicsState_Ground.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Core/Math/MathHelpers.h"
#include "System/Core/Math/Vector.h"

//------------------------------------------------------------------------- 

namespace KRG::Player
{
    void GroundPhysicsState::Activate()
    {
    }

    void GroundPhysicsState::Deactivate()
    {
    }

    bool GroundPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation )
    {
        // Be careful that deltaTransform only rotate the Z axis !

        Transform const capsuleWorldTransform = pCharacterComponent->GetCapsuleWorldTransform();
        Vector const CurrentVelocity = pCharacterComponent->GetCharacterVelocity();

        float const VerticalVelocity = CurrentVelocity.m_z - ( m_gravityAcceleration * deltaTime );
        Vector const deltaTranslationWithGravity = deltaTranslation + Vector( 0.f, 0.f, VerticalVelocity * deltaTime );

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

    Vector GroundPhysicsState::SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent const* pCapsuleComponent, Vector const& startPosition, Vector const& deltaTranslation, int32& idx )
    {
        idx++;

        float const initialHorizontalSpeed = deltaTranslation.GetLength2();

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

                adjustedDeltaTranslation = SweepCapsule(pPhysicsSystem, pCapsuleComponent, AjustedStartPosition, ReducedDeltaTranslation, idx);
            }
            else
            {
                KRG_ASSERT( sweepResults.hasBlock );

                // Purely vertical movement
                if( moveDirection.GetLength2() == 0.f )
                {
                    // Sweep down for collision
                    adjustedDeltaTranslation = sweepResults.GetShapePosition() - pCapsuleComponent->GetPosition();
                }
                else // 2D movement
                {
                    KRG_ASSERT( moveDirection.GetLength2() > 0.f );

                    Vector const collisionPosition = sweepResults.GetShapePosition();
                    Vector const TranslationDone = collisionPosition - startPosition;

                    Vector const normal = Physics::FromPx( sweepResults.block.normal );
                    Radians const slopeAngle = Math::GetAngleBetweenNormalizedVectors( normal, Vector::UnitZ );

                    // Collision with the floor
                    if( slopeAngle < m_maxNavigableSlopeAngle )
                    {
                        // Re-project the displacement vertically on the new ground
                        float const remainingDistance = sweepResults.GetRemainingDistance();
                        // the vertical projection is required since a simple projection tend to "suck" the direction along the perpendicular side of the slope
                        Vector const ProjectedDeltaTranslation = Plane::FromNormal( normal ).ProjectVectorVertically( deltaTranslation );
                        Vector const unitProjection = ProjectedDeltaTranslation.GetNormalized3();
                        Vector newDeltaMovement = unitProjection * remainingDistance;
                        KRG_ASSERT( !deltaTranslation.IsEqual3( newDeltaMovement ) );

                        // Clamp the horizontal speed to the initial speed
                        float const newHorizontalSpeed = newDeltaMovement.GetLength2();
                        if( newHorizontalSpeed > initialHorizontalSpeed )
                        {
                            float const ratio = initialHorizontalSpeed / newHorizontalSpeed;
                            newDeltaMovement.m_x *= ratio;
                            newDeltaMovement.m_y *= ratio;
                        }

                        adjustedDeltaTranslation = TranslationDone + SweepCapsule( pPhysicsSystem, pCapsuleComponent, collisionPosition, newDeltaMovement, idx );
                    }
                    // collision with a wall / unnavigable slope
                    else
                    {
                        // Only reproject if the direction is close to (60 degree +) perpendicular to the normal
                        Radians const normalAngle = Math::GetAngleBetweenNormalizedVectors( normal, -moveDirection );
                        if( normalAngle > m_wallSlideAngle )
                        {
                            // Re-project the displacement along the wall
                            float const remainingDistance = sweepResults.GetRemainingDistance();
                            // use a regular vector projection since we want to keep the vertically momentum when sliding along a wall;
                            Vector const ProjectedDeltaTranslation = Plane::FromNormal( normal ).ProjectVector( deltaTranslation );
                            Vector const unitProjection = ProjectedDeltaTranslation.GetNormalized3();
                            Vector newDeltaMovement = unitProjection * remainingDistance;
                            KRG_ASSERT( !deltaTranslation.IsEqual3( newDeltaMovement ) );

                            // Clamp the horizontal speed to the initial speed
                            float const newHorizontalSpeed = newDeltaMovement.GetLength2();
                            if( newHorizontalSpeed > initialHorizontalSpeed )
                            {
                                float const ratio = initialHorizontalSpeed / newHorizontalSpeed;
                                newDeltaMovement.m_x *= ratio;
                                newDeltaMovement.m_y *= ratio;
                            }

                            adjustedDeltaTranslation = TranslationDone + SweepCapsule( pPhysicsSystem, pCapsuleComponent, collisionPosition, newDeltaMovement, idx );
                        }
                        else
                        {
                            adjustedDeltaTranslation = sweepResults.GetShapePosition() - pCapsuleComponent->GetPosition();
                        }
                    }
                }
            }
        }

        return adjustedDeltaTranslation;
    }
}