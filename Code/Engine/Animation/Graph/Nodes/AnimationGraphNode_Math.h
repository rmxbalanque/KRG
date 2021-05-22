#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "System/Core/Math/Easing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API FloatRemapNode final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_inputValueNodeIdx, m_inputRange, m_outputRange );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            FloatRange                  m_inputRange = FloatRange( 0.0f );
            FloatRange                  m_outputRange = FloatRange( 0.0f );
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatClampNode final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_inputValueNodeIdx, m_clampRange );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            FloatRange                  m_clampRange = FloatRange( 0 );
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatAbsNode final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_inputValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API IDToFloatNode final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_inputValueNodeIdx, m_defaultValue, m_IDs, m_values );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            float                       m_defaultValue = 0.0f;
            TInlineVector<StringID, 5>  m_IDs;
            TInlineVector<float, 5>     m_values;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeID*                    m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VectorInfoNode final : public ValueNodeFloat
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

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_inputValueNodeIdx, m_desiredInfo );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            Info                        m_desiredInfo = Info::X;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeVector*                m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatBlendNode final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_inputValueNodeIdx, m_blendType, m_blendTime );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            Math::Easing::Type          m_blendType = Math::Easing::Type::Linear;
            float                       m_blendTime = 1.0f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pInputValueNode = nullptr;
        FloatRange                      m_blendRange = FloatRange( 0.0f );
        float                           m_currentValue = 0.0f;
        float                           m_currentBlendTime = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatMathNode final : public ValueNodeFloat
    {
    public:

        enum class Operator : uint8
        {
            Add,
            Sub,
            Mul,
            Div,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_valueNodeIdxA, m_valueNodeIdxB, m_returnAbsoluteResult, m_operator, m_valueB );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_valueNodeIdxA = InvalidIndex;
            NodeIndex                   m_valueNodeIdxB = InvalidIndex; // Optional
            bool                        m_returnAbsoluteResult = false;
            Operator                    m_operator = Operator::Add;
            float                       m_valueB = 0.0f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat*                 m_pValueNodeA = nullptr;
        ValueNodeFloat*                 m_pValueNodeB = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API TargetInfoNode final : public ValueNodeFloat
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

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeTarget::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeTarget::Settings, m_inputTargetNodeIdx, m_expectedCoordinateSpace, m_infoType );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                   m_inputTargetNodeIdx = InvalidIndex;
            CoordinateSpace             m_expectedCoordinateSpace = CoordinateSpace::Character;
            Info                        m_infoType = Info::Distance;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeVector*                m_pTargetNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatSwitchNode final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_switchValueNodeIdx, m_trueValueNodeIdx, m_falseValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                    m_switchValueNodeIdx = InvalidIndex;
            NodeIndex                    m_trueValueNodeIdx = InvalidIndex;
            NodeIndex                    m_falseValueNodeIdx = InvalidIndex;
        };

    private:

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

    class KRG_ENGINE_ANIMATION_API TargetOffsetNode final : public ValueNodeTarget
    {

    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeTarget::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeTarget::Settings, m_inputValueNodeIdx, m_isBoneSpaceOffset, m_rotationOffset, m_translationOffset );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex               m_inputValueNodeIdx = InvalidIndex;
            bool                    m_isBoneSpaceOffset = true;
            Quaternion              m_rotationOffset = Quaternion::Identity;
            Vector                  m_translationOffset = Vector::Zero;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeTarget*            m_pInputValueNode = nullptr;
        Target                      m_value;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VectorNegateNode final : public ValueNodeVector
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeVector::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeVector::Settings, m_inputValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex               m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeVector*            m_pInputValueNode = nullptr;
        Vector                      m_value = Vector::Zero;
    };
}