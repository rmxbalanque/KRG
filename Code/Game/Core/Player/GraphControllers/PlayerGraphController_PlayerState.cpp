#include "PlayerGraphController_PlayerState.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    PlayerStateGraphController::PlayerStateGraphController( Animation::AnimationGraphComponent* pGraphComponent )
        : Animation::GraphController( pGraphComponent )
    {
        BindControlParameter( m_playerStateParam );
    }
}