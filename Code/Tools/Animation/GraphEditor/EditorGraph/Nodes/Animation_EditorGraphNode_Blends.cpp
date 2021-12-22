#include "Animation_EditorGraphNode_Blends.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ParameterizedBlendEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );

        CreateOutputPin( "Pose", ValueType::Pose );
        CreateInputPin( "Parameter", ValueType::Float );
        CreateInputPin( "Input", ValueType::Pose );
        CreateInputPin( "Input", ValueType::Pose );
    }

    bool ParameterizedBlendEditorNode::CompileParameterAndSourceNodes( EditorGraphCompilationContext& context, ParameterizedBlendNode::Settings* pSettings ) const
    {
        // Parameter
        //-------------------------------------------------------------------------

        auto pParameterNode = GetConnectedInputNode<EditorGraphNode>( 0 );
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
            auto pSourceNode = GetConnectedInputNode<EditorGraphNode>( i );
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

    void RangedBlendEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        ParameterizedBlendEditorNode::Initialize( pParent );

        m_parameterValues.emplace_back( 0.0f );
        m_parameterValues.emplace_back( 1.0f );
    }

    NodeIndex RangedBlendEditorNode::Compile( EditorGraphCompilationContext & context ) const
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

    bool RangedBlendEditorNode::DrawPinControls( VisualGraph::Flow::Pin const& pin )
    {
        // Add parameter value input field
        if ( pin.IsInputPin() && pin.m_type == (uint32) ValueType::Pose )
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

    void RangedBlendEditorNode::OnDynamicPinCreation( UUID pinID )
    {
        m_parameterValues.emplace_back( 0.0f );
    }

    void RangedBlendEditorNode::OnDynamicPinDestruction( UUID pinID )
    {
        int32 const pinToBeRemovedIdx = GetInputPinIndex( pinID );
        KRG_ASSERT( pinToBeRemovedIdx != InvalidIndex );

        int32 const parameterIdx = pinToBeRemovedIdx - 1;
        KRG_ASSERT( parameterIdx >= 0 && parameterIdx < m_parameterValues.size() );
        m_parameterValues.erase( m_parameterValues.begin() + parameterIdx );
    }

    //-------------------------------------------------------------------------

    NodeIndex VelocityBlendEditorNode::Compile( EditorGraphCompilationContext& context ) const
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