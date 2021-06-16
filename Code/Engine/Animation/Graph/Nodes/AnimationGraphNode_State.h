#pragma once

#include "AnimationGraphNode_Passthrough.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API StateNode final : public PassthroughNode
    {
        friend class TransitionNode;

    public:

        enum class TransitionState : uint8
        {
            None,
            TransitioningIn,
            TransitioningOut,
        };
        //-------------------------------------------------------------------------

        struct TimedEvent
        {
            KRG_SERIALIZE_MEMBERS( m_ID, m_timeValue );

            TimedEvent() = default;

            TimedEvent( StringID ID, Seconds value )
                : m_ID ( ID )
                , m_timeValue( value )
            {}

            StringID                                    m_ID;
            Seconds                                     m_timeValue;
        };

        //-------------------------------------------------------------------------

        struct KRG_ENGINE_ANIMATION_API Settings : public PassthroughNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PassthroughNode::Settings, m_entryEvents, m_executeEvents, m_exitEvents, m_timedRemainingEvents, m_timedElapsedEvents, m_layerBoneMaskNodeIdx, m_layerWeightNodeIdx, m_isOffState );

        public:

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            TInlineVector<StringID, 3>                  m_entryEvents;
            TInlineVector<StringID, 3>                  m_executeEvents;
            TInlineVector<StringID, 3>                  m_exitEvents;
            TInlineVector<TimedEvent, 1>                m_timedRemainingEvents;
            TInlineVector<TimedEvent, 1>                m_timedElapsedEvents;
            NodeIndex                                   m_layerBoneMaskNodeIdx = InvalidIndex;
            NodeIndex                                   m_layerWeightNodeIdx = InvalidIndex;
            bool                                        m_isOffState = false;
        };

    public:

        virtual PoseNodeResult Update( GraphContext& context ) override;
        virtual PoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

        // State info
        inline float GetElapsedTimeInState() const { return m_elapsedTimeInState; }
        inline SampledEventRange GetSampledGraphEventRange() const { return m_sampledEventRange; }
        inline bool IsOffState() const { return GetSettings<StateNode>()->m_isOffState; }

        // Transitions
        inline void SetTransitioningState( TransitionState newState ) { m_transitionState = newState; }
        inline bool IsTransitioning() const { return m_transitionState != TransitionState::None; }
        inline bool IsTransitioningIn() const { return m_transitionState == TransitionState::TransitioningIn; }
        inline bool IsTransitioningOut() const { return m_transitionState == TransitionState::TransitioningOut; }

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        void StartTransitionIn( GraphContext& context );
        void StartTransitionOut( GraphContext& context );
        void SampleStateEvents( GraphContext& context );
        void UpdateLayerContext( GraphContext& context );

    private:

        SampledEventRange                               m_sampledEventRange;
        BoneMaskValueNode*                              m_pBoneMaskNode = nullptr;
        FloatValueNode*                                 m_pLayerWeightNode = nullptr;
        Seconds                                         m_elapsedTimeInState = 0.0f;
        TransitionState                                 m_transitionState = TransitionState::None;
        bool                                            m_isFirstStateUpdate = false;
    };
}