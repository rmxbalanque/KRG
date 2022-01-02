#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class KRG_ENGINE_ANIMATION_API PassthroughNode : public PoseNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex       m_childNodeIdx = InvalidIndex;
        };

    public:

        virtual bool IsValid() const override { return PoseNode::IsValid() && IsChildValid(); }
        virtual SyncTrack const& GetSyncTrack() const override;

    protected:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        // Syntactic sugar for readability in derived classes
        inline bool IsChildValid() const { return m_pChildNode != nullptr && m_pChildNode->IsValid(); }

        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

    protected:

        PoseNode*      m_pChildNode = nullptr;
    };
}