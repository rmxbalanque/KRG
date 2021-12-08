#include "AIGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    LocomotionGraphController::LocomotionGraphController( Animation::AnimationGraphComponent* pGraphComponent, Animation::AnimatedMeshComponent* pMeshComponent )
        : Animation::SubGraphController( pGraphComponent, pMeshComponent )
    {
        m_speedParam.TryBind( this );
        m_headingParam.TryBind( this );
        m_facingParam.TryBind( this );
    }
}