#pragma once

#include "PlayerAction.h"

//-------------------------------------------------------------------------
namespace KRG::Physics
{
    class PhysicsWorldSystem;
    class CharacterComponent;
}

namespace KRG::Player
{
    class FallingAction final : public Action
    {
    public:

        KRG_PLAYER_ACTION_ID( FallingAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override;
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;

    private:
        bool ProbeForGround( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent const* pCapsuleComponent, Vector const& startPosition );
    };
}