#include "Animation_EditorGraphNode_BoneMasks.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_BoneMasks.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void BoneMaskEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Bone Mask", GraphValueType::BoneMask, true );
    }

    GraphNodeIndex BoneMaskEditorNode::Compile( EditorGraphCompilationContext& context ) const
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

    void BoneMaskBlendEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::BoneMask, true );
        CreateInputPin( "Blend Weight", GraphValueType::Float );
        CreateInputPin( "Source", GraphValueType::BoneMask );
        CreateInputPin( "Target", GraphValueType::BoneMask );
    }

    GraphNodeIndex BoneMaskBlendEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        BoneMaskBlendNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<BoneMaskBlendNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

            pInputNode = GetConnectedInputNode<EditorGraphNode>( 1 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

            pInputNode = GetConnectedInputNode<EditorGraphNode>( 2 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

    void BoneMaskSelectorEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::BoneMask, true );
        CreateInputPin( "Parameter", GraphValueType::ID );
        CreateInputPin( "Default Mask", GraphValueType::BoneMask );
        CreateInputPin( "Mask 0", GraphValueType::BoneMask );
    }

    GraphNodeIndex BoneMaskSelectorEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        BoneMaskSelectorNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<BoneMaskSelectorNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            // Parameter
            //-------------------------------------------------------------------------

            auto pParameterNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pParameterNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pParameterNode->Compile( context );
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

            auto pDefaultMaskNode = GetConnectedInputNode<EditorGraphNode>( 1 );
            if ( pDefaultMaskNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pDefaultMaskNode->Compile( context );
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
                auto pOptionNode = GetConnectedInputNode<EditorGraphNode>( i );
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