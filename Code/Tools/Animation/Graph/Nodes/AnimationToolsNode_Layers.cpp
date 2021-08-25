#include "AnimationToolsNode_Layers.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"
#include "Tools/Animation/Graph/AnimationGraphTools_FlowGraph.h"
#include "Tools/Animation/Graph/StateMachine/AnimationToolsNode_StateMachine.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Layers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void LayerNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );

        CreateOutputPin( "Layer", NodeValueType::Unknown );
        CreateInputPin( "State Machine", NodeValueType::Pose );
    }

    bool LayerNode::IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const
    {
        return IsOfType<StateMachineToolsNode>( pOutputPinNode );
    }

    //-------------------------------------------------------------------------

    void LayerToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );

        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Base Node", NodeValueType::Pose );
        CreateInputPin( "Layer 0", NodeValueType::Unknown );
    }

    bool LayerToolsNode::IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const
    {
        int32 const pinIdx = GetInputPinIndex( inputPinID );
        if ( pinIdx > 0 )
        {
            return IsOfType<LayerNode>( pOutputPinNode );
        }

        return FlowToolsNode::IsValidConnection( inputPinID, pOutputPinNode, outputPinID );
    }

    InlineString<100> LayerToolsNode::GetNewDynamicInputPinName() const
    {
        int32 const numOptions = GetNumInputPins();
        InlineString<100> pinName;
        pinName.sprintf( "Layer %d", numOptions - 2 );
        return pinName;
    }

    void LayerToolsNode::OnDynamicPinDestruction( UUID pinID )
    {
        int32 const numOptions = GetNumInputPins();

        int32 const pintoBeRemovedIdx = GetInputPinIndex( pinID );
        KRG_ASSERT( pintoBeRemovedIdx != InvalidIndex );

        // Rename all pins
        //-------------------------------------------------------------------------

        int32 newPinIdx = 1;
        for ( auto i = 2; i < numOptions; i++ )
        {
            if ( i == pintoBeRemovedIdx )
            {
                continue;
            }

            InlineString<100> newPinName;
            newPinName.sprintf( "Layer %d", newPinIdx );

            GetInputPin( i )->m_name = newPinName;
            newPinIdx++;
        }
    }

    NodeIndex LayerToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        LayerBlendNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<LayerBlendNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            // Compile Base
            //-------------------------------------------------------------------------

            auto pBaseNode = GetConnectedInputNode<FlowToolsNode>( 0 );
            if ( pBaseNode != nullptr )
            {
                auto compiledNodeIdx = pBaseNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_baseNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return InvalidIndex;
                }
            }
            else
            {
                context.LogError( this, "Disconnected base node pin on layer node!" );
                return InvalidIndex;
            }

            pSettings->m_onlySampleBaseRootMotion = m_onlySampleBaseRootMotion;

            // Compile Layers
            //-------------------------------------------------------------------------

            bool atLeastOneLayerCompiled = false;

            for ( auto i = 1; i < GetNumInputPins(); i++ )
            {
                auto pLayerSettingsNode = GetConnectedInputNode<LayerNode>( i );
                if ( pLayerSettingsNode != nullptr )
                {
                    auto& layerSettings = pSettings->m_layerSettings.emplace_back();

                    layerSettings.m_isSynchronized = pLayerSettingsNode->m_isSynchronized;
                    layerSettings.m_ignoreEvents = pLayerSettingsNode->m_ignoreEvents;
                    layerSettings.m_blendOptions = pLayerSettingsNode->m_blendOptions;

                    // Compile layer state machine
                    auto pLayerStateMachineNode = pLayerSettingsNode->GetConnectedInputNode<StateMachineToolsNode>( 0 );
                    if ( pLayerStateMachineNode != nullptr )
                    {
                        auto compiledStateMachineNodeIdx = pLayerStateMachineNode->Compile( context );
                        if ( compiledStateMachineNodeIdx != InvalidIndex )
                        {
                            layerSettings.m_layerNodeIdx = compiledStateMachineNodeIdx;
                            atLeastOneLayerCompiled = true;
                        }
                        else
                        {
                            return InvalidIndex;
                        }
                    }
                    else
                    {
                        context.LogError( this, "Disconnected base node pin on layer node!" );
                        return InvalidIndex;
                    }
                }
            }

            if ( !atLeastOneLayerCompiled )
            {
                context.LogError( this, "No connected layers on layer node!" );
                return InvalidIndex;
            }
        }
        return pSettings->m_nodeIdx;
    }
}