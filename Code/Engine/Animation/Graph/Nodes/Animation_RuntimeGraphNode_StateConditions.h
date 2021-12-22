#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class StateNode;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API StateCompletedConditionNode : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_sourceStateNodeIdx, m_transitionDurationOverrideNodeIdx, m_transitionDuration );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                       m_sourceStateNodeIdx = InvalidIndex;
            NodeIndex                       m_transitionDurationOverrideNodeIdx = InvalidIndex;
            float                           m_transitionDuration = 0.0f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        StateNode*                          m_pSourceStateNode = nullptr;
        FloatValueNode*                     m_pDurationOverrideNode = nullptr;
        bool                                m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API TimeConditionNode : public BoolValueNode
    {
    public:

        enum class ComparisonType : uint8
        {
            KRG_REGISTER_ENUM

            PercentageThroughState,
            PercentageThroughSyncEvent,
            LoopCount,
            ElapsedTime,
        };

        enum class Operator : uint8
        {
            KRG_REGISTER_ENUM

            LessThan = 0,
            LessThanEqual,
            GreaterThan,
            GreaterThanEqual,
        };

        struct KRG_ENGINE_ANIMATION_API Settings : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_sourceStateNodeIdx, m_comparand, m_inputValueNodeIdx, m_type, m_operator );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                               m_sourceStateNodeIdx = InvalidIndex;
            NodeIndex                               m_inputValueNodeIdx = InvalidIndex;
            float                                   m_comparand = 0.0f;
            ComparisonType                          m_type = ComparisonType::ElapsedTime;
            Operator                                m_operator = Operator::LessThan;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        StateNode*                                  m_pSourceStateNode = nullptr;
        FloatValueNode*                             m_pInputValueNode = nullptr;
        float                                       m_requiredElapsedTime = 0.0f;
        bool                                        m_result = false;
    };
}