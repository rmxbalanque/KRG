#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API PassthroughNode : public AnimationNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public AnimationNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( AnimationNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex       m_childNodeIdx = InvalidIndex;
        };

    public:

        virtual bool IsValid() const override { return AnimationNode::IsValid() && IsChildValid(); }
        virtual SyncTrack const& GetSyncTrack() const override;

    protected:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        // Syntactic sugar for readability in derived classes
        inline bool IsChildValid() const { return m_pChildNode->IsValid(); }

        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

    protected:

        AnimationNode*      m_pChildNode = nullptr;
    };
}