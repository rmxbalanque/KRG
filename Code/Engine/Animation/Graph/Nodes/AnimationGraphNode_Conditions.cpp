#include "AnimationGraphNode_Conditions.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void AndNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pSettings, nodePtrs, dataSet );

        auto pNodeSettings = static_cast<OrNode::Settings const*>( pSettings );
        m_conditionNodes.reserve( pNodeSettings->m_conditionNodeIndices.size() );
        for ( auto conditionNodeIdx : pNodeSettings->m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, conditionNodeIdx, m_conditionNodes.emplace_back( nullptr ) );
        }
    }

    void AndNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        ValueNodeBool::InitializeInternal( context );

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

        ValueNodeBool::ShutdownInternal( context );
    }

    void AndNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

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

    void OrNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pSettings, nodePtrs, dataSet );

        auto pNodeSettings = static_cast<OrNode::Settings const*>( pSettings );
        m_conditionNodes.reserve( pNodeSettings->m_conditionNodeIndices.size() );
        for ( auto conditionNodeIdx : pNodeSettings->m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, conditionNodeIdx, m_conditionNodes.emplace_back( nullptr ) );
        }
    }

    void OrNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        ValueNodeBool::InitializeInternal( context );

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

        ValueNodeBool::ShutdownInternal( context );
    }

    void OrNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

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

    void NotNode::OnConstruct( GraphNode::Settings const* pInSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pInSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<NotNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void NotNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeBool::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void NotNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeBool::ShutdownInternal( context );
    }

    void NotNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
            m_result = !( m_pInputValueNode->GetValue<bool>( context ) );
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

    void FloatComparisonNode::OnConstruct( GraphNode::Settings const* pInSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pInSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatComparisonNode>()->m_inputValueNodeIdx, m_pInputValueNode );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FloatComparisonNode>()->m_comparandValueNodeIdx, m_pComparandValueNode );
    }

    void FloatComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );

        ValueNodeBool::InitializeInternal( context );
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
        ValueNodeBool::ShutdownInternal( context );
    }

    void FloatComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<FloatComparisonNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

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

    void RangeComparisonNode::OnConstruct( GraphNode::Settings const* pInSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pInSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<RangeComparisonNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void RangeComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeBool::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void RangeComparisonNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeBool::ShutdownInternal( context );
    }

    void RangeComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<RangeComparisonNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
            float const value = m_pInputValueNode->GetValue<float>( context );
            m_result = pSettings->m_range.ContainsInclusive( value );
        }

        *( (bool*) pOutValue ) = m_result;
    }

    //-------------------------------------------------------------------------

    void IDComparisonNode::OnConstruct( GraphNode::Settings const* pInSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pInSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<IDComparisonNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void IDComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeBool::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void IDComparisonNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeBool::ShutdownInternal( context );
    }

    void IDComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<IDComparisonNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );

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

    void MultipleIDComparisonNode::OnConstruct( GraphNode::Settings const* pInSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pInSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<MultipleIDComparisonNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void MultipleIDComparisonNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeBool::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void MultipleIDComparisonNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeBool::ShutdownInternal( context );
    }

    void MultipleIDComparisonNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<MultipleIDComparisonNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
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

    void IsTargetSetNode::OnConstruct( GraphNode::Settings const* pInSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        ValueNodeBool::OnConstruct( pInSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<IsTargetSetNode>()->m_inputValueNodeIdx, m_pInputValueNode );
    }

    void IsTargetSetNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        ValueNodeBool::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
        m_result = false;
    }

    void IsTargetSetNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        ValueNodeBool::ShutdownInternal( context );
    }

    void IsTargetSetNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        KRG_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<IsTargetSetNode>();

        if ( !IsUpToDate( context ) )
        {
            MarkAsUpdated( context );
            m_result = m_pInputValueNode->GetValue<Target>( context ).IsTargetSet();
        }

        *( (bool*) pOutValue ) = m_result;
    }
}