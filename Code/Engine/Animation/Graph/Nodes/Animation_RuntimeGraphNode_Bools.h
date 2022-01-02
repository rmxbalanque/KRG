#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class KRG_ENGINE_ANIMATION_API AndNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_conditionNodeIndices );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            TInlineVector<GraphNodeIndex, 4>            m_conditionNodeIndices;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TInlineVector<BoolValueNode*, 4>            m_conditionNodes;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API OrNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_conditionNodeIndices );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            TInlineVector<GraphNodeIndex, 4>            m_conditionNodeIndices;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TInlineVector<BoolValueNode*, 4>            m_conditionNodes;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API NotNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex                               m_inputValueNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoolValueNode*                              m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };
}