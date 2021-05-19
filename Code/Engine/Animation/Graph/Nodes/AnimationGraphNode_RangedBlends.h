#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationBlender.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ParameterizedBlendNode : public AnimationNode
    {
    public:

        struct Settings : public AnimationNode::Settings
        {
            TInlineVector<NodeIndex, 5>             m_sourceNodeIndices;
            NodeIndex                               m_inputParameterValueNodeIdx = InvalidIndex;
            bool                                    m_isSynchronized = false;
            bool                                    m_allowLooping = true;
        };

    protected:

        struct BlendRange
        {
            NodeIndex                               m_sourceIdx0 = InvalidIndex;
            NodeIndex                               m_sourceIdx1 = InvalidIndex;
            TRange<float>                           m_parameterValueRange = TRange<float>( 0 );
        };

        struct Parameterization
        {
            inline void Reset()
            {
                m_blendRanges.clear();
                m_parameterRange = TRange<float>( 0 );
            }

            TInlineVector<BlendRange, 5>            m_blendRanges;
            TRange<float>                           m_parameterRange;
        };

    public:

        virtual bool IsValid() const override;
        virtual SyncTrack const& GetSyncTrack() const override { return m_blendedSyncTrack; }

    protected:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void DeactivateBranch( GraphContext& context ) override final;

        virtual UpdateResult Update( GraphContext& context ) override final;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override final;

        // Parameterization
        //-------------------------------------------------------------------------

        // Optional function to create a parameterization if one isnt serialized
        virtual void InitializeParameterization( GraphContext& context ) {}
        virtual void ShutdownParameterization( GraphContext& context ) {}

        virtual Parameterization const& GetParameterization() const = 0;
        void SelectBlendRange( GraphContext& context );

        // Blending
        //-------------------------------------------------------------------------

        [[nodiscard]] SampledEventRange CombineAndUpdateEvents( GraphContext& context, UpdateResult const& SourceTaskResult, UpdateResult const& TargetTaskResult, float const blendWeight );

    protected:

        TInlineVector<AnimationNode*, 5>            m_sourceNodes;
        ValueNodeFloat*                             m_pInputParameterValueNode = nullptr;
        int32                                       m_selectedRangeIdx = InvalidIndex;
        float                                       m_blendWeight = 0.0f;
        SyncTrack                                   m_blendedSyncTrack;
    };

    //-------------------------------------------------------------------------

    class RangedBlendNode final : public ParameterizedBlendNode
    {
    public:

        struct Settings : public ParameterizedBlendNode::Settings
        {
            Parameterization                        m_parameterization;
        };

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual Parameterization const& GetParameterization() const { return GetSettings<RangedBlendNode>()->m_parameterization; }
    };

    //-------------------------------------------------------------------------

    class VelocityBlendNode final : public ParameterizedBlendNode
    {
    private:

        virtual void InitializeParameterization( GraphContext& context ) override;
        virtual void ShutdownParameterization( GraphContext& context ) override;
        virtual Parameterization const& GetParameterization() const { return m_parameterization; }

    protected:

        Parameterization                            m_parameterization; // Lazily initialized parameterization
        bool                                        m_parameterizationInitialized = false;
    };
}