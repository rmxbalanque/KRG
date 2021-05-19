#pragma once

#include "AnimationGraphNode_Passthrough.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class RootMotionOverrideNode : public PassthroughNode
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

        struct Settings : public PassthroughNode::Settings
        {
            NodeIndex                        m_desiredHeadingVelocityNodeIdx = InvalidIndex;
            NodeIndex                        m_desiredFacingDirectionNodeIdx = InvalidIndex;
            NodeIndex                        m_linearVelocityLimitNodeIdx = InvalidIndex;
            NodeIndex                        m_angularVelocityLimitNodeIdx = InvalidIndex;
            float                            m_maxLinearVelocity = 0.0f;
            float                            m_maxAngularVelocity = 0.0f;
            TBitFlags<OverrideFlags>         m_overrideFlags;
        };

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        void ModifyDisplacement( GraphContext& context, UpdateResult& NodeResult ) const;

    private:

        ValueNodeVector*                    m_pDesiredHeadingVelocityNode = nullptr;
        ValueNodeVector*                    m_pDesiredFacingDirectionNode = nullptr;
        ValueNodeFloat*                     m_pLinearVelocityLimitNode = nullptr;
        ValueNodeFloat*                     m_pAngularVelocityLimitNode = nullptr;
    };
}