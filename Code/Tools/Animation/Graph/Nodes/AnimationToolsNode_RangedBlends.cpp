#include "AnimationToolsNode_RangedBlends.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ParameterizedBlendToolsNode::ParameterizedBlendToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Parameter", NodeValueType::Float );
        CreateInputPin( "Option", NodeValueType::Pose );
        CreateInputPin( "Option", NodeValueType::Pose );
    }

    bool ParameterizedBlendToolsNode::CompileParameterAndSourceNodes( ToolsGraphCompilationContext& context, ParameterizedBlendNode::Settings* pSettings ) const
    {
        // Parameter
        //-------------------------------------------------------------------------

        auto pParameterNode = GetConnectedInputNode<ToolsNode>( 0 );
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

        for ( auto i = 2; i < GetNumInputPins(); i++ )
        {
            auto pSourceNode = GetConnectedInputNode<ToolsNode>( i );
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

            KRG_ASSERT( m_parameterValues.size() == ( GetNumInputPins() - 2 ) );
            TInlineVector<float, 5> values( m_parameterValues.begin(), m_parameterValues.end() );
            pSettings->m_parameterization = ParameterizedBlendNode::Parameterization::CreateParameterization( pSettings->m_sourceNodeIndices, values );
        }

        return pSettings->m_nodeIdx;
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