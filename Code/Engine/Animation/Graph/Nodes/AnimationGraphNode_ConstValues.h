#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API ConstBoolNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            bool m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstIDNode final : public IDValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IDValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( IDValueNode::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            StringID m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstIntNode final : public IntValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IntValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( IntValueNode::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            int32 m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstFloatNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            float m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstVectorNode final : public VectorValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public VectorValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( VectorValueNode::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            Vector m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstTargetNode final : public TargetValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public TargetValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( TargetValueNode::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            Target m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };
}