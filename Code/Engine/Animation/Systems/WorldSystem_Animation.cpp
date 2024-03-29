#include "WorldSystem_Animation.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void AnimationWorldSystem::ShutdownSystem()
    {
        KRG_ASSERT( m_graphComponents.empty() );
    }

    void AnimationWorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pGraphComponent = TryCast<AnimationGraphComponent>( pComponent ) )
        {
            m_graphComponents.Add( pGraphComponent );
        }
    }

    void AnimationWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pGraphComponent = TryCast<AnimationGraphComponent>( pComponent ) )
        {
            m_graphComponents.Remove( pGraphComponent->GetID() );
        }
    }
}