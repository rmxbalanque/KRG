#pragma once
#include "Engine/Animation/Graph/AnimGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AndNode : public ValueNodeBool
    {

    public:

        struct Settings : public ValueNodeBool::Settings
        {
            TInlineVector<NodeIndex, 4>            m_conditionNodeIndices;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TInlineVector<ValueNodeBool*, 4>            m_conditionNodes;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class OrNode : public ValueNodeBool
    {
    public:

        struct Settings : public ValueNodeBool::Settings
        {
            TInlineVector<NodeIndex, 4>            m_conditionNodeIndices;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TInlineVector<ValueNodeBool*, 4>            m_conditionNodes;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class NotNode : public ValueNodeBool
    {
    public:

        struct Settings : public ValueNodeBool::Settings
        {
            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeBool*                              m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class FloatComparisonNode : public ValueNodeBool
    {
    public:

        enum class Comparison : uint8
        {
            GreaterThanEqual = 0,
            LessThanEqual,
            NearEqual,
            GreaterThan,
            LessThan,
        };

        struct Settings : public ValueNodeBool::Settings
        {
            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            NodeIndex                               m_comparandValueNodeIdx = InvalidIndex;
            Comparison                              m_comparison = Comparison::GreaterThanEqual;
            float                                   m_epsilon = 0.0f;
            float                                   m_comparisonValue = 0.0f;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                             m_pInputValueNode = nullptr;
        ValueNodeFloat*                             m_pComparandValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class RangeComparisonNode : public ValueNodeBool
    {
    public:

        struct Settings : public ValueNodeBool::Settings
        {
            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            TRange<float>                           m_range;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                             m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class IDComparisonNode : public ValueNodeBool
    {
    public:

        enum class Comparison : uint8
        {
            Matches = 0,
            DoesntMatch,
        };

        struct Settings : public ValueNodeBool::Settings
        {
            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            StringID                                m_comparand;
            Comparison                              m_comparison = Comparison::Matches;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeID*                                m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class MultipleIDComparisonNode : public ValueNodeBool
    {
    public:

        enum class Comparison : uint8
        {
            Matches = 0,
            DoesntMatch,
        };

        struct Settings : public ValueNodeBool::Settings
        {
            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            Comparison                              m_comparison = Comparison::Matches;
            TInlineVector<StringID, 4>              m_comparisionIDs;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeID*                                m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class IsTargetSetNode : public ValueNodeBool
    {
    public:

        struct Settings : public ValueNodeBool::Settings
        {
            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeTarget*                m_pInputValueNode = nullptr;
        bool                            m_result = false;
    };
}