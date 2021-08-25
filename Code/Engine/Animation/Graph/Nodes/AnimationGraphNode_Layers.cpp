#include "AnimationGraphNode_Layers.h"
#include "AnimationGraphNode_StateMachine.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_DefaultPose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_Blend.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void LayerBlendNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<LayerBlendNode>( nodePtrs, options );

        SetNodePtrFromIndex( nodePtrs, m_baseNodeIdx, pNode->m_pBaseLayerNode );

        for ( auto const& layerSettings : m_layerSettings )
        {
            StateMachineNode*& pLayerNode = pNode->m_layers.emplace_back();
            SetNodePtrFromIndex( nodePtrs, layerSettings.m_layerNodeIdx, pLayerNode );
        }
    }

    void LayerBlendNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() && m_pBaseLayerNode != nullptr );

        auto pSettings = GetSettings<LayerBlendNode>();

        PoseNode::InitializeInternal( context, initialTime );
        m_pBaseLayerNode->Initialize( context, initialTime );

        int32 const numLayers = (int32) m_layers.size();
        for ( auto i = 0; i < numLayers; i++ )
        {
            auto pLayer = m_layers[i];
            KRG_ASSERT( pLayer != nullptr );

            // Only initialize the start time for synchronized layers
            if ( pSettings->m_layerSettings[i].m_isSynchronized )
            {
                pLayer->Initialize( context, initialTime );
            }
            else
            {
                pLayer->Initialize( context, SyncTrackTime() );
            }
        }

        m_currentTime = m_pBaseLayerNode->GetCurrentTime();
        m_duration = m_pBaseLayerNode->GetDuration();
    }

    void LayerBlendNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        for ( auto& Layer : m_layers )
        {
            KRG_ASSERT( Layer != nullptr );
            Layer->Shutdown( context );
        }

        m_pBaseLayerNode->Shutdown( context );
        PoseNode::ShutdownInternal( context );
    }

    void LayerBlendNode::DeactivateBranch( GraphContext& context )
    {
        if ( IsValid() )
        {
            PoseNode::DeactivateBranch( context );
            m_pBaseLayerNode->DeactivateBranch( context );

            auto const NumLayers = m_layers.size();
            for ( auto i = 0; i < NumLayers; i++ )
            {
                static_cast<PoseNode*>( m_layers[i] )->DeactivateBranch( context );
            }
        }
    }

    // NB: Layered nodes always update the base according to the specified update time delta or time range. The layers are then updated relative to the base.
    PoseNodeResult LayerBlendNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<LayerBlendNode>();

        PoseNodeResult Result;

        if ( IsValid() )
        {
            // Update the base
            //-------------------------------------------------------------------------

            MarkNodeActive( context );
            m_previousTime = m_pBaseLayerNode->GetCurrentTime();
            Result = m_pBaseLayerNode->Update( context );
            m_currentTime = m_pBaseLayerNode->GetCurrentTime();
            m_duration = m_pBaseLayerNode->GetDuration();

            // Update the layers
            //-------------------------------------------------------------------------

            // We need to register a task at the base layer in all cases - since we blend the layers tasks on top of it
            if ( !Result.HasRegisteredTasks() )
            {
                Result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::DefaultPoseTask>( GetNodeIndex(), Pose::InitialState::ReferencePose );
            }

            UpdateLayers( context, Result );
        }

        return Result;
    }

    // NB: Layered nodes always update the base according to the specified update time delta or time range. The layers are then updated relative to the base.
    PoseNodeResult LayerBlendNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<LayerBlendNode>();

        PoseNodeResult result;

        if ( IsValid() )
        {
            // Update the base
            //-------------------------------------------------------------------------

            MarkNodeActive( context );
            result = m_pBaseLayerNode->Update( context, updateRange );
            m_previousTime = GetSyncTrack().GetPercentageThrough( updateRange.m_startTime );
            m_currentTime = m_pBaseLayerNode->GetCurrentTime();
            m_duration = m_pBaseLayerNode->GetDuration();

            // Update the layers
            //-------------------------------------------------------------------------

            // We need to register a task at the base layer in all cases - since we blend the layers tasks on top of it
            if ( !result.HasRegisteredTasks() )
            {
                result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::DefaultPoseTask>( GetNodeIndex(), Pose::InitialState::ReferencePose );
            }

            UpdateLayers( context, result );
        }

        return result;
    }

    void LayerBlendNode::UpdateLayers( GraphContext& context, PoseNodeResult& nodeResult )
    {
        KRG_ASSERT( context.IsValid() && IsValid() );
        auto pSettings = GetSettings<LayerBlendNode>();

        SyncTrackTime const baseStartTime = m_pBaseLayerNode->GetSyncTrack().GetTime( m_pBaseLayerNode->GetPreviousTime() );
        SyncTrackTime const baseEndTime = m_pBaseLayerNode->GetSyncTrack().GetTime( m_pBaseLayerNode->GetCurrentTime() );
        SyncTrackTimeRange layerUpdateRange( baseStartTime, baseEndTime );

        int32 const numLayers = (int32) m_layers.size();
        for ( auto i = 0; i < numLayers; i++ )
        {
            // Create the layer context
            //-------------------------------------------------------------------------

            // If we are currently in a layer then cache it so that we can safely overwrite it
            if ( context.m_layerContext.IsSet() )
            {
                m_previousContext = context.m_layerContext;
            }

            context.m_layerContext.BeginLayer();

            // Record the current state of the registered tasks
            auto const TaskMarker = context.m_pTaskSystem->GetCurrentTaskIndexMarker();

            // Update time and layer weight
            //-------------------------------------------------------------------------
            // Always update the layers as they are state machines and transitions need to be evaluated

            #if KRG_DEVELOPMENT_TOOLS
            context.GetRootMotionActionRecorder()->PushBlendHierarchyLevel();
            #endif

            PoseNodeResult layerResult;
            StateMachineNode* pLayerStateMachine = m_layers[i];

            if ( pSettings->m_layerSettings[i].m_isSynchronized )
            {
                layerResult = static_cast<PoseNode*>( pLayerStateMachine )->Update( context, layerUpdateRange );
            }
            else
            {
                layerResult = static_cast<PoseNode*>( pLayerStateMachine )->Update( context );
            }

            // Register the layer blend tasks
            //-------------------------------------------------------------------------

            // If we registered a task for this layer, then we need to blend
            if ( layerResult.HasRegisteredTasks() )
            {
                // Create a blend task if the layer is enabled
                if ( context.m_layerContext.m_layerWeight > 0 )
                {
                    TBitFlags<RootMotionBlendOptions> rootMotionBlendOptions;
                    rootMotionBlendOptions.SetFlag( RootMotionBlendOptions::IgnoreTarget, pSettings->m_onlySampleBaseRootMotion );

                    nodeResult.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::BlendTask>( GetNodeIndex(), nodeResult.m_taskIdx, layerResult.m_taskIdx, context.m_layerContext.m_layerWeight, pSettings->m_layerSettings[i].m_blendOptions, context.m_layerContext.m_pLayerMask );
                    nodeResult.m_rootMotionDelta = Blender::BlendRootMotionDeltas( nodeResult.m_rootMotionDelta, layerResult.m_rootMotionDelta, context.m_layerContext.m_layerWeight, rootMotionBlendOptions );

                    #if KRG_DEVELOPMENT_TOOLS
                    context.GetRootMotionActionRecorder()->RecordBlend( GetNodeIndex(), nodeResult.m_rootMotionDelta );
                    #endif

                }
                else // Remove any layer registered tasks
                {
                    context.m_pTaskSystem->RollbackToTaskIndexMarker( TaskMarker );

                    #if KRG_DEVELOPMENT_TOOLS
                    context.GetRootMotionActionRecorder()->PopBlendHierarchyLevel();
                    #endif
                }
            }
            else
            {
                #if KRG_DEVELOPMENT_TOOLS
                context.GetRootMotionActionRecorder()->PopBlendHierarchyLevel();
                #endif
            }

            // Update events for the Layer
            //-------------------------------------------------------------------------

            SampledEventRange const& layerEventRange = layerResult.m_sampledEventRange;
            int32 const numLayerEvents = layerEventRange.GetLength();
            if ( numLayerEvents > 0 )
            {
                // Update events and mark them as ignored if requested
                context.m_sampledEvents.UpdateWeights( layerEventRange, context.m_layerContext.m_layerWeight );

                if ( pSettings->m_layerSettings[i].m_ignoreEvents )
                {
                    context.m_sampledEvents.SetFlag( layerEventRange, SampledEvent::Flags::Ignored );
                }

                // Merge layer sampled event into the layer's nodes range
                int32 const numCurrentEvents = nodeResult.m_sampledEventRange.GetLength();
                if ( numCurrentEvents > 0 )
                {
                    // Combine sampled event range - the current range must always be before the layer's range
                    KRG_ASSERT( nodeResult.m_sampledEventRange.m_endIdx <= layerEventRange.m_startIdx );
                    nodeResult.m_sampledEventRange.m_endIdx = layerEventRange.m_endIdx;
                }
                else
                {
                    nodeResult.m_sampledEventRange = layerEventRange;
                }
            }

            // Reset the layer context
            //-------------------------------------------------------------------------

            // Reset the layer context
            context.m_layerContext.EndLayer();

            // Restore previous context if it existed;
            if ( m_previousContext.IsSet() )
            {
                context.m_layerContext = m_previousContext;
            }
        }
    }
}