#include "PhysicsRagdoll.h"
#include "System/Animation/AnimationPose.h"
#include "System/Core/Drawing/DebugDrawing.h"
#include "System/Core/Math/MathHelpers.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool RagdollBodySettings::IsValid() const
    {
        if ( m_mass < 0 )
        {
            return false;
        }

        if ( m_staticFriction < 0 )
        {
            return false;
        }

        if ( m_dynamicFriction < 0 )
        {
            return false;
        }

        if ( m_restitution < 0 || m_restitution > 1 )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( m_stiffness < 0 )
        {
            return false;
        }

        if ( m_damping < 0 )
        {
            return false;
        }

        if ( m_internalCompliance < Math::HugeEpsilon || m_internalCompliance > 1 )
        {
            return false;
        }

        if ( m_externalCompliance < Math::HugeEpsilon || m_externalCompliance > 1 )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( m_twistLimitMin < -180 || m_twistLimitMin > 180 )
        {
            return false;
        }

        if ( m_twistLimitMax < -180 || m_twistLimitMax > 180 )
        {
            return false;
        }

        if ( m_twistLimitMin >= m_twistLimitMax )
        {
            return false;
        }

        if ( m_twistLimitContactDistance < 0 || m_twistLimitContactDistance > 180 )
        {
            return false;
        }

        // The contact distance should be less than half the distance between the upper and lower limits.
        if ( m_twistLimitContactDistance > ( ( m_twistLimitMax - m_twistLimitMin ) / 2 ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( m_swingLimitY < 1 || m_swingLimitY > 180 )
        {
            return false;
        }

        if ( m_swingLimitZ < 1 || m_swingLimitZ > 180 )
        {
            return false;
        }

        if ( m_tangentialStiffness < 0 )
        {
            return false;
        }

        if ( m_tangentialDamping < 0 )
        {
            return false;
        }

        if ( m_swingLimitContactDistance < 0 || m_swingLimitContactDistance > 180 )
        {
            return false;
        }

        // The contact distance should be less than either limit angle
        if ( m_swingLimitContactDistance > m_swingLimitY || m_swingLimitContactDistance > m_swingLimitZ )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        return true;
    }

    bool RagdollBodySettings::CorrectSettingsToValidRanges()
    {
        bool settingsUpdated = false;

        if ( m_mass < 0 )
        {
            m_mass = 1.0f;
            settingsUpdated = true;
        }

        if ( m_staticFriction < 0 )
        {
            m_staticFriction = 0.0f;
            settingsUpdated = true;
        }

        if ( m_dynamicFriction < 0 )
        {
            m_dynamicFriction = 0.0f;
            settingsUpdated = true;
        }

        if ( m_restitution < 0 || m_restitution > 1 )
        {
            m_restitution = 0.0f;
            settingsUpdated = true;
        }

        //-------------------------------------------------------------------------

        if ( m_stiffness < 0 )
        {
            m_stiffness = 0.0f;
            settingsUpdated = true;
        }

        if ( m_damping < 0 )
        {
            m_damping = 0.0f;
            settingsUpdated = true;
        }

        if ( m_internalCompliance < Math::HugeEpsilon || m_internalCompliance > 1 )
        {
            m_internalCompliance = Math::HugeEpsilon;
            settingsUpdated = true;
        }

        if (  m_internalCompliance > 1 )
        {
            m_internalCompliance = 1.0f;
            settingsUpdated = true;
        }

        if ( m_externalCompliance < Math::HugeEpsilon || m_externalCompliance > 1 )
        {
            m_externalCompliance = Math::HugeEpsilon;
            settingsUpdated = true;
        }

        if ( m_externalCompliance > 1 )
        {
            m_externalCompliance = 1.0f;
            settingsUpdated = true;
        }

        //-------------------------------------------------------------------------

        if ( m_twistLimitMin < 1 )
        {
            m_twistLimitMin = 1;
            settingsUpdated = true;
        }

        if ( m_twistLimitMin > 180 )
        {
            m_twistLimitMin = 180;
            settingsUpdated = true;
        }

        if ( m_twistLimitMax < 1 )
        {
            m_twistLimitMax = 1;
            settingsUpdated = true;
        }

        if ( m_twistLimitMax > 180 )
        {
            m_twistLimitMax = 180;
            settingsUpdated = true;
        }

        if ( m_twistLimitMin >= m_twistLimitMax )
        {
            // Try to keep the max limit fixed and ensure at least 2 degrees of limit
            if ( m_twistLimitMax > 2 )
            {
                m_twistLimitMin = m_twistLimitMax - 2;
            }
            else
            {
                m_twistLimitMin = 1;
                m_twistLimitMax = 3;
            }

            settingsUpdated = true;
        }

        if ( m_twistLimitContactDistance < 0 )
        {
            m_twistLimitContactDistance = 0;
            settingsUpdated = true;
        }

        if ( m_twistLimitContactDistance > 180 )
        {
            m_twistLimitContactDistance = 180;
            settingsUpdated = true;
        }

        // The contact distance should be less than half the distance between the upper and lower limits.
        float const maxTwistContactDistance = ( m_twistLimitMax - m_twistLimitMin ) / 2;
        if ( m_twistLimitContactDistance >= maxTwistContactDistance )
        {
            m_twistLimitContactDistance = maxTwistContactDistance - 0.5f;
            settingsUpdated = true;
        }

        //-------------------------------------------------------------------------

        if ( m_swingLimitY < 1 )
        {
            m_swingLimitY = 1;
            settingsUpdated = true;
        }

        if( m_swingLimitY > 180 )
        {
            m_swingLimitY = 180;
            settingsUpdated = true;
        }

        if ( m_swingLimitZ < 1 )
        {
            m_swingLimitZ = 1;
            settingsUpdated = true;
        }

        if ( m_swingLimitZ > 180 )
        {
            m_swingLimitZ = 180;
            settingsUpdated = true;
        }

        if ( m_tangentialStiffness < 0 )
        {
            m_tangentialStiffness = 0.0f;
            settingsUpdated = true;
        }

        if ( m_tangentialDamping < 0 )
        {
            m_tangentialDamping = 0.0f;
            settingsUpdated = true;
        }

        if ( m_swingLimitContactDistance < 0 )
        {
            m_swingLimitContactDistance = 0;
            settingsUpdated = true;
        }

        if ( m_swingLimitContactDistance > 180 )
        {
            m_swingLimitContactDistance = 180;
            settingsUpdated = true;
        }

        // The contact distance should be less than either limit angle
        float const maxSwingContactDistance = Math::Min( m_swingLimitY, m_swingLimitZ );
        if ( m_swingLimitContactDistance >= maxSwingContactDistance )
        {
            m_swingLimitContactDistance = maxSwingContactDistance - 1.0f;
            settingsUpdated = true;
        }

        //-------------------------------------------------------------------------

        return settingsUpdated;
    }

    //-------------------------------------------------------------------------

    bool RagdollSolverSettings::IsValid() const
    {
        if ( m_solverPositionIterations == 0 )
        {
            return false;
        }

        if ( m_solverVelocityIterations == 0 )
        {
            return false;
        }

        if ( m_stabilizationThreshold < 0 )
        {
            return false;
        }

        if ( m_sleepThreshold < 0 )
        {
            return false;
        }

        if ( m_separationTolerance < 0 )
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    bool RagdollDefinition::IsValid() const
    {
        if ( !m_pSkeleton.IsValid() )
        {
            return false;
        }

        if ( m_profiles.empty() )
        {
            return false;
        }

        // Validate the profiles
        for ( auto& profile : m_profiles )
        {
            if ( !profile.m_ID.IsValid() )
            {
                return false;
            }

            if ( !profile.m_solverSettings.IsValid() )
            {
                return false;
            }

            if ( profile.m_bodySettings.size() != m_bodies.size() )
            {
                return false;
            }

            for ( auto& bodySettings : profile.m_bodySettings )
            {
                if ( !bodySettings.IsValid() )
                {
                    return false;
                }
            }
        }

        return true;
    }

    //-------------------------------------------------------------------------

    bool RagdollDefinition::Profile::CorrectSettingsToValidRanges()
    {
        bool updated = false;
        for ( auto& bodySettings : m_bodySettings )
        {
            updated |= bodySettings.CorrectSettingsToValidRanges();
        }

        return updated;
    }

    //-------------------------------------------------------------------------

    RagdollDefinition::Profile* RagdollDefinition::GetProfile( StringID profileID )
    {
        for ( auto& profile : m_profiles )
        {
            if ( profile.m_ID == profileID )
            {
                return &profile;
            }
        }

        return nullptr;
    }

    int32 RagdollDefinition::GetBodyIndexForBoneID( StringID boneID ) const
    {
        for ( auto bodyIdx = 0; bodyIdx < m_bodies.size(); bodyIdx++ )
        {
            if ( m_bodies[bodyIdx].m_boneID == boneID )
            {
                return bodyIdx;
            }
        }

        return InvalidIndex;
    }

    int32 RagdollDefinition::GetBodyIndexForBoneIdx( int32 boneIdx ) const
    {
        auto const& boneID = m_pSkeleton->GetBoneID( boneIdx );
        return GetBodyIndexForBoneID( boneID );
    }

    void RagdollDefinition::CreateRuntimeData()
    {
        KRG_ASSERT( m_pSkeleton.IsLoaded() );

        int32 const numBones = m_pSkeleton->GetNumBones();
        int32 const numBodies = (int32) m_bodies.size();

        TInlineVector<Transform, 30> inverseBodyTransforms;

        // Calculate bone mappings and runtime helper transforms
        //-------------------------------------------------------------------------

        m_boneToBodyMap.clear();
        m_bodyToBoneMap.clear();

        m_boneToBodyMap.resize( numBones, InvalidIndex );
        m_bodyToBoneMap.resize( numBodies, InvalidIndex );
        inverseBodyTransforms.resize( numBodies );

        for ( int32 boneIdx = 0; boneIdx < numBones; boneIdx++ )
        {
            auto const& boneID = m_pSkeleton->GetBoneID( boneIdx );
            for ( int32 bodyIdx = 0; bodyIdx < numBodies; bodyIdx++ )
            {
                if ( m_bodies[bodyIdx].m_boneID == boneID )
                {
                    m_bodies[bodyIdx].m_initialGlobalTransform = m_bodies[bodyIdx].m_offsetTransform * m_pSkeleton->GetBoneGlobalTransform( boneIdx );
                    m_bodies[bodyIdx].m_inverseOffsetTransform = m_bodies[bodyIdx].m_offsetTransform.GetInverse();

                    inverseBodyTransforms[bodyIdx] = m_bodies[bodyIdx].m_initialGlobalTransform.GetInverse();

                    m_boneToBodyMap[boneIdx] = bodyIdx;
                    m_bodyToBoneMap[bodyIdx] = boneIdx;
                    break;
                }
            }
        }

        // Calculate parent bone indices and joint relative transforms
        for ( int32 bodyIdx = 0; bodyIdx < numBodies; bodyIdx++ )
        {
            auto& body = m_bodies[bodyIdx];
            body.m_parentBodyIdx = InvalidIndex;

            // Traverse hierarchy and set parent body idx to the first parent bone that has a body
            int32 boneIdx = m_pSkeleton->GetBoneIndex( body.m_boneID );
            int32 parentBoneIdx = m_pSkeleton->GetParentBoneIndex( boneIdx );
            while ( parentBoneIdx != InvalidIndex )
            {
                int32 const parentBodyIdx = m_boneToBodyMap[parentBoneIdx];
                if ( parentBodyIdx != InvalidIndex )
                {
                    body.m_parentBodyIdx = parentBodyIdx;
                    break;
                }
                else
                {
                    parentBoneIdx = m_pSkeleton->GetParentBoneIndex( parentBoneIdx );
                }
            }

            //-------------------------------------------------------------------------

            if ( body.m_parentBodyIdx != InvalidIndex )
            {
                body.m_parentRelativeTransform = body.m_jointTransform * inverseBodyTransforms[body.m_parentBodyIdx];
                body.m_childRelativeTransform = body.m_jointTransform * inverseBodyTransforms[bodyIdx];
            }
        }

        // Validate body parenting
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        int32 numRootBodies = 0;
        for ( int32 bodyIdx = 0; bodyIdx < numBodies; bodyIdx++ )
        {
            KRG_ASSERT( m_bodies[bodyIdx].m_parentBodyIdx != bodyIdx);

            if ( m_bodies[bodyIdx].m_parentBodyIdx == InvalidIndex )
            {
                numRootBodies++;
            }
        }
        KRG_ASSERT( numRootBodies == 1 );
        #endif
    }
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    Ragdoll::Ragdoll( PxPhysics* pPhysics, RagdollDefinition const* pDefinition, StringID const profileID )
        : m_pDefinition( pDefinition )
    {
        KRG_ASSERT( pPhysics != nullptr );
        KRG_ASSERT( pDefinition != nullptr && pDefinition->IsValid() );
        KRG_ASSERT( profileID.IsValid() ? pDefinition->HasProfile( profileID ) : true );

        if ( profileID.IsValid() )
        {
            m_pProfile = pDefinition->GetProfile( profileID );
        }
        else
        {
            m_pProfile = pDefinition->GetDefaultProfile();
        }
        KRG_ASSERT( m_pProfile != nullptr );

        m_pArticulation = pPhysics->createArticulation();
        KRG_ASSERT( m_pArticulation != nullptr );

        // Set articulation settings
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_ragdollName.sprintf( "%s_%s", pDefinition->m_ID.c_str(), m_pProfile->m_ID.c_str() );
        m_pArticulation->setName( m_ragdollName.c_str() );
        #endif

        UpdateSolverSettings( m_pProfile->m_solverSettings );

        // Create links
        //-------------------------------------------------------------------------

        int32 const numBodies = pDefinition->GetNumBodies();
        for ( int32 bodyIdx = 0; bodyIdx < numBodies; bodyIdx++ )
        {
            auto const& bodyDefinition = pDefinition->m_bodies[bodyIdx];
            auto const& bodySettings = m_pProfile->m_bodySettings[bodyIdx];

            PxTransform const linkPose = ToPx( bodyDefinition.m_initialGlobalTransform );
            PxArticulationLink* pParentLink = ( bodyIdx == 0 ) ? nullptr : m_links[bodyDefinition.m_parentBodyIdx];
            PxArticulationLink* pLink = m_pArticulation->createLink( pParentLink, linkPose );
            m_links.emplace_back( pLink );

            #if KRG_DEVELOPMENT_TOOLS
            pLink->setName( bodyDefinition.m_boneID.c_str() );
            #endif

            // Set Mass
            KRG_ASSERT( bodySettings.m_mass != 0.0f );
            PxRigidBodyExt::setMassAndUpdateInertia( *pLink, bodySettings.m_mass );

            // Create material
            auto pMaterial = pPhysics->createMaterial( bodySettings.m_staticFriction, bodySettings.m_dynamicFriction, bodySettings.m_restitution );
            pMaterial->setFrictionCombineMode( (PxCombineMode::Enum) bodySettings.m_frictionCombineMode );
            pMaterial->setRestitutionCombineMode( (PxCombineMode::Enum) bodySettings.m_restitutionCombineMode );

            // Create shape
            PxCapsuleGeometry const capsuleGeo( bodyDefinition.m_radius, bodyDefinition.m_halfHeight );
            auto pShape = PxRigidActorExt::createExclusiveShape( *pLink, capsuleGeo, *pMaterial );
            pMaterial->release();
        }

        // Create joints
        //-------------------------------------------------------------------------

        for ( int32 bodyIdx = 1; bodyIdx < numBodies; bodyIdx++ )
        {
            PxArticulationJoint* pJoint = static_cast<PxArticulationJoint*>( m_links[bodyIdx]->getInboundJoint() );
            pJoint->setParentPose( ToPx( pDefinition->m_bodies[bodyIdx].m_parentRelativeTransform ) );
            pJoint->setChildPose( ToPx( pDefinition->m_bodies[bodyIdx].m_childRelativeTransform ) );
            pJoint->setTargetVelocity( PxZero );
            pJoint->setTargetOrientation( PxIdentity );
        }

        UpdateJointSettings( m_pProfile->m_bodySettings );
    }

    Ragdoll::~Ragdoll()
    {
        KRG_ASSERT( m_pArticulation->getScene() == nullptr );

        m_pArticulation->release();
        m_pArticulation = nullptr;
        m_links.empty();
        m_pDefinition = nullptr;
    }

    //-------------------------------------------------------------------------

    void Ragdoll::UpdateSolverSettings( RagdollSolverSettings const& solverSettings )
    {
        KRG_ASSERT( IsValid() );

        LockWriteScene();
        {
            m_pArticulation->setSolverIterationCounts( solverSettings.m_solverPositionIterations, solverSettings.m_solverVelocityIterations );
            m_pArticulation->setStabilizationThreshold( solverSettings.m_stabilizationThreshold );
            m_pArticulation->setMaxProjectionIterations( solverSettings.m_maxProjectionIterations );
            m_pArticulation->setSleepThreshold( solverSettings.m_sleepThreshold );
            m_pArticulation->setSeparationTolerance( solverSettings.m_separationTolerance );
            m_pArticulation->setInternalDriveIterations( solverSettings.m_internalDriveIterations );
            m_pArticulation->setExternalDriveIterations( solverSettings.m_externalDriveIterations );
        }
        UnlockWriteScene();
    }

    void Ragdoll::UpdateJointSettings( TVector<RagdollBodySettings> const& settings )
    {
        int32 const numJoints = (int32) m_links.size() - 1;

        KRG_ASSERT( IsValid() );

        LockWriteScene();
        {
            // Root settings
            //-------------------------------------------------------------------------

            // TODO

            // Joints
            //-------------------------------------------------------------------------

            for ( int32 bodyIdx = 1; bodyIdx < numJoints; bodyIdx++ )
            {
                auto const& bodySettings = settings[bodyIdx];
                PxArticulationJoint* pJoint = static_cast<PxArticulationJoint*>( m_links[bodyIdx]->getInboundJoint() );

                pJoint->setDriveType( PxArticulationJointDriveType::eTARGET );

                pJoint->setDamping( bodySettings.m_damping );
                pJoint->setStiffness( bodySettings.m_stiffness );

                pJoint->setInternalCompliance( bodySettings.m_internalCompliance );
                pJoint->setExternalCompliance( bodySettings.m_externalCompliance );

                //-------------------------------------------------------------------------

                pJoint->setTwistLimitEnabled( bodySettings.m_twistLimitEnabled );
                pJoint->setTwistLimit( Math::DegreesToRadians * bodySettings.m_twistLimitMin, Math::DegreesToRadians * bodySettings.m_twistLimitMax );
                pJoint->setTwistLimitContactDistance( Math::DegreesToRadians * bodySettings.m_twistLimitContactDistance );

                pJoint->setSwingLimitEnabled( bodySettings.m_swingLimitEnabled );
                pJoint->setSwingLimit( Math::DegreesToRadians * bodySettings.m_swingLimitZ, Math::DegreesToRadians * bodySettings.m_swingLimitY );
                pJoint->setSwingLimitContactDistance( Math::DegreesToRadians * bodySettings.m_swingLimitContactDistance );
                pJoint->setTangentialStiffness( bodySettings.m_tangentialStiffness );
                pJoint->setTangentialDamping( bodySettings.m_tangentialDamping );
            }
        }
        UnlockWriteScene();
    }

    void Ragdoll::UpdateMassAndMaterialSettings( TVector<RagdollBodySettings> const& settings )
    {
        int32 const numBodies = (int32) m_links.size();

        KRG_ASSERT( IsValid() );
        KRG_ASSERT( settings.size() == numBodies );

        LockWriteScene();
        {
            for ( int32 bodyIdx = 0; bodyIdx < numBodies; bodyIdx++ )
            {
                PxShape* pShape = nullptr;
                int32 const numShapesFound = m_links[bodyIdx]->getShapes( &pShape, 1 );
                KRG_ASSERT( numShapesFound == 1 );

                auto const& bodySettings = settings[bodyIdx];

                // Set Mass
                PxRigidBodyExt::setMassAndUpdateInertia( *m_links[bodyIdx], bodySettings.m_mass );

                // Create material
                auto& physics = m_pArticulation->getScene()->getPhysics();
                PxMaterial* pMaterial = physics.createMaterial( bodySettings.m_staticFriction, bodySettings.m_dynamicFriction, bodySettings.m_restitution );
                pMaterial->setFrictionCombineMode( (PxCombineMode::Enum) bodySettings.m_frictionCombineMode );
                pMaterial->setRestitutionCombineMode( (PxCombineMode::Enum) bodySettings.m_restitutionCombineMode );

                // Update materials
                pShape->setMaterials( &pMaterial, 1 );
                pMaterial->release();
            }
        }
        UnlockWriteScene();
    }

    //-------------------------------------------------------------------------

    bool Ragdoll::IsSleeping() const
    {
        KRG_ASSERT( IsValid() );
        LockReadScene();
        bool const isSleeping = m_pArticulation->isSleeping();
        UnlockReadScene();

        return isSleeping;
    }

    void Ragdoll::PutToSleep()
    {
        KRG_ASSERT( IsValid() );
        LockWriteScene();
        m_pArticulation->putToSleep();
        UnlockWriteScene();
    }

    void Ragdoll::WakeUp()
    {
        KRG_ASSERT( IsValid() );
        LockWriteScene();
        m_pArticulation->wakeUp();
        UnlockWriteScene();
    }

    void Ragdoll::SetGravityEnabled( bool isGravityEnabled )
    {
        KRG_ASSERT( IsValid() );

        if ( m_gravityEnabled == isGravityEnabled )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_gravityEnabled = isGravityEnabled;

        LockWriteScene();
        int32 const numBodies = (int32) m_links.size();
        for ( auto i = 0; i < numBodies; i++ )
        {
            m_links[i]->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, !isGravityEnabled );
        }
        UnlockWriteScene();
    }

    void Ragdoll::SetPoseFollowingEnabled( bool shouldFollowPose )
    {
        if ( m_shouldFollowPose == shouldFollowPose )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_shouldFollowPose = shouldFollowPose;

        // Clear any set targets when disabling pose following
        if ( !m_shouldFollowPose ) 
        {
            LockWriteScene();
            int32 const numBodies = (int32) m_links.size();
            for ( int32 i = 1; i < numBodies; i++ )
            {
                auto pJoint = static_cast<PxArticulationJoint*>( m_links[i]->getInboundJoint() );
                pJoint->setTargetVelocity( PxZero );
                pJoint->setTargetOrientation( PxIdentity );
            }
            UnlockWriteScene();
        }
    }

    //-------------------------------------------------------------------------

    void Ragdoll::SetTargetPose( Seconds const deltaTime, Transform const& worldTransform, Animation::Pose* pPose )
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( pPose->GetSkeleton()->GetResourceID() == m_pDefinition->m_pSkeleton.GetResourceID() );
        KRG_ASSERT( pPose->HasGlobalTransforms() );

        //-------------------------------------------------------------------------

        if ( m_shouldFollowPose )
        {
            LockWriteScene();

            // Handle Root
            //-------------------------------------------------------------------------

            if ( m_pProfile->m_bodySettings[0].m_driveType == RagdollDriveType::Kinematic )
            {
                int32 const rootBoneIdx = m_pDefinition->m_bodyToBoneMap[0];
                Transform const rootAnimWorldTransform =  pPose->GetGlobalTransform( rootBoneIdx ) * worldTransform;
                Transform const rootBodyWorldTransform = m_pDefinition->m_bodies[0].m_offsetTransform * rootAnimWorldTransform;
                m_links[0]->setGlobalPose( ToPx( rootBodyWorldTransform ) );
            }
            else
            {
                // TODO: spring constraint
            }

            // Handle Joints
            //-------------------------------------------------------------------------

            int32 const numBodies = (int32) m_links.size();
            for ( int32 i = 1; i < numBodies; i++ )
            {
                auto pJoint = static_cast<PxArticulationJoint*>( m_links[i]->getInboundJoint() );
                int32 const childBoneIdx = m_pDefinition->m_bodyToBoneMap[i];
                Transform const childBoneWorldTransform = pPose->GetGlobalTransform( childBoneIdx ) * worldTransform;
                Transform const childBodyWorldTransform = m_pDefinition->m_bodies[i].m_offsetTransform * childBoneWorldTransform;

                if ( m_pProfile->m_bodySettings[i].m_driveType == RagdollDriveType::Kinematic )
                {
                    m_links[i]->setGlobalPose( ToPx( childBodyWorldTransform ) );
                    pJoint->setTargetVelocity( PxZero );
                    pJoint->setTargetOrientation( PxIdentity );
                }
                else // Driven
                {
                    int32 const parentBoneIdx = m_pDefinition->m_bodyToBoneMap[m_pDefinition->m_bodies[i].m_parentBodyIdx];
                    Transform const parentBoneWorldTransform = pPose->GetGlobalTransform( parentBoneIdx ) * worldTransform;
                    Transform const parentBodyWorldTransform = m_pDefinition->m_bodies[m_pDefinition->m_bodies[i].m_parentBodyIdx].m_offsetTransform * parentBoneWorldTransform;

                    Transform const jointWorldTransformRelativeToParent = m_pDefinition->m_bodies[i].m_parentRelativeTransform * parentBodyWorldTransform;
                    Transform const jointWorldTransformRelativeToChild = m_pDefinition->m_bodies[i].m_childRelativeTransform * childBodyWorldTransform;

                    // Calculate the different in orientation of the joint relative to parent space
                    Transform const jointDelta = jointWorldTransformRelativeToChild * jointWorldTransformRelativeToParent.GetInverse();

                    if ( m_pProfile->m_bodySettings[i].m_driveType == RagdollDriveType::Spring )
                    {
                        pJoint->setTargetOrientation( ToPx( jointDelta.GetRotation() ) );
                    }
                    else if ( m_pProfile->m_bodySettings[i].m_driveType == RagdollDriveType::Velocity )
                    {
                        pJoint->setTargetOrientation( ToPx( jointDelta.GetRotation() ) );

                        Vector const angularVelocity = Math::CalculateAngularVelocity( jointWorldTransformRelativeToParent.GetRotation(), jointWorldTransformRelativeToChild.GetRotation(), deltaTime );
                        pJoint->setTargetVelocity( ToPx( angularVelocity ) );
                    }
                }
            }
            UnlockWriteScene();
        }
    }

    void Ragdoll::GetPose( Transform const& inverseWorldTransform, Animation::Pose* pPose ) const
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( pPose->GetSkeleton()->GetResourceID() == m_pDefinition->m_pSkeleton.GetResourceID() );
        KRG_ASSERT( pPose->HasGlobalTransforms() );

        // Copy the global bone transforms here
        //-------------------------------------------------------------------------

        int32 const numBones = pPose->GetNumBones();
        m_globalBoneTransforms.resize( numBones );
        m_globalBoneTransforms = pPose->GetGlobalTransforms();

        //-------------------------------------------------------------------------

        LockReadScene();
        int32 const numBodies = (int32) m_links.size();
        for ( int32 bodyIdx = 0; bodyIdx < numBodies; bodyIdx++ )
        {
            int32 const boneIdx = m_pDefinition->m_bodyToBoneMap[bodyIdx];
            Transform const bodyWorldTransform = FromPx( m_links[bodyIdx]->getGlobalPose() );
            Transform const boneWorldTranform = m_pDefinition->m_bodies[bodyIdx].m_inverseOffsetTransform * bodyWorldTransform;
            m_globalBoneTransforms[boneIdx] = boneWorldTranform * inverseWorldTransform;
        }
        UnlockReadScene();

        // Calculate the local transforms and set back into the pose
        //-------------------------------------------------------------------------

        for ( int32 i = 0; i < numBones; i++ )
        {
            // Keep original local transforms for non-body bones
            if ( m_pDefinition->m_boneToBodyMap[i] == InvalidIndex )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            int32 const parentBoneIdx = m_pDefinition->m_pSkeleton->GetParentBoneIndex( i );
            if ( parentBoneIdx != InvalidIndex )
            {
                Transform const boneLocalTransform = m_globalBoneTransforms[i] * m_globalBoneTransforms[parentBoneIdx].GetInverse();
                pPose->SetTransform( i, boneLocalTransform );
            }
            else
            {
                pPose->SetTransform( i, m_globalBoneTransforms[i] );
            }
        }
    }

    void Ragdoll::GetRagdollPose( RagdollPose& pose ) const
    {
        KRG_ASSERT( IsValid() );

        int32 const numBodies = m_pDefinition->GetNumBodies();
        pose.resize( numBodies );

        LockReadScene();
        for ( auto i = 0; i < numBodies; i++ )
        {
            pose[i] = FromPx( m_links[i]->getGlobalPose() );
        }
        UnlockReadScene();
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void Ragdoll::DrawDebug( Drawing::DrawContext& ctx ) const
    {
        RagdollPose pose;
        GetRagdollPose( pose );

        int32 const numBodies = m_pDefinition->GetNumBodies();
        for ( int32 i = 0; i < numBodies; i++ )
        {
            auto const& bodySettings = m_pDefinition->m_bodies[i];
            ctx.DrawCapsuleHeightX( pose[i], bodySettings.m_radius, bodySettings.m_halfHeight, Colors::Yellow, 2.0f );
        }
    }
    #endif
}