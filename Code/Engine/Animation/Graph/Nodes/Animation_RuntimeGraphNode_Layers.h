#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"
#include "Engine/Animation/AnimationBlender.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class StateMachineNode;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API LayerBlendNode final : public PoseNode
    {

    public:

        struct LayerSettings
        {
            KRG_SERIALIZE_MEMBERS( m_layerNodeIdx, m_isSynchronized, m_ignoreEvents, m_blendOptions );

            GraphNodeIndex                                       m_layerNodeIdx = InvalidIndex;
            bool                                            m_isSynchronized = false;
            bool                                            m_ignoreEvents = false;
            TBitFlags<PoseBlendOptions>                     m_blendOptions;
        };

        //-------------------------------------------------------------------------

        struct KRG_ENGINE_ANIMATION_API Settings : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_baseNodeIdx, m_onlySampleBaseRootMotion, m_layerSettings );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex                                       m_baseNodeIdx = InvalidIndex;
            bool                                            m_onlySampleBaseRootMotion = true;
            TInlineVector<LayerSettings, 3>                 m_layerSettings;
        };

    public:

        virtual SyncTrack const& GetSyncTrack() const override { KRG_ASSERT( IsValid() ); return m_pBaseLayerNode->GetSyncTrack(); }
        virtual bool IsValid() const override { return PoseNode::IsValid() && m_pBaseLayerNode->IsValid(); }

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        void UpdateLayers( GraphContext& context, GraphPoseNodeResult& NodeResult );

    private:

        PoseNode*                                           m_pBaseLayerNode = nullptr;
        TInlineVector<StateMachineNode*, 3>                 m_layers;
        GraphLayerContext                                   m_previousContext;

        #if KRG_DEVELOPMENT_TOOLS
        int16                                               m_rootMotionActionIdxBase = InvalidIndex;
        #endif
    };
}