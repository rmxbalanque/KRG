#include "PlayerPhysicsController.h"
#include "Engine/Physics/PhysicsScene.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "System/Core/Math/MathHelpers.h"

//------------------------------------------------------------------------- 

namespace KRG::Player
{
    bool CharacterPhysicsController::TryMoveCapsule( EntityWorldUpdateContext const& ctx, Physics::Scene* pPhysicsScene, Vector const& deltaTranslation, Quaternion const& deltaRotation )
    {
        // Be careful that deltaTransform only rotate the Z axis !
        Vector const StepHeightOffset = Vector( 0.0f, 0.0f, 0.5f * m_settings.m_stepHeight );

        Transform capsuleWorldTransform = m_pCharacterComponent->GetCapsuleWorldTransform();
        if( m_isStepHeightEnabled )
        {
            capsuleWorldTransform.AddTranslation( StepHeightOffset );
        }

        float const capsuleRadius = m_pCharacterComponent->GetCapsuleRadius();
        float const CylinderHalfHeight = m_isStepHeightEnabled ? 0.5f * ( m_pCharacterComponent->GetCapsuleHeight() - m_settings.m_stepHeight ) : m_pCharacterComponent->GetCapsuleCylinderPortionHalfHeight();
        Vector const stepHeightOffset = m_isStepHeightEnabled ? Vector( 0.f, 0.f, m_settings.m_stepHeight ) : Vector::Zero;

        //-------------------------------------------------------------------------

        pPhysicsScene->AcquireReadLock();

        Vector deltaMovement = deltaTranslation;
        if( m_isOnGround && m_ProjectOntoFloor )
        {
            Vector const projection = Plane::FromNormal( m_floorNormal ).ProjectVectorVertically( deltaTranslation );
            deltaMovement = projection.GetNormalized3() * deltaTranslation.GetLength3();
        }

        // Move horizontally
        int32 moveRecursion = 0;
        auto moveResult = SweepCylinder( pPhysicsScene, CylinderHalfHeight, capsuleRadius, capsuleWorldTransform.GetTranslation(), deltaMovement, moveRecursion );

        // Update gravity if needed
        Vector verticalAjustement;
        if( m_isGravityEnabled )
        {
            m_verticalSpeed -= ( m_settings.m_gravitationalAcceleration * ctx.GetDeltaTime() );
            verticalAjustement = Vector::UnitZ * m_verticalSpeed * ctx.GetDeltaTime();
        }
        else
        {
            m_verticalSpeed = 0.f;
            verticalAjustement = Vector::Zero;
        }

        // Move vertically
        int32 verticalMoveRecursion = 0;
        auto const  verticalMoveResult = SweepCylinderVertical( pPhysicsScene, CylinderHalfHeight, capsuleRadius, moveResult.GetFinalPosition(), verticalAjustement, stepHeightOffset, verticalMoveRecursion );

        // Collided with a floor
        if( verticalMoveResult.GetSweepResults().hasBlock )
        {
            Vector const normal = Physics::FromPx( verticalMoveResult.GetSweepResults().block.normal );
            if( Math::GetAngleBetweenNormalizedVectors( normal, Vector::UnitZ ) < m_settings.m_maxNavigableSlopeAngle )
            {
                // Reset vertical speed
                m_verticalSpeed = 0.0;
                m_isOnGround = true;
                m_floorNormal = normal;
            }
            else
            {
                m_isOnGround = false;
            }
        }
        else
        {
            m_isOnGround = false;
        }

        //-------------------------------------------------------------------------

        Transform finalCapsuleWorldTransform( m_pCharacterComponent->GetCapsuleOrientation(), verticalMoveResult.GetFinalPosition() );
        if( m_isStepHeightEnabled )
        {
            finalCapsuleWorldTransform.AddTranslation( -StepHeightOffset );
        }

        if( m_isOnGround )
        {
            // Adjust the position to the capsule collision with the floor, required for not floating on slope
            MoveResult const capsuleMoveResult = AdjustCapsuleToGround( pPhysicsScene, m_pCharacterComponent->GetCapsuleCylinderPortionHalfHeight(), m_pCharacterComponent->GetCapsuleRadius(), finalCapsuleWorldTransform, m_settings.m_stepHeight );
            finalCapsuleWorldTransform.SetTranslation( capsuleMoveResult.GetFinalPosition() );
        }

        pPhysicsScene->ReleaseReadLock();

        auto characterWorldTransform = m_pCharacterComponent->CalculateWorldTransformFromCapsuleTransform( finalCapsuleWorldTransform );
        characterWorldTransform.SetRotation( deltaRotation * characterWorldTransform.GetRotation() );

        m_pCharacterComponent->MoveCharacter( ctx.GetDeltaTime(), characterWorldTransform );

        return true;
    }

