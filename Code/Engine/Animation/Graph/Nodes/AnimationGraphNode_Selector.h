#pragma once

#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "System/Core/ThirdParty/cereal/archives/binary.hpp"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API FSelectorNode final : public AnimationNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public AnimationNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( AnimationNode::Settings, m_optionNodeIndices, m_conditionNodeIndices );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            TInlineVector<NodeIndex, 5>                     m_optionNodeIndices;
            TInlineVector<NodeIndex, 5>                     m_conditionNodeIndices;
        };

    public:

        virtual bool IsValid() const override { return AnimationNode::IsValid() && m_pSelectedNode != nullptr; }
        virtual SyncTrack const& GetSyncTrack() const override { return IsValid() ? m_pSelectedNode->GetSyncTrack() : SyncTrack::s_defaultTrack; }

    private:

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

    class KRG_ENGINE_ANIMATION_API FAnimationSelectorNode final : public AnimationClipReferenceNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public AnimationNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( AnimationNode::Settings, m_optionNodeIndices, m_conditionNodeIndices );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            TInlineVector<NodeIndex, 5>                     m_optionNodeIndices;
            TInlineVector<NodeIndex, 5>                     m_conditionNodeIndices;
        };

    public:

        virtual bool IsValid() const override { return AnimationNode::IsValid() && m_pSelectedNode != nullptr; }
        virtual SyncTrack const& GetSyncTrack() const override { return IsValid() ? m_pSelectedNode->GetSyncTrack() : SyncTrack::s_defaultTrack; }

    private:

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