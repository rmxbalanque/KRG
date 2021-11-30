#pragma once

#include "Game/Core/_Module/API.h"
#include "Game/Core/Player/GameplayStates/PlayerGameplayStateMachine.h"
#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SpatialEntityComponent;
    class OrbitCameraComponent;
    namespace Physics { class CapsuleComponent; }
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerGameplayPhysicsComponent;
    class PlayerComponent;
    class PlayerGameplayState;

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API PlayerController final : public EntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( PlayerController, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        PlayerController();
        virtual ~PlayerController();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

    private:

        GameplayStateMachine                                m_stateMachine;
        GameplayStateContext                                m_stateMachineContext;
    };
}