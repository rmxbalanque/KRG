#pragma once

#include "Game/Core/_Module/API.h"
#include "Game/Core/AI/AIBehaviorSelector.h"
#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;
}

namespace KRG::Render
{
    class CharacterMeshComponent;
}

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class KRG_GAME_CORE_API AIController final : public EntitySystem
    {
        friend class AIDebugView;

        KRG_REGISTER_ENTITY_SYSTEM( AIController, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

    private:

        virtual void Activate() override;
        virtual void Shutdown() override;

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

    private:

        BehaviorContext                                         m_behaviorContext;
        BehaviorSelector                                        m_behaviorSelector = BehaviorSelector( m_behaviorContext );

        Animation::AnimationGraphComponent*                     m_pAnimGraphComponent = nullptr;
        Render::CharacterMeshComponent*                         m_pCharacterMeshComponent = nullptr;
    };
}