#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API PassthroughNode : public PoseNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            NodeIndex       m_childNodeIdx = InvalidIndex;
        };

    public:

        virtual bool IsValid() const override { return PoseNode::IsValid() && IsChildValid(); }
        virtual SyncTrack const& GetSyncTrack() const override;

    protected:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        // Syntactic sugar for readability in derived classes
        inline bool IsChildValid() const { return m_pChildNode != nullptr && m_pChildNode->IsValid(); }

        virtual PoseNodeResult Update( GraphContext& context ) override;
        virtual PoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

    protected:

        PoseNode*      m_pChildNode = nullptr;
    };
}