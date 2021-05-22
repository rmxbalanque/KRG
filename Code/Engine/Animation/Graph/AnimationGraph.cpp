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

        size_t const numNodes = pGraph->m_instanceNodeStartOffsets.size();
        KRG_ASSERT( pGraph->m_nodeSettings.size() == numNodes );

        m_pAllocatedInstanceMemory = reinterpret_cast<Byte*>( KRG::Alloc( pGraph->m_instanceRequiredMemory, pGraph->m_instanceRequiredAlignment ) );

        m_nodes.reserve( numNodes );
        for( auto const& nodeOffset : pGraph->m_instanceNodeStartOffsets )
        {
            m_nodes.emplace_back( reinterpret_cast<GraphNode*>( m_pAllocatedInstanceMemory + nodeOffset ) );
        }

        // Instantiate individual nodes
        //-------------------------------------------------------------------------

        for ( auto i = 0; i < numNodes; i++ )
        {
            pGraph->m_nodeSettings[i]->InstantiateNode( m_nodes, GraphNode::Settings::InitOptions::CreateNodeAndSetPointers );
        }
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
        KRG_ASSERT( m_pRootNode->IsInitialized() );
    }

    void AnimationGraphInstance::Shutdown( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        for ( auto nodeIdx : m_pGraph->m_persistentNodes )
        {
            m_nodes[nodeIdx]->Shutdown( context );
        }

        KRG_ASSERT( !m_pRootNode->IsInitialized() );
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