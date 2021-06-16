#include "AnimationToolsNode_Blends.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ParameterizedBlendToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );

        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Parameter", NodeValueType::Float );
        CreateInputPin( "Input", NodeValueType::Pose );
        CreateInputPin( "Input", NodeValueType::Pose );
    }

    bool ParameterizedBlendToolsNode::CompileParameterAndSourceNodes( ToolsGraphCompilationContext& context, ParameterizedBlendNode::Settings* pSettings ) const
    {
        // Parameter
        //-------------------------------------------------------------------------

        auto pParameterNode = GetConnectedInputNode<FlowToolsNode>( 0 );
        if ( pParameterNode != nullptr )
        {
            NodeIndex const compiledNodeIdx = pParameterNode->Compile( context );
            if ( compiledNodeIdx != InvalidIndex )
            {
                pSettings->m_inputParameterValueNodeIdx = compiledNodeIdx;
            }
            else
            {
                return false;
            }
        }
        else
        {
            context.LogError( this, "Disconnected parameter pin on blend node!" );
            return false;
        }

        // Sources
        //-------------------------------------------------------------------------

        for ( auto i = 1; i < GetNumInputPins(); i++ )
        {
            auto pSourceNode = GetConnectedInputNode<FlowToolsNode>( i );
            if ( pSourceNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pSourceNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_sourceNodeIndices.emplace_back( compiledNodeIdx );
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected source pin on blend node!" );
                return false;
            }
        }

        // Common Settings
        //-------------------------------------------------------------------------

        pSettings->m_isSynchronized = m_isSynchronized;

        return true;
    }

    //-------------------------------------------------------------------------

    void RangedBlendToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        ParameterizedBlendToolsNode::Initialize( pParent );

        m_parameterValues.emplace_back( 0.0f );
        m_parameterValues.emplace_back( 1.0f );
    }

    NodeIndex RangedBlendToolsNode::Compile( ToolsGraphCompilationContext & context ) const
    {
        RangedBlendNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<RangedBlendNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            if ( !CompileParameterAndSourceNodes( context, pSettings ) )
            {
                return InvalidIndex;
            }

            // Create parameterization
            //-------------------------------------------------------------------------

            KRG_ASSERT( m_parameterValues.size() == ( GetNumInputPins() - 1 ) );
            TInlineVector<float, 5> values( m_parameterValues.begin(), m_parameterValues.end() );
            pSettings->m_parameterization = ParameterizedBlendNode::Parameterization::CreateParameterization( values );
        }

        return pSettings->m_nodeIdx;
    }

    bool RangedBlendToolsNode::DrawPinControls( GraphEditor::Flow::Pin const& pin )
    {
        // Add parameter value input field
        if ( pin.IsInputPin() && pin.m_type == (uint32) NodeValueType::Pose )
        {
            int32 const pinIdx = GetInputPinIndex( pin.m_ID );
            int32 const parameterIdx = pinIdx - 1;
            KRG_ASSERT( parameterIdx >= 0 && parameterIdx < m_parameterValues.size() );

            ImGui::PushID( &m_parameterValues[parameterIdx] );
            ImGui::SetNextItemWidth( 50 );
            ImGui::InputFloat( "##parameter", &m_parameterValues[parameterIdx], 0.0f, 0.0f, "%.2f" );
            ImGui::PopID();

            return true;
        }

        return false;
    }

    void RangedBlendToolsNode::OnDynamicPinCreation( UUID pinID )
    {
        m_parameterValues.emplace_back( 0.0f );
    }

    void RangedBlendToolsNode::OnDynamicPinDestruction( UUID pinID )
    {
        int32 const pinToBeRemovedIdx = GetInputPinIndex( pinID );
        KRG_ASSERT( pinToBeRemovedIdx != InvalidIndex );

        int32 const parameterIdx = pinToBeRemovedIdx - 1;
        KRG_ASSERT( parameterIdx >= 0 && parameterIdx < m_parameterValues.size() );
        m_parameterValues.erase( m_parameterValues.begin() + parameterIdx );
    }

    //-------------------------------------------------------------------------

    NodeIndex VelocityBlendToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        VelocityBlendNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<VelocityBlendNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            if ( !CompileParameterAndSourceNodes( context, pSettings ) )
            {
                return InvalidIndex;
            }
        }

        return pSettings->m_nodeIdx;
    }
}