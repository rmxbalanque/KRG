#pragma once
#include "Engine/Physics/PhysicsState.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class GroundPhysicsState final : public Physics::PhysicsState
    {
    public:

        KRG_PHYSICS_STATE_ID( GroundPhysicsState );

    public:

        virtual bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation ) override;

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;
    };
}