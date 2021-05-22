#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationBlender.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API ParameterizedBlendNode : public AnimationNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public AnimationNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( AnimationNode::Settings, m_sourceNodeIndices, m_inputParameterValueNodeIdx, m_isSynchronized, m_allowLooping );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            TInlineVector<NodeIndex, 5>             m_sourceNodeIndices;
            NodeIndex                               m_inputParameterValueNodeIdx = InvalidIndex;
            bool                                    m_isSynchronized = false;
            bool                                    m_allowLooping = true;
        };

    protected:

        struct BlendRange
        {
            KRG_SERIALIZE_MEMBERS( m_sourceIdx0, m_sourceIdx1, m_parameterValueRange );

            NodeIndex                               m_sourceIdx0 = InvalidIndex;
            NodeIndex                               m_sourceIdx1 = InvalidIndex;
            FloatRange                              m_parameterValueRange = FloatRange( 0 );
        };

        struct Parameterization
        {
            KRG_SERIALIZE_MEMBERS( m_blendRanges, m_parameterRange );

            inline void Reset()
            {
                m_blendRanges.clear();
                m_parameterRange = FloatRange( 0 );
            }

            TInlineVector<BlendRange, 5>            m_blendRanges;
            FloatRange                              m_parameterRange;
        };

    public:

        virtual bool IsValid() const override;
        virtual SyncTrack const& GetSyncTrack() const override { return m_blendedSyncTrack; }

    protected:

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

        struct KRG_ENGINE_ANIMATION_API Settings final : public ParameterizedBlendNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ParameterizedBlendNode::Settings, m_parameterization );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            Parameterization                        m_parameterization;
        };

    private:

        virtual Parameterization const& GetParameterization() const { return GetSettings<RangedBlendNode>()->m_parameterization; }
    };

    //-------------------------------------------------------------------------

    class VelocityBlendNode final : public ParameterizedBlendNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ParameterizedBlendNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void InitializeParameterization( GraphContext& context ) override;
        virtual void ShutdownParameterization( GraphContext& context ) override;
        virtual Parameterization const& GetParameterization() const override { return m_parameterization; }

    protected:

        Parameterization                            m_parameterization; // Lazily initialized parameterization
        bool                                        m_parameterizationInitialized = false;
    };
}