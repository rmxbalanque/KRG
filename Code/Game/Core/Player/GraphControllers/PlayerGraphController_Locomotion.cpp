#include "PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    LocomotionGraphController::LocomotionGraphController( Animation::AnimationGraphComponent* pGraphComponent )
        : Animation::GraphController( pGraphComponent )
    {
        BindControlParameter( m_speedParam );
        BindControlParameter( m_headingParam );
        BindControlParameter( m_facingParam );
    }
}