#pragma once

#include "Animation_RuntimeGraphNode_Passthrough.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class Ragdoll;
    struct RagdollDefinition;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class KRG_ENGINE_ANIMATION_API PoweredRagdollNode final : public PassthroughNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PassthroughNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PassthroughNode::Settings, m_physicsBlendWeightNodeIdx, m_physicsBlendWeight, m_profileID, m_dataSlotIdx, m_isGravityEnabled );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex               m_physicsBlendWeightNodeIdx = InvalidIndex;
            DataSetSlotIndex        m_dataSlotIdx = InvalidIndex;
            StringID                m_profileID;
            float                   m_physicsBlendWeight = 1.0f;
            bool                    m_isGravityEnabled;
        };

    private:

        virtual bool IsValid() const override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        GraphPoseNodeResult RegisterRagdollTasks( GraphContext& context, GraphPoseNodeResult const& childResult );

    private:

        Physics::RagdollDefinition const*       m_pRagdollDefinition = nullptr;
        FloatValueNode*                         m_pBlendWeightValueNode = nullptr;
        Physics::Ragdoll*                       m_pRagdoll = nullptr;
        bool                                    m_isFirstUpdate = false;
    };
}