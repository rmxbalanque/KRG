#include "PlayerPhysicsState_Locomotion.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCapsule.h"
#include "../Math/MathHelpers.h"

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

    Transform PlayerLocomotionPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CapsuleComponent const* pCapsuleComponent, float const deltaTime, Quaternion const& deltaRotation, Vector const& deltaTranslation )
    {
        Transform const currentWorldTransform = pCapsuleComponent->GetWorldTransform();
        Transform finalWorldTransform = currentWorldTransform;
        //Vector const deltaTranslationWithGravity = deltaTranslation - Vector( 0.0f, 0.0f, 5.0f * deltaTime );

        if( deltaTranslation.GetLengthSquared3() == 0.0f )
        {
            return finalWorldTransform;
        }

        //-------------------------------------------------------------------------

        pPhysicsSystem->AcquireReadLock();

        //-------------------------------------------------------------------------
        // DEBUG 
        {
            Physics::QueryFilter filter;
            filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
            filter.AddIgnoredEntity( pCapsuleComponent->GetEntityID() );
            Physics::OverlapResults overlapResult;
            if( pPhysicsSystem->CapsuleOverlap( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), currentWorldTransform.GetRotation(), currentWorldTransform.GetTranslation(), filter, overlapResult ) )
            {
                KRG_LOG_MESSAGE( "Physics", "Started the move in overlap" );
            }

            Physics::SweepResults sweepResults;
            bool sf = pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), pCapsuleComponent->GetOrientation(), currentWorldTransform.GetTranslation(), deltaTranslation.GetNormalized3(), deltaTranslation.GetLength3(), filter, sweepResults );
            if( sweepResults.hasBlock && sweepResults.block.hadInitialOverlap() )
            {
                KRG_LOG_MESSAGE( "physics", "Sweep Fired" );
            }
        }

        //-------------------------------------------------------------------------

        {
            Vector adjustedDeltaTranslation = SweepCapsule( pPhysicsSystem, pCapsuleComponent, currentWorldTransform.GetRotation(), currentWorldTransform.GetTranslation(), deltaTranslation, 0 );
            finalWorldTransform.AddTranslationOffset( adjustedDeltaTranslation );
        }

        //-------------------------------------------------------------------------
        // DEBUG 

        {
            Physics::QueryFilter filter;
            filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
            filter.AddIgnoredEntity( pCapsuleComponent->GetEntityID() );
            Physics::OverlapResults overlapResult;
            if( pPhysicsSystem->CapsuleOverlap( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), finalWorldTransform.GetRotation(), finalWorldTransform.GetTranslation(), filter, overlapResult ) )
            {
                KRG_LOG_MESSAGE( "Physics", "Ended the move in overlap" );
            }
        }
        //-------------------------------------------------------------------------

        pPhysicsSystem->ReleaseReadLock();
        return finalWorldTransform;
    }

    Vector PlayerLocomotionPhysicsState::SweepCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CapsuleComponent const* pCapsuleComponent, Quaternion const& rotation, Vector const& startPos, Vector const& deltaTranslation, int32 Idx )
    {
        Vector moveDirection;
        float distance = 0.0f;
        deltaTranslation.ToDirectionAndLength3( moveDirection, distance );

        if( distance < Math::LargeEpsilon )
        {
            return Vector::Zero;
        }

        Vector adjustedDeltaTranslation = deltaTranslation;

        Physics::QueryFilter filter;
        filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
        filter.AddIgnoredEntity( pCapsuleComponent->GetEntityID() );

        Physics::SweepResults sweepResults;
        if( pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), pCapsuleComponent->GetOrientation(), startPos, moveDirection, distance, filter, sweepResults ) )
        {
            if( sweepResults.hasBlock && sweepResults.block.hadInitialOverlap() )
            {
                if( sweepResults.block.distance < 0.0f )
                {
                    // This should not really happen unless we have a really weird geo
                    Vector const normal = Physics::FromPx( sweepResults.block.normal );
                    float const penetrationDistance = sweepResults.block.distance;
                    adjustedDeltaTranslation = normal * ( Math::Abs( penetrationDistance ) + Math::LargeEpsilon );
                    KRG_LOG_MESSAGE( "Physics", "Initial penetration with actor : %s, %.4f", reinterpret_cast<EntityComponent*>( sweepResults.block.actor->userData )->GetName().c_str(), penetrationDistance );
                }
                else
                {
                    // Do nothing we are right next to the collision but not in penetration
                    adjustedDeltaTranslation = Vector::Zero;
                }
            }
            else
            {
                // Purely vertical movement
                if( moveDirection.GetLength2() == 0.f )
                {
                    // Sweep down for collision
                    adjustedDeltaTranslation = sweepResults.GetShapePosition() - pCapsuleComponent->GetPosition();
                }
                else // 2D movement -> try reprojecting along the collision
                {
                    Vector const collisionPosition = sweepResults.GetShapePosition();

                    //-------------------------------------------------------------------------
                    // DEBUG
                    Physics::OverlapResults overlapResult;
                    if( pPhysicsSystem->CapsuleOverlap( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), rotation, collisionPosition, filter, overlapResult ) )
                    {
                        KRG_LOG_MESSAGE( "Physics", "The collision position is in penetration with %s", reinterpret_cast<EntityComponent*>( overlapResult.getAnyHit( 0 ).actor->userData )->GetName().c_str() );
                    }
                    //-------------------------------------------------------------------------

                    Vector const normal = Physics::FromPx( sweepResults.block.normal );
                    //float const slopeAngle = Math::ACos( normal.GetDot3( Vector::UnitZ ) );
                    //if( slopeAngle < Math::PiDivTwo ) // Test for floor. i.e. Slope Angle < 45 degree
                    {
                        float const remainingDistance = Math::Max( 0.0f, distance - sweepResults.block.distance - Math::LargeEpsilon );
                        Vector const unitProjection = ( adjustedDeltaTranslation - ( normal * adjustedDeltaTranslation.GetDot3( normal ) ) ).GetNormalized3();
                        Vector const newDeltaMovement = unitProjection * remainingDistance;
                        Vector const newEndPosition = collisionPosition + newDeltaMovement;

                        adjustedDeltaTranslation = collisionPosition - startPos;

                        //-------------------------------------------------------------------------
                        // DEBUG
                        if( pPhysicsSystem->CapsuleOverlap( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), rotation, startPos + adjustedDeltaTranslation, filter, overlapResult ) )
                        {
                            KRG_LOG_MESSAGE( "Physics", "The adjusted position is in penetration with %s", reinterpret_cast<EntityComponent*>( overlapResult.getAnyHit( 0 ).actor->userData )->GetName().c_str() );
                        }
                        //-------------------------------------------------------------------------

                        adjustedDeltaTranslation += SweepCapsule( pPhysicsSystem, pCapsuleComponent, rotation, collisionPosition, newDeltaMovement, Idx+1 );
                    }
                    //else
                    //{
                    //    // Basic wall collision
                    //    adjustedDeltaTranslation = collisionPosition - startPosition - ( moveDirection * Math::Min( 0.01f, distance ) );
                    //}
                }
            }
        }

        //-------------------------------------------------------------------------
        // DEBUG
        Physics::OverlapResults overlapResult;
        if( pPhysicsSystem->CapsuleOverlap( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), rotation, startPos + adjustedDeltaTranslation, filter, overlapResult ) )
        {
            KRG_LOG_MESSAGE( "Physics", "The \"no\" collision position is in penetration with " );
        }
        //-------------------------------------------------------------------------

        return adjustedDeltaTranslation;
    }
}