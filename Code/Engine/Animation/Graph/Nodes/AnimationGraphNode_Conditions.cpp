#include "AnimationGraphNode_Conditions.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void AndNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<AndNode>( nodePtrs, options );

        pNode->m_conditionNodes.reserve( m_conditionNodeIndices.size() );
        for ( auto conditionNodeIdx : m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, conditionNodeIdx, pNode->m_conditionNodes.emplace_back( nullptr ) );
        }
    }

    void AndNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        BoolValueNode::InitializeInternal( context );

        for ( auto pNode : m_conditionNodes )
        {
            pNode->Initialize( context );
        }

        m_result = false;
    }

    void AndNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        for ( auto pNode : m_conditionNodes )
        {
            pNode->Shutdown( context );
        }

        BoolValueNode::ShutdownInternal( context );
    }

    void AndNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

            m_result = true;
            for ( auto pCondition : m_conditionNodes )
            {
                if ( !pCondition->GetValue<bool>( context ) )
                {
                    m_result = false;
                    break;
                }
            }
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

    void OrNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<OrNode>( nodePtrs, options );

        pNode->m_conditionNodes.reserve( m_conditionNodeIndices.size() );
        for ( auto conditionNodeIdx : m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, conditionNodeIdx, pNode->m_conditionNodes.emplace_back( nullptr ) );
        }
    }

    void OrNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        BoolValueNode::InitializeInternal( context );

        for ( auto pNode : m_conditionNodes )
        {
            pNode->Initialize( context );
        }

        m_result = false;
    }

    void OrNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        for ( auto pNode : m_conditionNodes )
        {
            pNode->Shutdown( context );
        }

        BoolValueNode::ShutdownInternal( context );
    }

    void OrNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

            m_result = false;
            for ( auto pCondition : m_conditionNodes )
            {
                if ( pCondition->GetValue<bool>( context ) )
                {
                    m_result = true;
                    break;
                }
            }
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

    void NotNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<NotNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void NotNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        BoolValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void NotNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        BoolValueNode::ShutdownInternal( context );
    }

    void NotNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            m_result = !( m_pInputValueNode->GetValue<bool>( context ) );
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

    void FloatComparisonNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<FloatComparisonNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
        SetNodePtrFromIndex( nodePtrs, m_comparandValueNodeIdx, pNode->m_pComparandValueNode );
    }

    void FloatComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );

        BoolValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );

        if ( m_pComparandValueNode != nullptr )
        {
            m_pComparandValueNode->Initialize( context );
        }

        m_result = false;
    }

    void FloatComparisonNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );

        if ( m_pComparandValueNode != nullptr )
        {
            m_pComparandValueNode->Shutdown( context );
        }

        m_pInputValueNode->Shutdown( context );
        BoolValueNode::ShutdownInternal( context );
    }

    void FloatComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatComparisonNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

            float const a = m_pInputValueNode->GetValue<float>( context );
            float const b = ( m_pComparandValueNode != nullptr ) ? m_pComparandValueNode->GetValue<float>( context ) : pSettings->m_comparisonValue;

            switch ( pSettings->m_comparison )
            {
                case Comparison::GreaterThanEqual:
                m_result = a >= b;
                break;

                case Comparison::LessThanEqual:
                m_result = a <= b;
                break;

                case Comparison::NearEqual:
                m_result = Math::IsNearEqual( a, b, pSettings->m_epsilon );
                break;

                case Comparison::GreaterThan:
                m_result = a > b;
                break;

                case Comparison::LessThan:
                m_result = a < b;
                break;
            }
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

    void RangeComparisonNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<RangeComparisonNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void RangeComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        BoolValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void RangeComparisonNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        BoolValueNode::ShutdownInternal( context );
    }

    void RangeComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<RangeComparisonNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            float const value = m_pInputValueNode->GetValue<float>( context );
            m_result = pSettings->m_range.ContainsInclusive( value );
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

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

    //-------------------------------------------------------------------------

    void IsTargetSetNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<IsTargetSetNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void IsTargetSetNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        BoolValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void IsTargetSetNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        BoolValueNode::ShutdownInternal( context );
    }

    void IsTargetSetNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<IsTargetSetNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            m_result = m_pInputValueNode->GetValue<Target>( context ).IsTargetSet();
        }

        *( (bool*) pOutValue ) = m_result;
    }
}