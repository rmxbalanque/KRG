#pragma once
#include "PlayerGameplayState.h"

//------------------------------------------------------------------------- 

namespace KRG::Player
{
    class GameplayStateMachine
    {
    public:

        void Initialize();
        void Shutdown();
        void Update( GameplayStateContext const& context );

    private:

        GameplayState* m_pActiveState = nullptr;
    };
}