    CharacterPhysicsController::MoveResult CharacterPhysicsController::SweepCylinder( Physics::Scene* pPhysicsScene, float cylinderHalfHeight, float cylinderRadius, Vector const& startPosition, Vector const& deltaTranslation, int32& idx )
    {
        MoveResult moveResult( startPosition );
        idx++;

        if ( idx > 10 )
        {
            return moveResult;
        }

        bool onlyApplyDepenetration = false;

        Vector moveDirection;
        float distance = 0.0f;
        deltaTranslation.ToDirectionAndLength3( moveDirection, distance );

        if ( distance < Math::Epsilon )
        {
            // we need to force a direction and distance to test for initial penetration even if the displacement is null.
            moveDirection = -Vector::UnitZ;
            distance = 0.01;
            onlyApplyDepenetration = true;
        }

        Vector adjustedDeltaTranslation = deltaTranslation;

        Physics::QueryFilter filter;
        filter.SetLayerMask( m_settings.m_physicsLayerMask );
        filter.AddIgnoredEntity( m_pCharacterComponent->GetEntityID() );
        for( auto IgnoredActor : m_settings.m_ignoredActors )
        {
            filter.AddIgnoredEntity( IgnoredActor );
        }

        Physics::SweepResults sweepResults;
        if ( pPhysicsScene->CylinderSweep( cylinderHalfHeight, cylinderRadius, m_pCharacterComponent->GetCapsuleOrientation(), startPosition, moveDirection, distance, filter, sweepResults ) )
        {
            if ( sweepResults.hasBlock &&sweepResults.block.hadInitialOverlap() )
            {
                // Sometime PhysX will detect an initial overlap with 0.0 penetration depth,
                // this is due to the discrepancy between math of the scene query (sweep) and the geo query (use for penetration dept calculation).
                // Read this for more info : 
                // https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/BestPractices.html#character-controller-systems-using-scene-queries-and-penetration-depth-computation

                Vector const normal = Physics::FromPx( sweepResults.block.normal );
                float const penetrationDistance = Math::Abs( sweepResults.block.distance ) + Math::HugeEpsilon;
                Vector const correctedStartPosition = startPosition + ( normal * penetrationDistance );

                auto const depenetratedResult = SweepCylinder( pPhysicsScene, cylinderHalfHeight, cylinderRadius, correctedStartPosition, deltaTranslation, idx );
                moveResult.ApplyCorrectiveMove( depenetratedResult );
            }
            else
            {
                KRG_ASSERT( sweepResults.hasBlock );

                // We have no movement and only want to apply depenetration, we didn't have any penetration here, so return without moving
                if( onlyApplyDepenetration )
                {
                    return moveResult;
                }

                // Purely vertical movement
                if ( moveDirection.GetLength2() == 0.f )
                {
                    // Sweep down for collision
                    adjustedDeltaTranslation = sweepResults.GetShapePosition() - m_pCharacterComponent->GetPosition();
                }
                else // 2D movement
                {
                    KRG_ASSERT( moveDirection.GetLength2() > 0.f );

                    float const initialHorizontalSpeed = deltaTranslation.GetLength2();
                    Vector const collisionPosition = sweepResults.GetShapePosition();
                    Vector const translationDone = collisionPosition - startPosition;

                    Vector const normal = Physics::FromPx( sweepResults.block.normal );
                    Radians const slopeAngle = Math::GetAngleBetweenNormalizedVectors( normal, Vector::UnitZ );

                    // Collision with the floor
                    if ( slopeAngle < m_settings.m_maxNavigableSlopeAngle )
                    {
                        // Re-project the displacement vertically on the new ground
                        float const remainingDistance = sweepResults.GetRemainingDistance();
                        // the vertical projection is required since a simple projection tend to "suck" the direction along the perpendicular side of the slope
                        Vector const projectedDeltaTranslation = Plane::FromNormal( normal ).ProjectVectorVertically( deltaTranslation );
                        Vector const unitProjection = projectedDeltaTranslation.GetNormalized3();
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

                        auto const reprojectedResult = SweepCylinder( pPhysicsScene, cylinderHalfHeight, cylinderRadius, collisionPosition, newDeltaMovement, idx );
                        moveResult.ApplySubsequentMove( reprojectedResult );
                    }
                    // collision with a wall / unnavigable slope
                    else
                    {
                        // Only reproject if the direction is close to (60 degree +) perpendicular to the normal
                        Radians const normalAngle = Math::GetAngleBetweenNormalizedVectors( normal, -moveDirection );
                        if ( normalAngle > m_settings.m_wallSlideAngle )
                        {
                            // Re-project the displacement along the wall
                            float const remainingDistance = sweepResults.GetRemainingDistance();
                            // use a regular vector projection since we want to keep the vertically momentum when sliding along a wall;
                            Vector const projectedDeltaTranslation = Plane::FromNormal( normal ).ProjectVector( deltaTranslation );
                            Vector const unitProjection = projectedDeltaTranslation.GetNormalized3();
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

                            auto const reprojectedResult = SweepCylinder( pPhysicsScene, cylinderHalfHeight, cylinderRadius, collisionPosition, newDeltaMovement, idx );
                            moveResult.ApplySubsequentMove( reprojectedResult );
                        }
                        else
                        {
                            moveResult.FinalisePosition( sweepResults );
                        }
                    }
                }
            }
        }
        else
        {
            moveResult.FinalisePosition( sweepResults );
        }

        return moveResult;
    }

