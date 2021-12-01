#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Game/Core/Player/GameplayStates/PlayerGameplayState.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class KRG_GAME_CORE_API PlayerController final : public EntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( PlayerController, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

        // List of all the state machine states
        //-------------------------------------------------------------------------

        enum GameplayStateID : int8
        {
            InvalidState = -1,
            LocomotionState = 0,
            JumpState = 1,

            NumGameplayStates,
            DefaultState = LocomotionState,
        };

        // Transitions in the state machine
        //-------------------------------------------------------------------------

        struct StateTransition
        {
            enum class Availability : uint8
            {
                Always,
                OnlyOnCompleted
            };

            StateTransition( GameplayStateID stateID, Availability availability = Availability::Always )
                : m_targetStateID( stateID )
                , m_availability( availability )
            {}

            inline bool IsAvailable( GameplayState::Status stateUpdateResult ) const
            {
                return ( m_availability == Availability::Always ) || ( stateUpdateResult == GameplayState::Status::Completed );
            }

            GameplayStateID     m_targetStateID;
            Availability        m_availability;
        };

    public:

        PlayerController();
        virtual ~PlayerController();

    private:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

        // State Machine
        //-------------------------------------------------------------------------

        void CreateStateMachineStates();
        void DestroyStateMachineStates();
        void UpdateStateMachine();

    private:

        GameplayStateContext                                                m_stateMachineContext;
        GameplayStateID                                                     m_activeStateID = InvalidState;
        TArray<GameplayState*, NumGameplayStates>                           m_registeredStates;
        TInlineVector<StateTransition, 6>                                   m_globalPreStateTransitions;
        TArray<TInlineVector<StateTransition, 6>, NumGameplayStates>        m_perStateTransitions;
        TInlineVector<StateTransition, 6>                                   m_globalPostStateTransitions;
    };
}