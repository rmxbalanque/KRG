#pragma once

#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API AnimationClipNode final : public AnimationClipReferenceNode
    {

    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public AnimationNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( AnimationNode::Settings, m_playInReverseValueNodeIdx, m_shouldSampleRootMotion, m_allowLooping, m_animationID );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                                   m_playInReverseValueNodeIdx = InvalidIndex;
            bool                                        m_shouldSampleRootMotion = true;
            bool                                        m_allowLooping = false;
            uint16                                      m_animationID;
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