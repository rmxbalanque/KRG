#pragma once

#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class GameTestSystem : public IEntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( GameTestSystem, RequiresUpdate( UpdateStage::FrameStart ) );

    public:

        GameTestSystem() = default;

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override {}
        virtual void UnregisterComponent( EntityComponent* pComponent ) override {}
        virtual void Update( EntityUpdateContext const& ctx ) override {}
    };
}