#pragma once

#include "Game/Core/_Module/API.h"
#include "Game/Core/Player/Actions/PlayerAction.h"
#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class KRG_GAME_CORE_API PlayerController final : public EntitySystem
    {
        friend class PlayerDebugView;

        KRG_REGISTER_ENTITY_SYSTEM( PlayerController, RequiresUpdate( UpdateStage::PrePhysics ) );

        // List of all the state machine states
        //-------------------------------------------------------------------------

        enum ActionID : int8
        {
            InvalidAction = -1,
            Locomotion = 0,
            Jump = 1,

            NumActions,
            DefaultAction = Locomotion,
        };

        // Transitions in the state machine
        //-------------------------------------------------------------------------

        struct Transition
        {
            enum class Availability : uint8
            {
                Always,
                OnlyOnCompleted
            };

            Transition( ActionID stateID, Availability availability = Availability::Always )
                : m_targetStateID( stateID )
                , m_availability( availability )
            {}

            inline bool IsAvailable( Action::Status stateUpdateResult ) const
            {
                return ( m_availability == Availability::Always ) || ( stateUpdateResult == Action::Status::Completed );
            }

            ActionID            m_targetStateID;
            Availability        m_availability;
        };

    public:

        virtual ~PlayerController();

    private:

        virtual void Activate() override;
        virtual void Deactivate() override;

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

        // State Machine
        //-------------------------------------------------------------------------

        void InitializeStateMachine();
        void ShutdownStateMachine();
        void UpdateStateMachine();

    private:

        ActionContext                                           m_actionContext;
        ActionID                                                m_activeBaseActionID = InvalidAction;

        TArray<Action*, NumActions>                             m_baseActions;
        TArray<TInlineVector<Transition, 6>, NumActions>        m_actionTransitions;
        TInlineVector<Transition, 6>                            m_highPriorityGlobalTransitions;
        TInlineVector<Transition, 6>                            m_lowPriorityGlobalTransitions;

        TInlineVector<OverlayAction*, 5>                        m_overlayActions;
    };
}