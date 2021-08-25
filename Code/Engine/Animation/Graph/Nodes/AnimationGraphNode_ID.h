#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API IDComparisonNode final: public BoolValueNode
    {
    public:

        enum class Comparison : uint8
        {
            Matches = 0,
            DoesntMatch,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_comparand, m_comparison );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            StringID                                m_comparand;
            Comparison                              m_comparison = Comparison::Matches;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IDValueNode*                                m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API MultipleIDComparisonNode final : public BoolValueNode
    {
    public:

        enum class Comparison : uint8
        {
            Matches = 0,
            DoesntMatch,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_comparison, m_comparisionIDs );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            Comparison                              m_comparison = Comparison::Matches;
            TInlineVector<StringID, 4>              m_comparisionIDs;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IDValueNode*                                m_pInputValueNode = nullptr;
        bool                                        m_result = false;
    };
}