#include "PlayerAnimationController.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    PlayerAnimationController::PlayerAnimationController( Animation::AnimationGraphComponent* pGraphComponent, Animation::AnimatedMeshComponent* pMeshComponent )
        : Animation::GraphController( pGraphComponent, pMeshComponent )
    {
        m_subGraphControllers.emplace_back( KRG::New<LocomotionGraphController>( pGraphComponent, pMeshComponent ) );
        m_characterStateParam.TryBind( this );
    }
}