    CharacterPhysicsController::MoveResult CharacterPhysicsController::SweepCylinderVertical( Physics::Scene* pPhysicsScene, float cylinderHalfHeight, float cylinderRadius, Vector const& startPosition, Vector const& deltaTranslation, Vector const& stepHeightOffset, int32& idx )
    {
        MoveResult moveResult( startPosition );

        Vector const downSweep = deltaTranslation - stepHeightOffset;

        Vector moveDirection;
        float distance = 0.0f;
        downSweep.ToDirectionAndLength3( moveDirection, distance );

        bool onlyApplyDepenetration = false;
        if( distance < Math::Epsilon )
        {
            // we need to force a direction and distance to test for initial penetration even if the displacement is null.
            moveDirection = -Vector::UnitZ;
            distance = 0.01;
            onlyApplyDepenetration = true;
        }

        Physics::QueryFilter filter;
        filter.SetLayerMask( m_settings.m_physicsLayerMask );
        filter.AddIgnoredEntity( m_pCharacterComponent->GetEntityID() );
        for( auto IgnoredActor : m_settings.m_ignoredActors )
        {
            filter.AddIgnoredEntity( IgnoredActor );
        }

        Physics::SweepResults sweepResults;
        if( pPhysicsScene->CylinderSweep( cylinderHalfHeight, cylinderRadius, m_pCharacterComponent->GetCapsuleOrientation(), startPosition, moveDirection, distance, filter, sweepResults ) )
        {
            if( sweepResults.hasBlock && sweepResults.block.hadInitialOverlap() )
            {
                // This should not happen since we swept vertically and resolved collision earlier, but we should handle it just in case I'm proved wrong
                KRG_ASSERT( false ); // this assumption need to be validated

                // Sometime PhysX will detect an initial overlap with 0.0 penetration depth,
                // this is due to the discrepancy between math of the scene query (sweep) and the geo query (use for penetration dept calculation).
                // Read this for more info : 
                // https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/BestPractices.html#character-controller-systems-using-scene-queries-and-penetration-depth-computation

                Vector const normal = Physics::FromPx( sweepResults.block.normal );
                float const penetrationDistance = Math::Abs( sweepResults.block.distance ) + Math::HugeEpsilon;
                Vector const correctedStartPosition = startPosition + ( normal * penetrationDistance );

                auto const depenetratedResult = SweepCylinderVertical( pPhysicsScene, cylinderHalfHeight, cylinderRadius, correctedStartPosition, deltaTranslation, stepHeightOffset, idx );
                moveResult.ApplyCorrectiveMove( depenetratedResult );
            }
            else
            {
                if( onlyApplyDepenetration )
                {
                    return moveResult;
                }

                Vector const normal = Physics::FromPx( sweepResults.block.normal );

                // Only reproject if the direction is close to (60 degree +) perpendicular to the normal
                Radians const normalAngle = Math::GetAngleBetweenNormalizedVectors( normal, -moveDirection );
                if( normalAngle > m_settings.m_maxNavigableSlopeAngle )
                {
                    // Re-project the displacement along the wall
                    Vector const collisionPosition = sweepResults.GetShapePosition();
                    float const remainingDistance = sweepResults.GetRemainingDistance();

                    // use a regular vector projection since we want to keep the vertically momentum when sliding along a wall;
                    Vector const projectedDeltaTranslation = Plane::FromNormal( normal ).ProjectVector( downSweep );
                    Vector const unitProjection = projectedDeltaTranslation.GetNormalized3();
                    Vector const newDeltaMovement = unitProjection * remainingDistance;
                    KRG_ASSERT( !deltaTranslation.IsEqual3( newDeltaMovement ) );

                    auto const reprojectedResult = SweepCylinderVertical( pPhysicsScene, cylinderHalfHeight, cylinderRadius, collisionPosition, newDeltaMovement, Vector::Zero, idx );
                    moveResult.ApplySubsequentMove( reprojectedResult, stepHeightOffset );
                }
                else
                {
                    moveResult.FinalisePosition( sweepResults, stepHeightOffset );
                }
            }
        }
        else
        {
            moveResult.FinalisePosition( sweepResults, stepHeightOffset );
        }

        return moveResult;
    }

    CharacterPhysicsController::MoveResult CharacterPhysicsController::AdjustCapsuleToGround( Physics::Scene* pPhysicsScene, float capsuleCylinderPortionHalfHeight, float capsuleRadius, Transform const& transform, float distance )
    {
        MoveResult moveResult( transform.GetTranslation() );

        Physics::QueryFilter filter;
        filter.SetLayerMask( m_settings.m_physicsLayerMask );
        filter.AddIgnoredEntity( m_pCharacterComponent->GetEntityID() );
        for( auto IgnoredActor : m_settings.m_ignoredActors )
        {
            filter.AddIgnoredEntity( IgnoredActor );
        }

        Physics::SweepResults sweepResults;
        pPhysicsScene->CapsuleSweep( capsuleCylinderPortionHalfHeight, capsuleRadius - 0.01f, transform.GetRotation(), transform.GetTranslation(), -Vector::UnitZ, distance, filter, sweepResults );
        KRG_ASSERT( sweepResults.hasBlock ? !sweepResults.block.hadInitialOverlap() : true );

        moveResult.FinalisePosition( sweepResults );
        return moveResult;
    }
}