#pragma once

#include "AnimationGraphNode_Passthrough.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class SpeedScaleNode : public PassthroughNode
    {
    public:

        struct Settings : public PassthroughNode::Settings
        {
            NodeIndex               m_scaleValueNodeIdx = InvalidIndex;
            TRange<float>           m_scaleLimits = TRange<float>( 0, 0 );
            float                   m_blendTime = 0.2f;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        inline float GetSpeedScale( GraphContext& context ) const
        {
            // Get clamped Modifier value
            float scaleMultiplier = m_pScaleValueNode->GetValue<float>( context );
            scaleMultiplier = GetSettings<SpeedScaleNode>()->m_scaleLimits.GetClampedValue( scaleMultiplier );
            return scaleMultiplier;
        }

    private:

        ValueNodeFloat*             m_pScaleValueNode = nullptr;
        float                       m_blendWeight = 1.0f; // Used to ensure the modifier is slowly blended in when coming from a sync'd transition that ends
    };

    //-------------------------------------------------------------------------

    class VelocityBasedSpeedScaleNode : public PassthroughNode
    {
    public:

        struct Settings : public PassthroughNode::Settings
        {
            NodeIndex               m_desiredVelocityValueNodeIdx = InvalidIndex;
            float                   m_blendTime = 0.2f;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

    private:

        ValueNodeFloat*             m_pDesiredVelocityValueNode = nullptr;
        float                       m_blendWeight = 1.0f; // Used to ensure the modifier is slowly blended in when coming from a sync'd transition that ends
    };
}