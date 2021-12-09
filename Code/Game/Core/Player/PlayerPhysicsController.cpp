#include "PlayerPhysicsController.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Core/Math/MathHelpers.h"
#include "System/Core/Math/Vector.h"

//------------------------------------------------------------------------- 

namespace KRG::Player
{
    bool CharacterPhysicsController::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation )
    {
        // Be careful that deltaTransform only rotate the Z axis !

        Transform const capsuleWorldTransform = pCharacterComponent->GetCapsuleWorldTransform();
        Vector const CurrentVelocity = pCharacterComponent->GetCharacterVelocity();

        float VerticalSpeed = 0.0f;
        if ( m_isGravityEnable )
        {
            VerticalSpeed += CurrentVelocity.m_z - ( m_gravityAcceleration * deltaTime );
        }
        Vector const deltaTranslationWithGravity = deltaTranslation + Vector( 0.f, 0.f, VerticalSpeed * deltaTime );

        //-------------------------------------------------------------------------

        pPhysicsWorld->AcquireReadLock();

        int32 recursion = 0;
        auto MoveResult = SweepCapsule( pPhysicsWorld, pCharacterComponent, capsuleWorldTransform.GetTranslation(), deltaTranslationWithGravity, recursion );
        pPhysicsWorld->ReleaseReadLock();

        //-------------------------------------------------------------------------

        // this is a horrible hack
        if ( MoveResult.NeededDepenetration() )
        {
            Transform finalCapsuleWorldTransform = capsuleWorldTransform;
            finalCapsuleWorldTransform.SetTranslation( MoveResult.m_correctedPosition );

            auto CharacterWorldTransform = pCharacterComponent->CalculateWorldTransformFromCapsuleTransform( finalCapsuleWorldTransform );
            CharacterWorldTransform.SetRotation( deltaRotation * CharacterWorldTransform.GetRotation() );
            // this is only needed since we don't want to account for the depenetration displacement in the velocity
            pCharacterComponent->TeleportCharacter( CharacterWorldTransform );
        }

        Transform finalCapsuleWorldTransform = capsuleWorldTransform;
        finalCapsuleWorldTransform.SetTranslation( MoveResult.m_finalPosition );

        auto CharacterWorldTransform = pCharacterComponent->CalculateWorldTransformFromCapsuleTransform( finalCapsuleWorldTransform );
        CharacterWorldTransform.SetRotation( deltaRotation * CharacterWorldTransform.GetRotation() );
        pCharacterComponent->MoveCharacter( deltaTime, CharacterWorldTransform );

        return true;
    }

    CharacterPhysicsController::MoveResult CharacterPhysicsController::SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent const* pCapsuleComponent, Vector const& startPosition, Vector const& deltaTranslation, int32& Idx )
    {
        MoveResult moveResult( startPosition );
        Idx++;

        if ( Idx > 10 )
        {
            return moveResult;
        }

        Vector moveDirection;
        float distance = 0.0f;
        deltaTranslation.ToDirectionAndLength3( moveDirection, distance );
        if ( distance < Math::Epsilon )
        {
            return moveResult;
        }

        Vector adjustedDeltaTranslation = deltaTranslation;

        Physics::QueryFilter filter;
        filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
        filter.AddIgnoredEntity( pCapsuleComponent->GetEntityID() );

        Physics::SweepResults sweepResults;
        if ( pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCapsuleCylinderPortionHalfHeight(), pCapsuleComponent->GetCapsuleRadius(), pCapsuleComponent->GetCapsuleOrientation(), startPosition, moveDirection, distance, filter, sweepResults ) )
        {
            if ( sweepResults.hasBlock && sweepResults.block.hadInitialOverlap() )
            {
                // Sometime PhysX will detect an initial overlap with 0.0 penetration depth,
                // this is due to the discrepancy between math of the scene query (sweep) and the geo query (use for penetration dept calculation).
                // Read this for more info : 
                // https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/BestPractices.html#character-controller-systems-using-scene-queries-and-penetration-depth-computation

                Vector const normal = Physics::FromPx( sweepResults.block.normal );
                float const penetrationDistance = Math::Abs( sweepResults.block.distance ) + Math::HugeEpsilon;
                Vector const correctedStartPosition = startPosition + ( normal * penetrationDistance );

                auto const depenetratedResult = SweepCapsule( pPhysicsSystem, pCapsuleComponent, correctedStartPosition, deltaTranslation, Idx );
                moveResult.ApplyCorrectiveMove( depenetratedResult );
            }
            else
            {
                KRG_ASSERT( sweepResults.hasBlock );

                // Purely vertical movement
                if ( moveDirection.GetLength2() == 0.f )
                {
                    // Sweep down for collision
                    adjustedDeltaTranslation = sweepResults.GetShapePosition() - pCapsuleComponent->GetPosition();
                }
                else // 2D movement
                {
                    KRG_ASSERT( moveDirection.GetLength2() > 0.f );

                    float const initialHorizontalSpeed = deltaTranslation.GetLength2();
                    Vector const collisionPosition = sweepResults.GetShapePosition();
                    Vector const TranslationDone = collisionPosition - startPosition;

                    Vector const normal = Physics::FromPx( sweepResults.block.normal );
                    Radians const slopeAngle = Math::GetAngleBetweenNormalizedVectors( normal, Vector::UnitZ );

                    // Collision with the floor
                    if ( slopeAngle < m_maxNavigableSlopeAngle )
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
                        if ( newHorizontalSpeed > initialHorizontalSpeed )
                        {
                            float const ratio = initialHorizontalSpeed / newHorizontalSpeed;
                            newDeltaMovement.m_x *= ratio;
                            newDeltaMovement.m_y *= ratio;
                        }

                        auto const reprojectedResult = SweepCapsule( pPhysicsSystem, pCapsuleComponent, collisionPosition, newDeltaMovement, Idx );
                        moveResult.ApplySubsequentMove( reprojectedResult );
                    }
                    // collision with a wall / unnavigable slope
                    else
                    {
                        // Only reproject if the direction is close to (60 degree +) perpendicular to the normal
                        Radians const normalAngle = Math::GetAngleBetweenNormalizedVectors( normal, -moveDirection );
                        if ( normalAngle > m_wallSlideAngle )
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
                            if ( newHorizontalSpeed > initialHorizontalSpeed )
                            {
                                float const ratio = initialHorizontalSpeed / newHorizontalSpeed;
                                newDeltaMovement.m_x *= ratio;
                                newDeltaMovement.m_y *= ratio;
                            }

                            auto const reprojectedResult = SweepCapsule( pPhysicsSystem, pCapsuleComponent, collisionPosition, newDeltaMovement, Idx );
                            moveResult.ApplySubsequentMove( reprojectedResult );
                        }
                        else
                        {
                            moveResult.m_finalPosition = sweepResults.GetShapePosition();
                            moveResult.m_remainingDistance = sweepResults.GetRemainingDistance();
                        }
                    }
                }
            }
        }
        else
        {
            moveResult.m_finalPosition = startPosition + deltaTranslation;
            moveResult.m_remainingDistance = 0.0f;
        }

        return moveResult;
    }
}