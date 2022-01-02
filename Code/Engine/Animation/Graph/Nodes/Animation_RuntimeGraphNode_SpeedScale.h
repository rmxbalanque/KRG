#pragma once

#include "Animation_RuntimeGraphNode_Passthrough.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class KRG_ENGINE_ANIMATION_API SpeedScaleNode final : public PassthroughNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PassthroughNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PassthroughNode::Settings, m_scaleValueNodeIdx, m_scaleLimits, m_blendTime );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex               m_scaleValueNodeIdx = InvalidIndex;
            FloatRange              m_scaleLimits = FloatRange( 0, 0 );
            float                   m_blendTime = 0.2f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        inline float GetSpeedScale( GraphContext& context ) const
        {
            // Get clamped Modifier value
            float scaleMultiplier = m_pScaleValueNode->GetValue<float>( context );
            scaleMultiplier = GetSettings<SpeedScaleNode>()->m_scaleLimits.GetClampedValue( scaleMultiplier );
            return scaleMultiplier;
        }

    private:

        FloatValueNode*             m_pScaleValueNode = nullptr;
        float                       m_blendWeight = 1.0f; // Used to ensure the modifier is slowly blended in when coming from a sync'd transition that ends
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VelocityBasedSpeedScaleNode final : public PassthroughNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PassthroughNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PassthroughNode::Settings, m_desiredVelocityValueNodeIdx, m_blendTime );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex               m_desiredVelocityValueNodeIdx = InvalidIndex;
            float                   m_blendTime = 0.2f;
        };

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

    private:

        FloatValueNode*             m_pDesiredVelocityValueNode = nullptr;
        float                       m_blendWeight = 1.0f; // Used to ensure the modifier is slowly blended in when coming from a sync'd transition that ends
    };
}