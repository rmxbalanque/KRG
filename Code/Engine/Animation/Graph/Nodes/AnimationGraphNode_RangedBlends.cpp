#include "AnimationGraphNode_RangedBlends.h"
#include "Engine/Animation/AnimationClip.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_Blend.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ParameterizedBlendNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        KRG_ASSERT( options == GraphNode::Settings::InitOptions::OnlySetPointers );
        auto pNode = CreateNode<VelocityBlendNode>( nodePtrs, options );

        SetNodePtrFromIndex( nodePtrs, m_inputParameterValueNodeIdx, pNode->m_pInputParameterValueNode );

        pNode->m_sourceNodes.reserve( m_sourceNodeIndices.size() );
        for ( auto sourceIdx : m_sourceNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, sourceIdx, pNode->m_sourceNodes.emplace_back() );
        }
    }

    bool ParameterizedBlendNode::IsValid() const
    {
        if ( !AnimationNode::IsValid() )
        {
            return false;
        }

        if ( !m_pInputParameterValueNode->IsValid() )
        {
            return false;
        }

        for ( auto pSource : m_sourceNodes )
        {
            if ( !pSource->IsValid() )
            {
                return false;
            }
        }

        return true;
    }

    void ParameterizedBlendNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_pInputParameterValueNode != nullptr && m_sourceNodes.size() > 1 );

        AnimationNode::InitializeInternal( context, initialTime );

        for ( auto pSourceNode : m_sourceNodes )
        {
            pSourceNode->Initialize( context, initialTime );
        }

        m_pInputParameterValueNode->Initialize( context );

        //-------------------------------------------------------------------------

        if ( IsValid() )
        {
            InitializeParameterization( context );
            SelectBlendRange( context );
            auto const& blendRange = GetParameterization().m_blendRanges[m_selectedRangeIdx];
            auto Source0 = m_sourceNodes[blendRange.m_sourceIdx0];
            auto Source1 = m_sourceNodes[blendRange.m_sourceIdx1];
            KRG_ASSERT( Source0 != nullptr && Source1 != nullptr );

            auto pSettings = GetSettings<RangedBlendNode>();
            if ( pSettings->m_isSynchronized )
            {
                SyncTrack const& SyncTrackSource0 = Source0->GetSyncTrack();
                SyncTrack const& SyncTrackSource1 = Source1->GetSyncTrack();
                m_blendedSyncTrack = SyncTrack( SyncTrackSource0, SyncTrackSource1, m_blendWeight );
                m_duration = SyncTrack::CalculateDurationSynchronized( Source0->GetDuration(), Source1->GetDuration(), SyncTrackSource0.GetNumEvents(), SyncTrackSource1.GetNumEvents(), m_blendedSyncTrack.GetNumEvents(), m_blendWeight );
            }
            else
            {
                m_duration = Math::Lerp( Source0->GetDuration(), Source1->GetDuration(), m_blendWeight );
            }

            m_previousTime = GetSyncTrack().GetPercentageThrough( initialTime );
            m_currentTime = m_previousTime;
        }
    }

    void ParameterizedBlendNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( IsValid() )
        {
            ShutdownParameterization( context );
        }

        //-------------------------------------------------------------------------

        m_pInputParameterValueNode->Shutdown( context );

        for ( auto Source : m_sourceNodes )
        {
            Source->Shutdown( context );
        }

        AnimationNode::ShutdownInternal( context );
    }

    void ParameterizedBlendNode::SelectBlendRange( GraphContext& context)
    {
        auto const& parameterization = GetParameterization();

        m_selectedRangeIdx = InvalidIndex;
        m_blendWeight = -1.0f;

        // Get Parameter clamped to parameterization range
        float inputParameterValue = m_pInputParameterValueNode->GetValue<float>( context );
        inputParameterValue = parameterization.m_parameterRange.GetClampedValue( inputParameterValue );

        // Find matching source nodes and blend weight
        auto const numBlendRanges = parameterization.m_blendRanges.size();
        for ( auto i = 0; i < numBlendRanges; i++ )
        {
            if ( parameterization.m_blendRanges[i].m_parameterValueRange.ContainsInclusive( inputParameterValue ) )
            {
                m_selectedRangeIdx = i;
                m_blendWeight = parameterization.m_blendRanges[i].m_parameterValueRange.GetPercentageThrough( inputParameterValue );
                break;
            }
        }

        // Ensure we found a valid range
        KRG_ASSERT( m_blendWeight != -1 );
    }

    void ParameterizedBlendNode::DeactivateBranch( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( IsValid() )
        {
            AnimationNode::DeactivateBranch( context );

            for ( auto pSource0 : m_sourceNodes )
            {
                if ( pSource0->IsNodeActive( context ) )
                {
                    pSource0->DeactivateBranch( context );
                }
            }
        }
    }

    SampledEventRange ParameterizedBlendNode::CombineAndUpdateEvents( GraphContext& context, UpdateResult const& sourceResult0, UpdateResult const& sourceResult1, float const blendWeight )
    {
        SampledEventRange combinedRange;
        SampledEventRange const& sourceEventRange0 = sourceResult0.m_sampledEventRange;
        SampledEventRange const& sourceEventRange1 = sourceResult1.m_sampledEventRange;

        // If we are fully in one or the other source, then only sample events for the active source
        if ( m_blendWeight == 0.0f )
        {
            combinedRange = sourceEventRange0;
            KRG_ASSERT( context.m_sampledEvents.IsValidRange( sourceEventRange1 ) );
            context.m_sampledEvents.UpdateWeights( sourceEventRange1, 0.0f );
        }
        else if ( m_blendWeight == 1.0f )
        {
            combinedRange = sourceEventRange1;
            KRG_ASSERT( context.m_sampledEvents.IsValidRange( sourceEventRange0 ) );
            context.m_sampledEvents.UpdateWeights( sourceEventRange0, 0.0f );
        }
        else // Combine events
        {
            // Sample events for both sources and updated sampled event weights
            uint32 const numEventsSource0 = sourceEventRange0.GetLength();
            uint32 const numEventsSource1 = sourceEventRange1.GetLength();

            if ( ( numEventsSource0 + numEventsSource1 ) > 0 )
            {
                context.m_sampledEvents.UpdateWeights( sourceEventRange0, m_blendWeight );
                context.m_sampledEvents.UpdateWeights( sourceEventRange1, 1.0f - m_blendWeight );

                // Combine sampled event range - source0's range must always be before source1's range
                if ( numEventsSource0 > 0 && numEventsSource1 > 0 )
                {
                    KRG_ASSERT( sourceEventRange0.m_endIdx <= sourceEventRange1.m_startIdx );
                    combinedRange.m_startIdx = sourceEventRange0.m_startIdx;
                    combinedRange.m_endIdx = sourceEventRange1.m_endIdx;
                }
                else if ( numEventsSource0 > 0 ) // Only source0 has sampled events
                {
                    combinedRange = sourceEventRange0;
                }
                else // Only source1 has sampled events
                {
                    combinedRange = sourceEventRange1;
                }
            }
            else
            {
                combinedRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
            }
        }

        KRG_ASSERT( context.m_sampledEvents.IsValidRange( combinedRange ) );
        return combinedRange;
    }

    UpdateResult ParameterizedBlendNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        UpdateResult result;

        if ( !IsValid() )
        {
            return result;
        }

        //-------------------------------------------------------------------------

        // If this node is synchronized, call the synchronize update
        auto pSettings = GetSettings<RangedBlendNode>();
        if ( pSettings->m_isSynchronized )
        {
            Percentage const deltaPercentage = Percentage( context.m_deltaTime / m_duration );
            Percentage const fromTime = m_currentTime;
            Percentage const toTime = Percentage::Clamp( m_currentTime + deltaPercentage, pSettings->m_allowLooping );

            SyncTrackTimeRange UpdateRange;
            UpdateRange.m_startTime = m_blendedSyncTrack.GetTime( fromTime );
            UpdateRange.m_endTime = m_blendedSyncTrack.GetTime( toTime );
            return Update( context, UpdateRange );
        }
        else // Update in a asynchronous manner
        {
            MarkNodeActive( context );
            SelectBlendRange( context );

            auto const& parameterization = GetParameterization();
            BlendRange const& selectedBlendRange = parameterization.m_blendRanges[m_selectedRangeIdx];
            auto pSource0 = m_sourceNodes[selectedBlendRange.m_sourceIdx0];
            auto pSource1 = m_sourceNodes[selectedBlendRange.m_sourceIdx1];
            KRG_ASSERT( pSource0 != nullptr && pSource1 != nullptr );

            //-------------------------------------------------------------------------

            if ( m_blendWeight == 0.0f )
            {
                result = pSource0->Update( context );
                m_duration = pSource0->GetDuration();
            }
            else if ( m_blendWeight == 1.0f )
            {
                result = pSource1->Update( context );
                m_duration = pSource1->GetDuration();
            }
            else
            {
                #if KRG_DEVELOPMENT_TOOLS
                context.GetRootMotionActionRecorder()->PushBlendHierarchyLevel();
                #endif

                // Update source nodes
                UpdateResult const sourceResult0 = pSource0->Update( context );
                UpdateResult const sourceResult1 = pSource1->Update( context );

                // Update internal time
                m_duration = Math::Lerp( pSource0->GetDuration(), pSource1->GetDuration(), m_blendWeight );

                // Update events
                result.m_sampledEventRange = CombineAndUpdateEvents( context, sourceResult0, sourceResult1, m_blendWeight );

                // Do we need to blend between the two nodes?
                if ( sourceResult0.HasRegisteredTasks() && sourceResult1.HasRegisteredTasks() )
                {
                    result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::BlendTask>( GetNodeIdx(), sourceResult0.m_taskIdx, sourceResult1.m_taskIdx, m_blendWeight, PoseBlendOptions::Interpolate );
                    result.m_rootMotionDelta = Blender::BlendRootMotionDeltas( sourceResult0.m_rootMotionDelta, sourceResult1.m_rootMotionDelta, m_blendWeight);

                    #if KRG_DEVELOPMENT_TOOLS
                    context.GetRootMotionActionRecorder()->RecordBlend( GetNodeIdx(), result.m_rootMotionDelta );
                    #endif
                }
                else // Keep the result that has a pose
                {
                    result = ( sourceResult0.HasRegisteredTasks() ) ? sourceResult0 : sourceResult1;

                    #if KRG_DEVELOPMENT_TOOLS
                    context.GetRootMotionActionRecorder()->PopBlendHierarchyLevel();
                    #endif
                }
            }

            // Update internal time
            auto const deltaPercentage = Percentage( context.m_deltaTime/ m_duration );
            m_previousTime = m_currentTime;
            m_currentTime = m_currentTime + deltaPercentage;

            // Update time / duration for the remaining source nodes
            // For unsynchronized update, we unfortunately need to update all nodes but we ensure that no unnecessary tasks are registered
            // Ensure that all event weights from the inactive nodes are also set to 0
            //-------------------------------------------------------------------------

            // Clear source nodes ptrs we didn't update
            if ( m_blendWeight == 0.0f )
            {
                pSource1 = nullptr;
            }
            else if ( m_blendWeight == 1.0f )
            {
                pSource0 = nullptr;
            }

            for ( auto pSourceNode : m_sourceNodes )
            {
                if ( pSourceNode != pSource0 && pSourceNode != pSource1 )
                {
                    auto const taskMarker = context.m_pTaskSystem->GetCurrentTaskIndexMarker();
                    UpdateResult const updateResult = static_cast<AnimationNode*>( pSourceNode )->Update( context );
                    context.m_sampledEvents.UpdateWeights( updateResult.m_sampledEventRange, 0.0f );
                    context.m_pTaskSystem->RollbackToTaskIndexMarker( taskMarker );
                }
            }
        }

        return result;
    }

    UpdateResult ParameterizedBlendNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        UpdateResult result;

        if ( IsValid() )
        {
            MarkNodeActive( context );
            SelectBlendRange( context );

            auto const& parameterization = GetParameterization();
            BlendRange const& selectedBlendRange = parameterization.m_blendRanges[m_selectedRangeIdx];
            auto pSource0 = m_sourceNodes[selectedBlendRange.m_sourceIdx0];
            auto pSource1 = m_sourceNodes[selectedBlendRange.m_sourceIdx1];
            KRG_ASSERT( pSource0 != nullptr && pSource1 != nullptr );

            //-------------------------------------------------------------------------

            // For synchronized updates, we can skip the updated for one of the sources if the weight is fully 0 or 1, since we can always update it to the correct time later.
            // Note: for additive blends we always need to update both nodes

            if ( m_blendWeight == 0.0f  )
            {
                result = pSource0->Update( context, updateRange );
            }
            else if ( m_blendWeight == 1.0f )
            {
                result = pSource1->Update( context, updateRange );
            }
            else // We need to update both sources
            {
                #if KRG_DEVELOPMENT_TOOLS
                context.GetRootMotionActionRecorder()->PushBlendHierarchyLevel();
                #endif

                UpdateResult const sourceResult0 = pSource0->Update( context, updateRange );
                UpdateResult const sourceResult1 = pSource1->Update( context, updateRange );
                if ( sourceResult0.HasRegisteredTasks() && sourceResult1.HasRegisteredTasks() )
                {
                    result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::BlendTask>( GetNodeIdx(), sourceResult0.m_taskIdx, sourceResult1.m_taskIdx, m_blendWeight, PoseBlendOptions::Interpolate );
                    result.m_rootMotionDelta = Blender::BlendRootMotionDeltas( sourceResult0.m_rootMotionDelta, sourceResult1.m_rootMotionDelta, m_blendWeight );

                    #if KRG_DEVELOPMENT_TOOLS
                    context.GetRootMotionActionRecorder()->RecordBlend( GetNodeIdx(), result.m_rootMotionDelta );
                    #endif
                }
                else
                {
                    result = ( sourceResult0.HasRegisteredTasks() ) ? sourceResult0 : sourceResult1;

                    #if KRG_DEVELOPMENT_TOOLS
                    context.GetRootMotionActionRecorder()->PopBlendHierarchyLevel();
                    #endif
                }

                result.m_sampledEventRange = CombineAndUpdateEvents( context, sourceResult0, sourceResult1, m_blendWeight );
            }

            // Update internal time and events
            SyncTrack const& syncTrack0 = pSource0->GetSyncTrack();
            SyncTrack const& syncTrack1 = pSource1->GetSyncTrack();
            m_blendedSyncTrack = SyncTrack( syncTrack0, syncTrack1, m_blendWeight );
            m_duration = SyncTrack::CalculateDurationSynchronized( pSource0->GetDuration(), pSource1->GetDuration(), syncTrack0.GetNumEvents(), syncTrack1.GetNumEvents(), m_blendedSyncTrack.GetNumEvents(), m_blendWeight );
            m_previousTime = GetSyncTrack().GetPercentageThrough( updateRange.m_startTime );
            m_currentTime = GetSyncTrack().GetPercentageThrough( updateRange.m_endTime );
        }

        return result;
    }

    //-------------------------------------------------------------------------

    void RangedBlendNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<RangedBlendNode>( nodePtrs, options );
        ParameterizedBlendNode::Settings::InstantiateNode( nodePtrs, GraphNode::Settings::InitOptions::OnlySetPointers );
    }

    //-------------------------------------------------------------------------

    void VelocityBlendNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<VelocityBlendNode>( nodePtrs, options );
        ParameterizedBlendNode::Settings::InstantiateNode( nodePtrs, GraphNode::Settings::InitOptions::OnlySetPointers );
    }

    void VelocityBlendNode::InitializeParameterization( GraphContext& context )
    {
        struct ParameterValue
        {
            NodeIndex       m_idx;
            float           m_value;
        };

        //-------------------------------------------------------------------------

        if ( IsValid() )
        {
            TInlineVector<ParameterValue, 10> sortedOptions;

            // Get all source velocities
            //-------------------------------------------------------------------------

            auto pSettings = GetSettings<VelocityBlendNode>();
            int32 const numSources = (int32) pSettings->m_sourceNodeIndices.size();
            for ( NodeIndex i = 0; i < numSources; i++ )
            {
                AnimationClip const* pAnimation = static_cast<AnimationClipReferenceNode const*>( m_sourceNodes[i] )->GetAnimation();
                KRG_ASSERT( pAnimation != nullptr );

                sortedOptions[i].m_idx = i;
                sortedOptions[i].m_value = pAnimation->GetAverageLinearVelocity();
            }

            // Sort the options based on value
            //-------------------------------------------------------------------------

            auto SortPredicate = [] ( ParameterValue const& a, ParameterValue const& b )
            {
                if ( a.m_value == b.m_value )
                {
                    return a.m_idx < b.m_idx;
                }

                return a.m_value < b.m_value;
            };

            eastl::sort( sortedOptions.begin(), sortedOptions.end(), SortPredicate );

            // Create the parameterization
            //-------------------------------------------------------------------------

            int32 const numBlendRanges = numSources - 1;
            m_parameterization.m_blendRanges.resize( numBlendRanges );
            for ( auto i = 0; i < numBlendRanges; i++ )
            {
                KRG_ASSERT( sortedOptions[i].m_value <= sortedOptions[i + 1].m_value );
                m_parameterization.m_blendRanges[i].m_sourceIdx0 = sortedOptions[i].m_idx;
                m_parameterization.m_blendRanges[i].m_sourceIdx1 = sortedOptions[i + 1].m_idx;
                m_parameterization.m_blendRanges[i].m_parameterValueRange = FloatRange( sortedOptions[i].m_value, sortedOptions[i + 1].m_value );
            }

            m_parameterization.m_parameterRange = FloatRange( sortedOptions.front().m_value, sortedOptions.back().m_value );
        }
    }

    void VelocityBlendNode::ShutdownParameterization( GraphContext& context )
    {
        m_parameterization.Reset();
    }
}