#pragma once

#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class FSelectorNode final : public AnimationNode
    {
    public:

        struct Settings : public AnimationNode::Settings
        {
            TInlineVector<NodeIndex, 5>                     m_optionNodeIndices;
            TInlineVector<NodeIndex, 5>                     m_conditionNodeIndices;
        };

    public:

        virtual bool IsValid() const override { return AnimationNode::IsValid() && m_pSelectedNode != nullptr; }
        virtual SyncTrack const& GetSyncTrack() const override { return IsValid() ? m_pSelectedNode->GetSyncTrack() : SyncTrack::s_defaultTrack; }

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

        int32 SelectOption( GraphContext& context ) const;

    private:

        TInlineVector<AnimationNode*, 5>                    m_optionNodes;
        TInlineVector<ValueNodeBool*, 5>                    m_conditions;
        AnimationNode*                                      m_pSelectedNode = nullptr;
        int32                                               m_selectedOptionIdx = InvalidIndex;
    };

    //-------------------------------------------------------------------------

    class FAnimationSelectorNode : public AnimationClipReferenceNode
    {
    public:

        struct Settings : public AnimationNode::Settings
        {
            TInlineVector<NodeIndex, 5>                     m_optionNodeIndices;
            TInlineVector<NodeIndex, 5>                     m_conditionNodeIndices;
        };

    public:

        virtual bool IsValid() const override { return AnimationNode::IsValid() && m_pSelectedNode != nullptr; }
        virtual SyncTrack const& GetSyncTrack() const override { return IsValid() ? m_pSelectedNode->GetSyncTrack() : SyncTrack::s_defaultTrack; }

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

        virtual AnimationClip const* GetAnimation() const override;
        virtual void DisableRootMotionSampling() override;
        virtual bool IsLooping() const override;

        int32 SelectOption( GraphContext& context ) const;

    private:

        TInlineVector<AnimationClipReferenceNode*, 5>       m_optionNodes;
        TInlineVector<ValueNodeBool*, 5>                    m_conditions;
        AnimationClipReferenceNode*                         m_pSelectedNode = nullptr;
        int32                                               m_selectedOptionIdx = InvalidIndex;
    };
}