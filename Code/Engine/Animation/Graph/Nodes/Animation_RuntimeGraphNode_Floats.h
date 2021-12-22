#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"
#include "Engine/Core/Math/Easing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API FloatRemapNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_inputValueNodeIdx, m_inputRange, m_outputRange );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            FloatRange                  m_inputRange = FloatRange( 0.0f );
            FloatRange                  m_outputRange = FloatRange( 0.0f );
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatClampNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_inputValueNodeIdx, m_clampRange );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            FloatRange                  m_clampRange = FloatRange( 0 );
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatAbsNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_inputValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatEaseNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_inputValueNodeIdx, m_easingType, m_easeTime );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            Math::Easing::Type          m_easingType = Math::Easing::Type::Linear;
            float                       m_easeTime = 1.0f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                 m_pInputValueNode = nullptr;
        FloatRange                      m_easeRange = FloatRange( 0.0f );
        float                           m_currentValue = 0.0f;
        float                           m_currentEaseTime = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatMathNode final : public FloatValueNode
    {
    public:

        enum class Operator : uint8
        {
            KRG_REGISTER_ENUM

            Add,
            Sub,
            Mul,
            Div,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_inputValueNodeIdxA, m_inputValueNodeIdxB, m_returnAbsoluteResult, m_operator, m_valueB );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdxA = InvalidIndex;
            NodeIndex                   m_inputValueNodeIdxB = InvalidIndex; // Optional
            bool                        m_returnAbsoluteResult = false;
            Operator                    m_operator = Operator::Add;
            float                       m_valueB = 0.0f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                 m_pValueNodeA = nullptr;
        FloatValueNode*                 m_pValueNodeB = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatComparisonNode final : public BoolValueNode
    {
    public:

        enum class Comparison : uint8
        {
            KRG_REGISTER_ENUM

            GreaterThanEqual = 0,
            LessThanEqual,
            NearEqual,
            GreaterThan,
            LessThan,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_comparandValueNodeIdx, m_comparison, m_epsilon, m_comparisonValue );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            NodeIndex                               m_comparandValueNodeIdx = InvalidIndex;
            Comparison                              m_comparison = Comparison::GreaterThanEqual;
            float                                   m_epsilon = 0.0f;
            float                                   m_comparisonValue = 0.0f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                             m_pInputValueNode = nullptr;
        FloatValueNode*                             m_pComparandValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatRangeComparisonNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_range, m_isInclusiveCheck );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            FloatRange                              m_range;
            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            bool                                    m_isInclusiveCheck = true;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*                             m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatSwitchNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_switchValueNodeIdx, m_trueValueNodeIdx, m_falseValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                    m_switchValueNodeIdx = InvalidIndex;
            NodeIndex                    m_trueValueNodeIdx = InvalidIndex;
            NodeIndex                    m_falseValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoolValueNode*                  m_pSwitchValueNode = nullptr;
        FloatValueNode*                 m_pTrueValueNode = nullptr;
        FloatValueNode*                 m_pFalseValueNode = nullptr;
        float                           m_value = 0.0f;
    };
}