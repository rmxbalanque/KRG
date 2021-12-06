#include "AnimationGraphController.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void GraphControllerRegistry::CreateControllers( AnimationGraphComponent* pGraphComponent )
    {
        KRG_ASSERT( m_controllers.empty() );
        CreateControllersInternal( pGraphComponent );
        KRG_ASSERT( !m_controllers.empty() );
    }

    void GraphControllerRegistry::DestroyControllers()
    {
        for ( auto pController : m_controllers )
        {
            KRG::Delete( pController );
        }
        m_controllers.clear();
    }
}