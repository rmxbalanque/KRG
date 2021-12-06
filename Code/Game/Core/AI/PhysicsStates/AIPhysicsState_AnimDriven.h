#pragma once
#include "Engine/Physics/PhysicsStateController.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AnimDrivenPhysicsState final : public Physics::PhysicsState
    {
    public:

        KRG_PHYSICS_STATE_ID( AnimDrivenPhysicsState );

    public:

        virtual bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, float const deltaTime, Transform const& deltaTransform ) override;

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;
    };
}