#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Game/Core/Player/Actions/PlayerAction.h"

// HACK
#include "bfxMover.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class KRG_GAME_CORE_API AIController final : public EntitySystem
    {
        friend class AIDebugView;

        KRG_REGISTER_ENTITY_SYSTEM( AIController, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        virtual ~AIController();

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

    private:

        // HACK
        bfx::MoverTune  m_tune;
        bfx::Mover*     m_pMover = nullptr;
    };
}