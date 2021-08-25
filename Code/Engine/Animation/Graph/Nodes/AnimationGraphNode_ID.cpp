#include "AnimationGraphNode_ID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void IDComparisonNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<IDComparisonNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void IDComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        BoolValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void IDComparisonNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        BoolValueNode::ShutdownInternal( context );
    }

    void IDComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<IDComparisonNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

            switch ( pSettings->m_comparison )
            {
                case Comparison::Matches:
                {
                    m_result = m_pInputValueNode->GetValue<StringID>( context ) == pSettings->m_comparand;
                }
                break;

                case Comparison::DoesntMatch:
                {
                    m_result = m_pInputValueNode->GetValue<StringID>( context ) != pSettings->m_comparand;
                }
                break;
            }
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

    void MultipleIDComparisonNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<MultipleIDComparisonNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void MultipleIDComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        BoolValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void MultipleIDComparisonNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        BoolValueNode::ShutdownInternal( context );
    }

    void MultipleIDComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<MultipleIDComparisonNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            StringID const inputID = m_pInputValueNode->GetValue<StringID>( context );

            switch ( pSettings->m_comparison )
            {
                case Comparison::Matches:
                {
                    m_result = VectorContains( pSettings->m_comparisionIDs, inputID );
                }
                break;

                case Comparison::DoesntMatch:
                {
                    m_result = !VectorContains( pSettings->m_comparisionIDs, inputID );
                }
                break;
            }
        }

        *( (bool*) pOutValue ) = m_result;
    }
}