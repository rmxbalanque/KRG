#pragma once
#include "System/Core/Math/Transform.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class CapsuleComponent;
    class PhysicsWorldSystem;
}

//-------------------------------------------------------------------------

namespace KRG::GameplayPhysics
{
    class PhysicsState
    {
    public:

        virtual ~PhysicsState() = default;

        // Get the ID for this physics state
        virtual uint32 GetPhysicsStateID() const = 0;

        // Called whenever we switch to this state
        virtual void Activate() {}

        // Called whenever we leave this state
        virtual void Deactivate() {}

        // Try to move the capsule in the physics world, returns the final valid transform for the capsule based on the desired physics collision rules
        virtual Transform TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CapsuleComponent const* pCapsuleComponent, float const deltaTime, Quaternion const& deltaRotation, Vector const& deltaTranslation ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_GAMEPLAY_PHYSICS_STATE_ID( TypeName ) \
constexpr static uint32 const s_physicsStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetPhysicsStateID() const override final { return TypeName::s_physicsStateID; }