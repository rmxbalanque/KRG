#include "PlayerPhysicsState_Locomotion.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCapsule.h"

//------------------------------------------------------------------------- 

namespace KRG::Player
{
    void PlayerLocomotionPhysicsState::Activate()
    {
        m_verticalVelocity = 0.0f;
    }

    void PlayerLocomotionPhysicsState::Deactivate()
    {
        m_verticalVelocity = 0.0f;
    }

    void PlayerLocomotionPhysicsState::PredictMovement()
    {
        //// Linear gravity
        ////-------------------------------------------------------------------------

        //Vector const gravityStartPosition = startPosition + adjustedDeltaTranslation;
        //m_verticalVelocity += -2.f * ctx.GetDeltaTime();
        //Vector const gravityDelta = Vector( 0.f, 0.f, m_verticalVelocity );
        //Vector gravityEndPosition = gravityStartPosition + gravityDelta;
        //Physics::SweepResults gravityResults;

        //if ( pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), pCapsuleComponent->GetOrientation(), gravityStartPosition, gravityEndPosition, filter, gravityResults ) )
        //{
        //    if ( gravityResults.hasBlock )
        //    {
        //        gravityEndPosition = gravityResults.GetCollisionPosition() + Vector( 0.f, 0.f, 0.01f );
        //        m_verticalVelocity = 0.0f;
        //    }
        //}
        //deltaMovement = gravityEndPosition - startPosition;
    }

    Transform PlayerLocomotionPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CapsuleComponent const* pCapsuleComponent, Quaternion const& deltaRotation, Vector const& deltaTranslation )
    {
        Transform const currentWorldTransform = pCapsuleComponent->GetWorldTransform();
        Vector const startPosition = currentWorldTransform.GetTranslation();
        Vector const desiredEndPosition = startPosition + deltaTranslation;
        Transform finalWorldTransform = currentWorldTransform;
        Vector adjustedDeltaTranslation = deltaTranslation;

        Vector moveDirection;
        float distance = 0.0f;
        deltaTranslation.ToDirectionAndLength3( moveDirection, distance );

        if ( distance == 0.0f )
        {
            return finalWorldTransform;
        }

        //-------------------------------------------------------------------------

        Physics::QueryFilter filter;
        filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
        filter.AddIgnoredEntity( pCapsuleComponent->GetEntityID() );

        pPhysicsSystem->AcquireReadLock();
        {
            Physics::SweepResults results;

            bool bInitialCollision = false;
            KRG_ASSERT( pCapsuleComponent->GetOrientation().IsNormalized() );
            if ( pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), pCapsuleComponent->GetOrientation(), startPosition, desiredEndPosition, filter, results ) )
            {
                if ( results.block.hadInitialOverlap() )
                {
                    bInitialCollision = true;
                    KRG_LOG_MESSAGE( "Physics", "Initial collision detected" );
                    Vector const normal = Physics::FromPx( results.block.normal );
                    Vector const newstartPosition = startPosition + normal * 0.01f;
                    pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), pCapsuleComponent->GetOrientation(), newstartPosition, desiredEndPosition, filter, results );
                }

                if ( results.block.hadInitialOverlap() )
                {
                    KRG_LOG_MESSAGE( "Physics", "Initial collision : Failed to resolve" );
                    Vector const collisionPosition = results.GetCollisionPosition();
                    adjustedDeltaTranslation = collisionPosition - startPosition;
                }
                else if ( results.hasBlock && !results.block.hadInitialOverlap() )
                {
                    if ( bInitialCollision )
                    {
                        KRG_LOG_MESSAGE( "Physics", "Initial collision: Succeded to resolve" );
                    }

                    Vector const collisionPosition = results.GetCollisionPosition();
                    Vector const normal = Physics::FromPx( results.block.normal );
                    float const slopeAngle = Math::ACos( normal.GetDot3( Vector::UnitZ ) );
                    if ( slopeAngle < Math::PiDivTwo ) // Test for floor. i.e. Slope Angle < 45 degree
                    {
                        float const remainingDistance = distance - results.block.distance;
                        Vector const unitProjection = ( adjustedDeltaTranslation - ( normal * adjustedDeltaTranslation.GetDot3( normal ) ) ).GetNormalized3();
                        Vector const newDeltaMovement = unitProjection * remainingDistance;
                        Vector const newEndPosition = collisionPosition + newDeltaMovement;

                        // Project only once for now, but this might need to be recursive
                        if ( pPhysicsSystem->CapsuleSweep( pCapsuleComponent->GetCylinderPortionHalfHeight(), pCapsuleComponent->GetRadius(), pCapsuleComponent->GetOrientation(), collisionPosition, newEndPosition, filter, results ) )
                        {
                            // Basic wall collision
                            adjustedDeltaTranslation = results.GetCollisionPosition() - pCapsuleComponent->GetPosition() - ( unitProjection * 0.01f );
                        }
                        else
                        {
                            adjustedDeltaTranslation = newEndPosition - startPosition;
                        }
                        // Recursive
                    }
                    else
                    {
                        // Basic wall collision
                        adjustedDeltaTranslation = collisionPosition - pCapsuleComponent->GetPosition() - ( moveDirection * 0.01f );
                    }

                    //if( results.block.hadInitialOverlap() )
                    //{
                    //	KRG_LOG_MESSAGE( "Physics", "overlap: %s", results.block.actor->getName() );
                    //}
                    //KRG_LOG_MESSAGE( "Physics", "block: %s", results.block.actor->getName() );
                }
            }
        }
        pPhysicsSystem->ReleaseReadLock();

        //-------------------------------------------------------------------------
        
        finalWorldTransform.AddTranslationOffset( adjustedDeltaTranslation );
        return finalWorldTransform;
    }
}