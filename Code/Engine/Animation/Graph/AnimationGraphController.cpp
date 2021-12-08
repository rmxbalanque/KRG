#include "AnimationGraphController.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    namespace Internal
    {
        GraphControllerBase::GraphControllerBase( AnimationGraphComponent* pGraphComponent, AnimatedMeshComponent* pMeshComponent )
            : m_pGraphComponent( pGraphComponent )
            , m_pAnimatedMeshComponent( pMeshComponent )
        {
            KRG_ASSERT( m_pGraphComponent != nullptr && pMeshComponent != nullptr );
        }
    }

    //-------------------------------------------------------------------------

    GraphController::~GraphController()
    {
        for ( auto pController : m_subGraphControllers )
        {
            KRG::Delete( pController );
        }
        m_subGraphControllers.clear();
    }

    void GraphController::PreGraphUpdate( Seconds deltaTime )
    {
        for ( auto pController : m_subGraphControllers )
        {
            pController->PreGraphUpdate( deltaTime );
        }
    }

    void GraphController::PostGraphUpdate( Seconds deltaTime )
    {
        for ( auto pController : m_subGraphControllers )
        {
            pController->PostGraphUpdate( deltaTime );
        }
    }
}