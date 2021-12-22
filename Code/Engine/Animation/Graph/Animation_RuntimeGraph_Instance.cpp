#include "Animation_RuntimeGraph_Instance.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphInstance::GraphInstance( AnimationGraphVariation const* pGraphVariation )
        : m_pGraphVariation( pGraphVariation )
    {
        KRG_ASSERT( pGraphVariation != nullptr );

        // Create graph nodes
        //-------------------------------------------------------------------------

        auto pGraphDef = m_pGraphVariation->m_pGraphDefinition.GetPtr();
        size_t const numNodes = pGraphDef->m_instanceNodeStartOffsets.size();
        KRG_ASSERT( pGraphDef->m_nodeSettings.size() == numNodes );

        m_pAllocatedInstanceMemory = reinterpret_cast<Byte*>( KRG::Alloc( pGraphDef->m_instanceRequiredMemory, pGraphDef->m_instanceRequiredAlignment ) );

        m_nodes.reserve( numNodes );
        for( auto const& nodeOffset : pGraphDef->m_instanceNodeStartOffsets )
        {
            m_nodes.emplace_back( reinterpret_cast<GraphNode*>( m_pAllocatedInstanceMemory + nodeOffset ) );
        }

        // Instantiate individual nodes
        //-------------------------------------------------------------------------

        for ( auto i = 0; i < numNodes; i++ )
        {
            pGraphDef->m_nodeSettings[i]->InstantiateNode( m_nodes, m_pGraphVariation->m_pDataSet.GetPtr(), GraphNode::Settings::InitOptions::CreateNodeAndSetPointers );
        }
    }

    GraphInstance::~GraphInstance()
    {
        KRG_ASSERT( m_pRootNode == nullptr );

        for ( auto pNode : m_nodes )
        {
            pNode->~GraphNode();
        }

        KRG::Free( m_pAllocatedInstanceMemory );
    }

    void GraphInstance::Initialize( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        auto pGraphDef = m_pGraphVariation->m_pGraphDefinition.GetPtr();
        for ( auto nodeIdx : pGraphDef->m_persistentNodeIndices )
        {
            m_nodes[nodeIdx]->Initialize( context );
        }

        m_pRootNode = static_cast<PoseNode*>( m_nodes[pGraphDef->m_rootNodeIdx ] );
        KRG_ASSERT( m_pRootNode->IsInitialized() );
    }

    void GraphInstance::Shutdown( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        auto pGraphDef = m_pGraphVariation->m_pGraphDefinition.GetPtr();
        for ( auto nodeIdx : pGraphDef->m_persistentNodeIndices )
        {
            m_nodes[nodeIdx]->Shutdown( context );
        }

        KRG_ASSERT( !m_pRootNode->IsInitialized() );
        m_pRootNode = nullptr;
    }

    //-------------------------------------------------------------------------

    void GraphInstance::Reset( GraphContext& context )
    {
        m_pRootNode->Shutdown( context );
        m_pRootNode->Initialize( context, SyncTrackTime() );
    }

    PoseNodeResult GraphInstance::UpdateGraph( GraphContext& context )
    {
        return m_pRootNode->Update( context );
    }
}