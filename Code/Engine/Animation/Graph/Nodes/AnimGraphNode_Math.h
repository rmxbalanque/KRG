#pragma once
#include "Engine/Animation/Graph/AnimGraphNode.h"
#include "System/Core/Math/Easing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class FloatRemapNode : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            TRange<float>               m_inputRange = TRange<float>( 0.0f );
            TRange<float>               m_outputRange = TRange<float>( 0.0f );
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class FloatClampNode : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            float                       m_minValue = 0;
            float                       m_maxValue = 0;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class FloatAbsNode : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class IDToFloatNode : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            float                       m_defaultValue = 0.0f;
            TInlineVector<StringID, 5>  m_IDs;
            TInlineVector<float, 5>     m_values;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeID*                    m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class VectorInfoNode : public ValueNodeFloat
    {
    public:

        enum class Info : uint8
        {
            X,
            Y,
            Z,
            W,
            Length,
            AngleHorizontal,
            AngleVertical,
            SizeHorizontal,
            SizeVertical,
        };

    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            Info                        m_desiredInfo = Info::X;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeVector*                m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class FloatBlendNode : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            Math::Easing::Type          m_blendType = Math::Easing::Type::Linear;
            float                       m_blendTime = 1.0f;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        TRange<float>                   m_blendRange = TRange<float>( 0.0f );
        float                           m_currentValue = 0.0f;
        float                           m_currentBlendTime = 0.0f;
    };

    //-------------------------------------------------------------------------

    class FloatMathNode : public ValueNodeFloat
    {
    public:

        enum class Operator : uint8
        {
            Add,
            Sub,
            Mul,
            Div,
        };

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                   m_valueNodeIdxA = InvalidIndex;
            NodeIndex                   m_valueNodeIdxB = InvalidIndex; // Optional
            bool                        m_returnAbsoluteResult = false;
            Operator                    m_operator = Operator::Add;
            float                       m_valueB = 0.0f;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pValueNodeA = nullptr;
        ValueNodeFloat*                 m_pValueNodeB = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class TargetInfoNode : public ValueNodeFloat
    {
    public:

        enum class Info
        {
            AngleHorizontal,
            AngleVertical,

            Distance,
            DistanceHorizontalOnly,
            DistanceVerticalOnly,

            DeltaOrientationX,
            DeltaOrientationY,
            DeltaOrientationZ,
        };

        struct Settings : public ValueNodeTarget::Settings
        {
            NodeIndex                   m_inputTargetNodeIdx = InvalidIndex;
            CoordinateSpace             m_expectedCoordinateSpace = CoordinateSpace::Character;
            Info                        m_infoType = Info::Distance;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue );

    private:

        ValueNodeVector*                m_pTargetNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class FloatSwitchNode : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            NodeIndex                    m_switchValueNodeIdx = InvalidIndex;
            NodeIndex                    m_trueValueNodeIdx = InvalidIndex;
            NodeIndex                    m_falseValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeBool*                  m_pSwitchValueNode = nullptr;
        ValueNodeFloat*                 m_pTrueValueNode = nullptr;
        ValueNodeFloat*                 m_pFalseValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class TargetOffsetNode : public ValueNodeTarget
    {

    public:

        struct Settings : public ValueNodeTarget::Settings
        {
            NodeIndex               m_inputValueNodeIdx = InvalidIndex;
            bool                    m_isBoneSpaceOffset = true;
            Quaternion              m_rotationOffset = Quaternion::Identity;
            Vector                  m_translationOffset = Vector::Zero;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue );

    private:

        ValueNodeTarget*            m_pInputValueNode = nullptr;
        Target                      m_value;
    };

    //-------------------------------------------------------------------------

    class VectorNegateNode : public ValueNodeVector
    {
    public:

        struct Settings : public ValueNodeVector::Settings
        {
            NodeIndex               m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue );

    private:

        ValueNodeVector*            m_pInputValueNode = nullptr;
        Vector                      m_value = Vector::Zero;
    };
}