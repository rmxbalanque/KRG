#include "AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphInstance::AnimationGraphInstance( AnimationGraph const* pGraph )
        : m_pGraph( pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );

        // Create graph nodes
        //-------------------------------------------------------------------------
        // TODO: Instantiate graph nodes

        m_pAllocatedInstanceMemory = KRG::Alloc( 16 );
    }

    AnimationGraphInstance::~AnimationGraphInstance()
    {
        KRG_ASSERT( m_pRootNode == nullptr );

        for ( auto pNode : m_nodes )
        {
            pNode->~GraphNode();
        }

        KRG::Free( m_pAllocatedInstanceMemory );
    }

    void AnimationGraphInstance::Initialize( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        for ( auto nodeIdx : m_pGraph->m_persistentNodes )
        {
            m_nodes[nodeIdx]->Initialize( context );
        }

        m_pRootNode = static_cast<AnimationNode*>( m_nodes[ m_pGraph->m_rootNodeIdx ] );
    }

    void AnimationGraphInstance::Shutdown( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        for ( auto nodeIdx : m_pGraph->m_persistentNodes )
        {
            m_nodes[nodeIdx]->Shutdown( context );
        }

        m_pRootNode = nullptr;
    }

    //-------------------------------------------------------------------------

    void AnimationGraphInstance::Reset( GraphContext& context )
    {
        m_pRootNode->Shutdown( context );
        m_pRootNode->Initialize( context, SyncTrackTime() );
    }

    UpdateResult AnimationGraphInstance::UpdateGraph( GraphContext& context )
    {
        return m_pRootNode->Update( context );
    }
}