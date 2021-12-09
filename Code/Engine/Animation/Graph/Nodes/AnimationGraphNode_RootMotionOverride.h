#pragma once

#include "AnimationGraphNode_Passthrough.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API RootMotionOverrideNode final : public PassthroughNode
    {
    public:

        // What elements of the root motion do we wish to override
        enum class OverrideFlags : uint8
        {
            HeadingX,
            HeadingY,
            HeadingZ,
            FacingX,
            FacingY,
            FacingZ,
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public PassthroughNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PassthroughNode::Settings, m_desiredHeadingVelocityNodeIdx, m_desiredFacingDirectionNodeIdx, m_linearVelocityLimitNodeIdx, m_angularVelocityLimitNodeIdx, m_maxLinearVelocity, m_maxAngularVelocity, m_overrideFlags );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex                        m_desiredHeadingVelocityNodeIdx = InvalidIndex;
            NodeIndex                        m_desiredFacingDirectionNodeIdx = InvalidIndex;
            NodeIndex                        m_linearVelocityLimitNodeIdx = InvalidIndex;
            NodeIndex                        m_angularVelocityLimitNodeIdx = InvalidIndex;
            float                            m_maxLinearVelocity = -1.0f;
            float                            m_maxAngularVelocity = -1.0f;
            TBitFlags<OverrideFlags>         m_overrideFlags;
        };

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual PoseNodeResult Update( GraphContext& context ) override;
        virtual PoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        void ModifyDisplacement( GraphContext& context, PoseNodeResult& NodeResult ) const;

    private:

        VectorValueNode*                    m_pDesiredHeadingVelocityNode = nullptr;
        VectorValueNode*                    m_pDesiredFacingDirectionNode = nullptr;
        FloatValueNode*                     m_pLinearVelocityLimitNode = nullptr;
        FloatValueNode*                     m_pAngularVelocityLimitNode = nullptr;
    };
}