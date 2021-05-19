#pragma once

#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AnimationClipNode : public AnimationClipReferenceNode
    {

    public:

        struct Settings : public AnimationNode::Settings
        {

            NodeIndex                                   m_playInReverseValueNodeIdx = InvalidIndex;
            bool                                        m_shouldSampleRootMotion = true;
            bool                                        m_allowLooping = false;
            UUID                                        m_animationID;
        };

    public:

        virtual bool IsValid() const override;

        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        virtual AnimationClip const* GetAnimation() const final { KRG_ASSERT( IsValid() ); return m_pAnimation; }
        virtual void DisableRootMotionSampling() final { KRG_ASSERT( IsValid() ); m_shouldSampleRootMotion = false; }
        virtual bool IsLooping() const final { return GetSettings<AnimationClipNode>()->m_allowLooping; }

        virtual SyncTrack const& GetSyncTrack() const override { KRG_ASSERT( IsValid() ); return m_pAnimation->GetSyncTrack(); }

    private:

        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        UpdateResult CalculateResult( GraphContext& context, bool bIsSynchronizedUpdate ) const;

    private:

        AnimationClip const*                            m_pAnimation = nullptr;
        ValueNodeBool*                                  m_pPlayInReverseValueNode = nullptr;
        bool                                            m_shouldPlayInReverse = false;
        bool                                            m_shouldSampleRootMotion = true;
    };
}