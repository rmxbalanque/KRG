#pragma once

#include "PhysX.h"
#include "PhysicsMaterial.h"
#include "System/Animation/AnimationSkeleton.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Math/NumericRange.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxScene;
}

namespace KRG::Animation
{
    class Pose;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    //-------------------------------------------------------------------------
    // Ragdoll Settings
    //-------------------------------------------------------------------------

    enum class RagdollDriveType : uint8
    {
        KRG_REGISTER_ENUM

        Kinematic = 0,
        Spring = 1,
        Velocity = 2,
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_PHYSICS_API RagdollBodySettings : public IRegisteredType
    {
        KRG_SERIALIZE_MEMBERS(  m_mass, m_staticFriction, m_dynamicFriction, m_restitution, m_frictionCombineMode, m_restitutionCombineMode, m_driveType,
                                m_twistLimitEnabled, m_swingLimitEnabled, m_stiffness, m_damping, m_internalCompliance, m_externalCompliance, m_twistLimitMin, m_twistLimitMax, m_twistLimitContactDistance,
                                m_swingLimitY, m_swingLimitZ, m_tangentialStiffness, m_tangentialDamping, m_swingLimitContactDistance );
        KRG_REGISTER_TYPE( RagdollBodySettings );

        // Are all the settings valid
        bool IsValid() const;

        // Ensures that all body settings are within the required limits. Returns true if any changes were made!
        bool CorrectSettingsToValidRanges();

        // Reset all settings to default
        void Reset() { *this = RagdollBodySettings(); }

    public:

        // Mass and Material Settings
        //-------------------------------------------------------------------------

        KRG_EXPOSE float                                m_mass = 1.0f;
        KRG_EXPOSE float                                m_staticFriction = PhysicsMaterial::DefaultStaticFriction;
        KRG_EXPOSE float                                m_dynamicFriction = PhysicsMaterial::DefaultDynamicFriction;
        KRG_EXPOSE float                                m_restitution = PhysicsMaterial::DefaultRestitution;
        KRG_EXPOSE PhysicsCombineMode                   m_frictionCombineMode = PhysicsCombineMode::Average;
        KRG_EXPOSE PhysicsCombineMode                   m_restitutionCombineMode = PhysicsCombineMode::Average;

        // Joint Settings
        //-------------------------------------------------------------------------

        KRG_EXPOSE RagdollDriveType                     m_driveType = RagdollDriveType::Kinematic;
        KRG_EXPOSE bool                                 m_twistLimitEnabled = false;
        KRG_EXPOSE bool                                 m_swingLimitEnabled = false;

        // General
        KRG_EXPOSE float                                m_stiffness = 0.0f;
        KRG_EXPOSE float                                m_damping = 0.0f;

        KRG_EXPOSE float                                m_internalCompliance = 1.0f;
        KRG_EXPOSE float                                m_externalCompliance = 1.0f;

        // Twist
        KRG_EXPOSE float                                m_twistLimitMin = -45;
        KRG_EXPOSE float                                m_twistLimitMax = 45;
        KRG_EXPOSE float                                m_twistLimitContactDistance = 3.0f;

        // Swing
        KRG_EXPOSE float                                m_swingLimitY = 45.0f;
        KRG_EXPOSE float                                m_swingLimitZ = 45.0f;
        KRG_EXPOSE float                                m_tangentialStiffness = 0.0f; // Only for swing limit
        KRG_EXPOSE float                                m_tangentialDamping = 0.0f; // Only for swing limit
        KRG_EXPOSE float                                m_swingLimitContactDistance = 3.0f;
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_PHYSICS_API RagdollSolverSettings : public IRegisteredType
    {
        KRG_SERIALIZE_MEMBERS( m_solverPositionIterations, m_solverVelocityIterations, m_stabilizationThreshold, m_maxProjectionIterations, m_sleepThreshold, m_separationTolerance, m_internalDriveIterations, m_externalDriveIterations );
        KRG_REGISTER_TYPE( RagdollSolverSettings );

        // Are all the settings valid
        bool IsValid() const;

        // Reset all settings to default
        void Reset() { *this = RagdollSolverSettings(); }

    public:

        KRG_EXPOSE uint32                               m_solverPositionIterations = 4;
        KRG_EXPOSE uint32                               m_solverVelocityIterations = 4;
        KRG_EXPOSE uint32                               m_maxProjectionIterations = 4;
        KRG_EXPOSE uint32                               m_internalDriveIterations = 4;
        KRG_EXPOSE uint32                               m_externalDriveIterations = 4;
        KRG_EXPOSE float                                m_separationTolerance = 0.1f;
        KRG_EXPOSE float                                m_stabilizationThreshold = 0.01f * Constants::s_speedScale * Constants::s_speedScale;
        KRG_EXPOSE float                                m_sleepThreshold = 1;
    };

    //-------------------------------------------------------------------------
    // Ragdoll Definition
    //-------------------------------------------------------------------------
    // This defines a ragdoll: The bodies and joints as well as the various joint profiles

    struct KRG_ENGINE_PHYSICS_API RagdollDefinition : public Resource::IResource, public IRegisteredType
    {
        KRG_SERIALIZE_MEMBERS( m_ID, m_pSkeleton, m_bodies, m_profiles );
        KRG_REGISTER_TYPE_RESOURCE( 'RGDL', "Physics Ragdoll", RagdollDefinition );

    public:

        struct KRG_ENGINE_PHYSICS_API BodyDefinition : public IRegisteredType
        {
            KRG_SERIALIZE_MEMBERS( m_boneID, m_offsetTransform, m_radius, m_halfHeight, m_jointTransform );
            KRG_REGISTER_TYPE( BodyDefinition );

            KRG_REGISTER StringID                       m_boneID;
            int32                                       m_parentBodyIdx = InvalidIndex;
            KRG_EXPOSE float                            m_radius = 0.075f;
            KRG_EXPOSE float                            m_halfHeight = 0.025f;
            KRG_EXPOSE Transform                        m_offsetTransform;
            Transform                                   m_initialGlobalTransform;
            Transform                                   m_inverseOffsetTransform;
            KRG_EXPOSE Transform                        m_jointTransform; // Global joint transform
            Transform                                   m_childRelativeTransform;
            Transform                                   m_parentRelativeTransform;
        };

        struct KRG_ENGINE_PHYSICS_API Profile : public IRegisteredType
        {
            KRG_SERIALIZE_MEMBERS( m_ID, m_bodySettings, m_solverSettings );
            KRG_REGISTER_TYPE( Profile );

            // Ensures that all body settings are within the required limits. Returns true if any changes were made!
            bool CorrectSettingsToValidRanges();

        public:

            KRG_EXPOSE StringID                         m_ID = StringID( "Default" );
            KRG_EXPOSE TVector<RagdollBodySettings>     m_bodySettings;
            KRG_EXPOSE RagdollSolverSettings            m_solverSettings;
        };

    public:

        virtual bool IsValid() const override;

        void CreateRuntimeData();

        int32 GetNumBodies() const { return (int32) m_bodies.size(); }
        int32 GetBodyIndexForBoneID( StringID boneID ) const;
        int32 GetBodyIndexForBoneIdx( int32 boneIdx ) const;

        // Profiles
        //-------------------------------------------------------------------------

        inline int32 GetNumProfiles() const { return (int32) m_profiles.size(); }
        Profile const* GetDefaultProfile() const { return &m_profiles[0]; }
        Profile* GetDefaultProfile() { return &m_profiles[0]; }
        Profile* GetProfile( StringID profileID );
        Profile const* GetProfile( StringID profileID ) const { return const_cast<RagdollDefinition*>( this )->GetProfile( profileID ); }
        bool HasProfile( StringID profileID ) const { return GetProfile( profileID ) != nullptr; }

    public:

        KRG_REGISTER StringID                           m_ID;
        KRG_EXPOSE TResourcePtr<Animation::Skeleton>    m_pSkeleton;
        KRG_EXPOSE TVector<BodyDefinition>              m_bodies;
        KRG_EXPOSE TVector<Profile>                     m_profiles;

        // Runtime Data
        TVector<int32>                                  m_boneToBodyMap;
        TVector<int32>                                  m_bodyToBoneMap;
    };

    //-------------------------------------------------------------------------
    // Ragdoll Instance
    //-------------------------------------------------------------------------

    using RagdollPose = TInlineVector<Transform, 40>;

    class KRG_ENGINE_PHYSICS_API Ragdoll
    {
        friend class PhysicsWorldSystem;

    public:

        Ragdoll( physx::PxPhysics* pPhysics, RagdollDefinition const* pDefinition, StringID const profileID = StringID() );
        ~Ragdoll();

        bool IsValid() const { return m_pArticulation != nullptr; }

        // Ragdoll Controls
        //-------------------------------------------------------------------------

        void SetGravityEnabled( bool isGravityEnabled );
        inline void SetPoseFollowingEnabled( bool shouldFollowPose );

        bool IsSleeping() const;
        void PutToSleep();
        void WakeUp();

        // Update / Results
        //-------------------------------------------------------------------------

        void SetTargetPose( Seconds const deltaTime, Transform const& worldTransform, Animation::Pose* pPose );
        void GetPose( Transform const& inverseWorldTransform, Animation::Pose* pPose ) const;
        void GetRagdollPose( RagdollPose& pose ) const;

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void DrawDebug( Drawing::DrawContext& ctx ) const;
        #endif

    private:

        void UpdateMassAndMaterialSettings( TVector<RagdollBodySettings> const& bodySettings );
        void UpdateSolverSettings( RagdollSolverSettings const& solverSettings );
        void UpdateJointSettings( TVector<RagdollBodySettings> const& bodySettings );

        KRG_FORCE_INLINE void LockWriteScene()
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->lockWrite();
            }
        }

        KRG_FORCE_INLINE void UnlockWriteScene()
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->unlockWrite();
            }
        }

        KRG_FORCE_INLINE void LockReadScene() const
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->lockRead();
            }
        }

        KRG_FORCE_INLINE void UnlockReadScene() const
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->unlockRead();
            }
        }

    private:

        RagdollDefinition const*                    m_pDefinition = nullptr;
        RagdollDefinition::Profile const*           m_pProfile = nullptr;
        physx::PxArticulation*                      m_pArticulation = nullptr;
        TVector<physx::PxArticulationLink*>         m_links;
        bool                                        m_shouldFollowPose = false;
        bool                                        m_gravityEnabled = true;
        mutable TVector<Transform>                  m_globalBoneTransforms;

        #if KRG_DEVELOPMENT_TOOLS
        InlineString<100>                           m_ragdollName;
        #endif
    };
}