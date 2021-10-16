#include "AnimationToolsNode_BoneMasks.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_BoneMasks.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_BoneMaskNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Bone Mask", ValueType::BoneMask, true );
    }

    NodeIndex Tools_BoneMaskNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        BoneMaskNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<BoneMaskNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_rootMotionWeight = m_rootMotionWeight;
            for ( auto const& weight : m_weights )
            {
                pSettings->m_weights.push_back( { weight.m_boneID, weight.m_weight } );
            }
        }
        return pSettings->m_nodeIdx;
    }
    
    //-------------------------------------------------------------------------

    void Tools_BoneMaskBlendNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::BoneMask, true );
        CreateInputPin( "Blend Weight", ValueType::Float );
        CreateInputPin( "Source", ValueType::BoneMask );
        CreateInputPin( "Target", ValueType::BoneMask );
    }

    NodeIndex Tools_BoneMaskBlendNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        BoneMaskBlendNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<BoneMaskBlendNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_blendWeightValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 1 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_sourceMaskNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 2 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_targetMaskNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_BoneMaskSelectorNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::BoneMask, true );
        CreateInputPin( "Parameter", ValueType::ID );
        CreateInputPin( "Default Mask", ValueType::BoneMask );
        CreateInputPin( "Mask 0", ValueType::BoneMask );
    }

    NodeIndex Tools_BoneMaskSelectorNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        BoneMaskSelectorNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<BoneMaskSelectorNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            // Parameter
            //-------------------------------------------------------------------------

            auto pParameterNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pParameterNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pParameterNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_parameterValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected parameter pin on selector node!" );
                return false;
            }

            // Default Mask
            //-------------------------------------------------------------------------

            auto pDefaultMaskNode = GetConnectedInputNode<Tools_GraphNode>( 1 );
            if ( pDefaultMaskNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pDefaultMaskNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_defaultMaskNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected default  input pin on selector node!" );
                return false;
            }

            // Dynamic Options
            //-------------------------------------------------------------------------

            int32 const numInputs = GetNumInputPins();

            for ( auto i = 2; i < numInputs; i++ )
            {
                auto pOptionNode = GetConnectedInputNode<Tools_GraphNode>( i );
                if ( pOptionNode != nullptr )
                {
                    auto compiledNodeIdx = pOptionNode->Compile( context );
                    if ( compiledNodeIdx != InvalidIndex )
                    {
                        pSettings->m_maskNodeIndices.emplace_back( compiledNodeIdx );
                    }
                    else
                    {
                        return InvalidIndex;
                    }
                }
                else
                {
                    context.LogError( this, "Disconnected option pin on selector node!" );
                    return InvalidIndex;
                }
            }

            //-------------------------------------------------------------------------

            pSettings->m_switchDynamically = m_switchDynamically;
            pSettings->m_blendTime = m_blendTime;

            pSettings->m_parameterValues.clear();
            pSettings->m_parameterValues.insert( pSettings->m_parameterValues.begin(), m_parameterValues.begin(), m_parameterValues.end() );
        }
        return pSettings->m_nodeIdx;
    }
}