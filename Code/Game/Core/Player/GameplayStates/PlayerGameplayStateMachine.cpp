#include "PlayerGameplayStateMachine.h"
#include "PlayerGameplayState_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    void GameplayStateMachine::Initialize()
    {
        m_pActiveState = KRG::New<LocomotionState>();
    }

    void GameplayStateMachine::Shutdown()
    {
        KRG::Delete( m_pActiveState);
    }

    void GameplayStateMachine::Update( GameplayStateContext const& context )
    {
        m_pActiveState->Update( context );
    }
}