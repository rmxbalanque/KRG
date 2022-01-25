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

    struct KRG_ENGINE_PHYSICS_API RagdollBodyMaterialSettings : public IRegisteredType
    {
        KRG_SERIALIZE_MEMBERS( m_staticFriction, m_dynamicFriction, m_restitution, m_frictionCombineMode, m_restitutionCombineMode );
        KRG_REGISTER_TYPE( RagdollBodyMaterialSettings );

    public:

        bool IsValid() const;

        // Ensures that all material settings are within the required limits. Returns true if any changes were made!
        bool CorrectSettingsToValidRanges();

    public:

        KRG_EXPOSE float                                    m_staticFriction = PhysicsMaterial::DefaultStaticFriction;
        KRG_EXPOSE float                                    m_dynamicFriction = PhysicsMaterial::DefaultDynamicFriction;
        KRG_EXPOSE float                                    m_restitution = PhysicsMaterial::DefaultRestitution;
        KRG_EXPOSE PhysicsCombineMode                       m_frictionCombineMode = PhysicsCombineMode::Average;
        KRG_EXPOSE PhysicsCombineMode                       m_restitutionCombineMode = PhysicsCombineMode::Average;
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_PHYSICS_API RagdollRootControlBodySettings : public IRegisteredType
    {
        KRG_SERIALIZE_MEMBERS( m_driveType, m_maxDistance, m_tolerance, m_stiffness, m_damping );
        KRG_REGISTER_TYPE( RagdollRootControlBodySettings );

        enum DriveType : uint8
        {
            KRG_REGISTER_ENUM

            None = 0,
            Distance = 1,
            Spring = 2
        };

    public:

        bool IsValid() const;

    public:

        KRG_EXPOSE DriveType                                m_driveType = DriveType::None;
        KRG_EXPOSE float                                    m_maxDistance = 0.05f;
        KRG_EXPOSE float                                    m_tolerance = 0.01f;
        KRG_EXPOSE float                                    m_stiffness = 0.0f;
        KRG_EXPOSE float                                    m_damping = 0.0f;
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_PHYSICS_API RagdollBodySettings : public IRegisteredType
    {
        KRG_SERIALIZE_MEMBERS( m_mass, m_enableCCD, m_enableSelfCollision );
        KRG_REGISTER_TYPE( RagdollBodySettings );

        // Are all the settings valid
        bool IsValid() const;

        // Ensures that all body settings are within the required limits. Returns true if any changes were made!
        bool CorrectSettingsToValidRanges();

    public:

        KRG_REGISTER float                                  m_mass = 1.0f;
        KRG_REGISTER bool                                   m_enableCCD = false;
        KRG_REGISTER bool                                   m_enableSelfCollision = false;
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_PHYSICS_API RagdollJointSettings : public IRegisteredType
    {
        KRG_SERIALIZE_MEMBERS(  m_driveType, m_stiffness, m_damping, m_internalCompliance, m_externalCompliance,
                                m_twistLimitEnabled, m_twistLimitMin, m_twistLimitMax, m_twistLimitContactDistance,
                                m_swingLimitEnabled, m_swingLimitY, m_swingLimitZ, m_tangentialStiffness, m_tangentialDamping, m_swingLimitContactDistance );
        KRG_REGISTER_TYPE( RagdollJointSettings );

        enum DriveType : uint8
        {
            KRG_REGISTER_ENUM

            Kinematic = 0,
            Spring = 1,
            Velocity = 2,
        };

    public:

        // Are all the settings valid
        bool IsValid() const;

        // Ensures that all joint settings are within the required limits. Returns true if any changes were made!
        bool CorrectSettingsToValidRanges();

        // Reset all settings to default
        void Reset() { *this = RagdollJointSettings(); }

    public:

        KRG_REGISTER DriveType                                m_driveType = DriveType::Kinematic;
        KRG_REGISTER bool                                     m_twistLimitEnabled = false;
        KRG_REGISTER bool                                     m_swingLimitEnabled = false;

        // General
        KRG_REGISTER float                                    m_stiffness = 0.0f;
        KRG_REGISTER float                                    m_damping = 0.0f;
        KRG_REGISTER float                                    m_internalCompliance = 0.01f;
        KRG_REGISTER float                                    m_externalCompliance = 0.01f;

        // Twist
        KRG_REGISTER float                                    m_twistLimitMin = -45;
        KRG_REGISTER float                                    m_twistLimitMax = 45;
        KRG_REGISTER float                                    m_twistLimitContactDistance = 3.0f;

        // Swing
        KRG_REGISTER float                                    m_swingLimitY = 45.0f;
        KRG_REGISTER float                                    m_swingLimitZ = 45.0f;
        KRG_REGISTER float                                    m_tangentialStiffness = 0.0f; // Only for swing limit
        KRG_REGISTER float                                    m_tangentialDamping = 0.0f; // Only for swing limit
        KRG_REGISTER float                                    m_swingLimitContactDistance = 3.0f;
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

        constexpr static uint32 const s_maxNumBodies = 64;

        struct KRG_ENGINE_PHYSICS_API BodyDefinition : public IRegisteredType
        {
            KRG_SERIALIZE_MEMBERS( m_boneID, m_offsetTransform, m_radius, m_halfHeight, m_jointTransform );
            KRG_REGISTER_TYPE( BodyDefinition );

            KRG_REGISTER StringID                           m_boneID;
            int32                                           m_parentBodyIdx = InvalidIndex;
            KRG_EXPOSE float                                m_radius = 0.075f;
            KRG_EXPOSE float                                m_halfHeight = 0.025f;
            KRG_EXPOSE Transform                            m_offsetTransform;
            Transform                                       m_initialGlobalTransform;
            Transform                                       m_inverseOffsetTransform;
            KRG_EXPOSE Transform                            m_jointTransform; // Global joint transform
            Transform                                       m_bodyRelativeJointTransform; // The joint transform relative to the current body
            Transform                                       m_parentRelativeJointTransform; // The joint transform relative to the parent body
        };

        //-------------------------------------------------------------------------

        struct KRG_ENGINE_PHYSICS_API Profile : public IRegisteredType
        {
            KRG_SERIALIZE_MEMBERS
            (
                m_ID, m_rootControlBodySettings, m_bodySettings, m_jointSettings, m_materialSettings,
                m_solverPositionIterations, m_solverVelocityIterations, m_maxProjectionIterations, m_internalDriveIterations, m_externalDriveIterations, m_separationTolerance, m_stabilizationThreshold, m_sleepThreshold
            );
            KRG_REGISTER_TYPE( Profile );

            bool IsValid() const;

            // Ensures that all body settings are within the required limits. Returns true if any changes were made!
            bool CorrectSettingsToValidRanges();

            inline bool ShouldBodiesCollides( int32 bodyIdx0, int32 bodyIdx1 ) const
            {
                KRG_ASSERT( IsValid() );
                uint64 const result = m_selfCollisionRules[bodyIdx0] & ( 1ULL << bodyIdx1 );
                return result != 0;
            }

        public:

            KRG_REGISTER StringID                               m_ID = StringID( "Default" );
            KRG_REGISTER RagdollRootControlBodySettings         m_rootControlBodySettings;
            KRG_REGISTER TVector<RagdollBodySettings>           m_bodySettings;
            KRG_REGISTER TVector<RagdollJointSettings>          m_jointSettings;
            KRG_REGISTER TVector<RagdollBodyMaterialSettings>   m_materialSettings;

            // Solver Settings
            KRG_EXPOSE uint32                                   m_solverPositionIterations = 4;
            KRG_EXPOSE uint32                                   m_solverVelocityIterations = 4;
            KRG_EXPOSE uint32                                   m_maxProjectionIterations = 4;
            KRG_EXPOSE uint32                                   m_internalDriveIterations = 4;
            KRG_EXPOSE uint32                                   m_externalDriveIterations = 4;
            KRG_EXPOSE float                                    m_separationTolerance = 0.1f;
            KRG_EXPOSE float                                    m_stabilizationThreshold = 0.01f * Constants::s_speedScale * Constants::s_speedScale;
            KRG_EXPOSE float                                    m_sleepThreshold = 1;

            TVector<uint64>                                     m_selfCollisionRules;
        };

    public:

        virtual bool IsValid() const override;

        void CreateRuntimeData();

        int32 GetNumBodies() const { return (int32) m_bodies.size(); }
        int32 GetNumJoints() const { return (int32) m_bodies.size() - 1; }
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

        KRG_EXPOSE StringID                                     m_ID;
        KRG_EXPOSE TResourcePtr<Animation::Skeleton>            m_pSkeleton;
        KRG_REGISTER TVector<BodyDefinition>                    m_bodies;
        KRG_REGISTER TVector<Profile>                           m_profiles;

        // Runtime Data
        TVector<int32>                                          m_boneToBodyMap;
        TVector<int32>                                          m_bodyToBoneMap;
    };

    //-------------------------------------------------------------------------
    // Ragdoll Instance
    //-------------------------------------------------------------------------
    // By default ragdolls with the same userID will not collide together
    // Ragdoll self-collision is user specified in the definition

    using RagdollPose = TInlineVector<Transform, 40>;

    class KRG_ENGINE_PHYSICS_API Ragdoll
    {
        friend class PhysicsWorldSystem;

        class [[nodiscard]] ScopedWriteLock
        {
        public:

            ScopedWriteLock( Ragdoll* pRagdoll )
                : m_pRagdoll( pRagdoll )
            {
                KRG_ASSERT( pRagdoll != nullptr );
                m_pRagdoll->LockWriteScene();
            }

            ~ScopedWriteLock()
            {
                m_pRagdoll->UnlockWriteScene();
            }

        private:

            Ragdoll*  m_pRagdoll = nullptr;
        };

        class [[nodiscard]] ScopedReadLock
        {
        public:

            ScopedReadLock( Ragdoll const* pRagdoll )
                : m_pRagdoll( pRagdoll )
            {
                KRG_ASSERT( pRagdoll != nullptr );
                m_pRagdoll->LockReadScene();
            }

            ~ScopedReadLock()
            {
                m_pRagdoll->UnlockReadScene();
            }

        private:

            Ragdoll const*  m_pRagdoll = nullptr;
        };

    public:

        // Create a new ragdoll instance. The userID is used to prevent collisions between the ragdolls on the same user
        Ragdoll( physx::PxPhysics* pPhysics, RagdollDefinition const* pDefinition, StringID const profileID = StringID(), uint64 userID = 0 );
        ~Ragdoll();

        bool IsValid() const { return m_pArticulation != nullptr; }

        void AddToScene( physx::PxScene* pScene );
        void RemoveFromScene();

        // Collision Rules
        //-------------------------------------------------------------------------

        inline uintptr_t GetUserID() const { return m_userID; }

        inline bool ShouldBodiesCollides( int32 bodyIdx0, int32 bodyIdx1 ) const
        {
            KRG_ASSERT( IsValid() );
            return m_pProfile->ShouldBodiesCollides( bodyIdx0, bodyIdx1 );
        }

        // Ragdoll API
        //-------------------------------------------------------------------------

        void SwitchProfile( StringID newProfileID );

        void SetGravityEnabled( bool isGravityEnabled );
        void SetPoseFollowingEnabled( bool shouldFollowPose );

        bool IsSleeping() const;
        void PutToSleep();
        void WakeUp();

        // Update / Results
        //-------------------------------------------------------------------------

        void Update( Seconds const deltaTime, Transform const& worldTransform, Animation::Pose* pPose = nullptr, bool initializeBodies = false );
        bool GetPose( Transform const& inverseWorldTransform, Animation::Pose* pPose ) const;
        void GetRagdollPose( RagdollPose& pose ) const;

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void RefreshSettings();
        void RecreateRootControlBody();
        void ResetState();
        void DrawDebug( Drawing::DrawContext& ctx ) const;
        #endif

    private:

        void TryCreateRootControlBody();
        void DestroyRootControlBody();

        void UpdateBodySettings();
        void UpdateSolverSettings();
        void UpdateJointSettings();

        inline void LockWriteScene()
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->lockWrite();
            }
        }

        inline void UnlockWriteScene()
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->unlockWrite();
            }
        }

        inline void LockReadScene() const
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->lockRead();
            }
        }

        inline void UnlockReadScene() const
        {
            if ( auto pScene = m_pArticulation->getScene() )
            {
                pScene->unlockRead();
            }
        }

    private:

        physx::PxPhysics*                                       m_pPhysics = nullptr;
        RagdollDefinition const*                                m_pDefinition = nullptr;
        RagdollDefinition::Profile const*                       m_pProfile = nullptr;
        physx::PxArticulation*                                  m_pArticulation = nullptr;
        uint64                                                  m_userID = 0;
        TVector<physx::PxArticulationLink*>                     m_links;

        physx::PxRigidDynamic*                                  m_pRootControlActor = nullptr;
        physx::PxDistanceJoint*                                 m_pRootTargetJoint = nullptr;

        bool                                                    m_shouldFollowPose = false;
        bool                                                    m_gravityEnabled = true;
        mutable TVector<Transform>                              m_globalBoneTransforms;

        #if KRG_DEVELOPMENT_TOOLS
        TInlineString<100>                                       m_ragdollName;
        #endif
    };
}