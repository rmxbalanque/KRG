#include "AnimationToolsNode_Layers.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/Nodes/AnimationToolsNode_StateMachine.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Layers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_LayerSettingsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );

        CreateOutputPin( "Layer", ValueType::Unknown );
        CreateInputPin( "State Machine", ValueType::Pose );
    }

    bool Tools_LayerSettingsNode::IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const
    {
        return IsOfType<Tools_StateMachineNode>( pOutputPinNode );
    }

    //-------------------------------------------------------------------------

    void Tools_LayerNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );

        CreateOutputPin( "Pose", ValueType::Pose );
        CreateInputPin( "Base Node", ValueType::Pose );
        CreateInputPin( "Layer 0", ValueType::Unknown );
    }

    bool Tools_LayerNode::IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const
    {
        int32 const pinIdx = GetInputPinIndex( inputPinID );
        if ( pinIdx > 0 )
        {
            return IsOfType<Tools_LayerSettingsNode>( pOutputPinNode );
        }

        return Tools_GraphNode::IsValidConnection( inputPinID, pOutputPinNode, outputPinID );
    }

    InlineString<100> Tools_LayerNode::GetNewDynamicInputPinName() const
    {
        int32 const numOptions = GetNumInputPins();
        InlineString<100> pinName;
        pinName.sprintf( "Layer %d", numOptions - 2 );
        return pinName;
    }

    void Tools_LayerNode::OnDynamicPinDestruction( UUID pinID )
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

    NodeIndex Tools_LayerNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        LayerBlendNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<LayerBlendNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            // Compile Base
            //-------------------------------------------------------------------------

            auto pBaseNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
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
                auto pLayerSettingsNode = GetConnectedInputNode<Tools_LayerSettingsNode>( i );
                if ( pLayerSettingsNode != nullptr )
                {
                    auto& layerSettings = pSettings->m_layerSettings.emplace_back();

                    layerSettings.m_isSynchronized = pLayerSettingsNode->m_isSynchronized;
                    layerSettings.m_ignoreEvents = pLayerSettingsNode->m_ignoreEvents;
                    layerSettings.m_blendOptions = pLayerSettingsNode->m_blendOptions;

                    // Compile layer state machine
                    auto pLayerStateMachineNode = pLayerSettingsNode->GetConnectedInputNode<Tools_StateMachineNode>( 0 );
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