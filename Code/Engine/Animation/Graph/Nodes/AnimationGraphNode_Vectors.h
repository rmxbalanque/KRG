#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Core/Math/Easing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API VectorInfoNode final : public FloatValueNode
    {
    public:

        enum class Info : uint8
        {
            KRG_REGISTER_ENUM

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

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_inputValueNodeIdx, m_desiredInfo );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
            Info                        m_desiredInfo = Info::X;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        VectorValueNode*                m_pInputValueNode = nullptr;
        float                           m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VectorNegateNode final : public VectorValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public VectorValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( VectorValueNode::Settings, m_inputValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                   m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        VectorValueNode*                m_pInputValueNode = nullptr;
        Vector                          m_value = Vector::Zero;
    };
}