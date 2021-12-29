#include "AIAnimationController.h"
#include "GraphControllers/AIGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    AIAnimationController::AIAnimationController( Animation::AnimationGraphComponent* pGraphComponent, Render::SkeletalMeshComponent* pMeshComponent )
        : Animation::GraphController( pGraphComponent, pMeshComponent )
    {
        m_subGraphControllers.emplace_back( KRG::New<LocomotionGraphController>( pGraphComponent, pMeshComponent ) );
        m_characterStateParam.TryBind( this );
    }
}