#pragma once
#include "Game/Core/GameplayPhysics/GameplayPhysicsState.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AnimDrivenPhysicsState final : public GameplayPhysics::PhysicsState
    {
    public:

        KRG_GAMEPLAY_PHYSICS_STATE_ID( AnimDrivenPhysicsState );

    public:

        virtual Transform TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CapsuleComponent const* pCapsuleComponent, float const deltaTime, Quaternion const& deltaRotation, Vector const& deltaTranslation ) override;

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;
    };
}