#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/Events/AnimationEvent_Foot.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class StateNode;

    //-------------------------------------------------------------------------

    enum class EventSearchMode : uint8
    {
        KRG_REGISTER_ENUM

        SearchAll = 0,
        OnlySearchStateEvents,
        OnlySearchAnimEvents,
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API GenericEventConditionNode : public BoolValueNode
    {
    public:

        enum class Operator : uint8
        {
            KRG_REGISTER_ENUM

            Or = 0,
            And,
        };

    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_sourceStateNodeIdx, m_eventIDs, m_operator, m_searchMode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                                   m_sourceStateNodeIdx = InvalidIndex;
            Operator                                    m_operator = Operator::Or;
            EventSearchMode                             m_searchMode = EventSearchMode::SearchAll;
            TInlineVector<StringID, 5>                  m_eventIDs;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

        bool AreSearchTagsFound( GraphContext& context ) const;

    private:

        StateNode const*                                m_pSourceStateNode = nullptr;
        bool                                            m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API GenericEventPercentageThroughNode : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_sourceStateNodeIdx, m_eventID, m_searchMode, m_preferHighestPercentageThrough );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                                   m_sourceStateNodeIdx = InvalidIndex;
            EventSearchMode                             m_searchMode = EventSearchMode::SearchAll;
            bool                                        m_preferHighestPercentageThrough = false;
            StringID                                    m_eventID;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        StateNode const*                                m_pSourceStateNode = nullptr;
        float                                           m_result = -1.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FootEventConditionNode : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_sourceStateNodeIdx, m_phaseCondition, m_preferHighestPercentageThrough );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                                   m_sourceStateNodeIdx = InvalidIndex;
            FootEvent::PhaseCondition                   m_phaseCondition = FootEvent::PhaseCondition::LeftFootDown;
            bool                                        m_preferHighestPercentageThrough = false;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        StateNode const*                                m_pSourceStateNode = nullptr;
        bool                                            m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FootstepEventPercentageThroughNode : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_sourceStateNodeIdx, m_phaseCondition, m_preferHighestPercentageThrough );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                                   m_sourceStateNodeIdx = InvalidIndex;
            FootEvent::PhaseCondition                   m_phaseCondition = FootEvent::PhaseCondition::LeftFootDown;
            bool                                        m_preferHighestPercentageThrough = false;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        StateNode const*                                m_pSourceStateNode = nullptr;
        float                                           m_result = -1.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API SyncEventConditionNode : public BoolValueNode
    {
    public:

        enum class TriggerMode : uint8
        {
            KRG_REGISTER_ENUM

            ExactlyAtEventIndex,
            GreaterThanEqualToEventIndex,
        };

        struct KRG_ENGINE_ANIMATION_API Settings : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_sourceStateNodeIdx, m_syncEventIdx, m_triggerMode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                                   m_sourceStateNodeIdx = InvalidIndex;
            TriggerMode                                 m_triggerMode = TriggerMode::ExactlyAtEventIndex;
            int32                                       m_syncEventIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        StateNode*                                      m_pSourceStateNode = nullptr;
        bool                                            m_result = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API CurrentSyncEventNode : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_sourceStateNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            NodeIndex                                   m_sourceStateNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        StateNode*                                      m_pSourceStateNode = nullptr;
        float                                           m_result = 0.0f;
    };
}