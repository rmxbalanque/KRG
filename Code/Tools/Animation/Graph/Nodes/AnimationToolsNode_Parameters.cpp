#include "AnimationToolsNode_Parameters.h"
#include "AnimationToolsNode_Results.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Graph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Parameters.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ControlParameterToolsNode::ControlParameterToolsNode( String const& name, NodeValueType type )
        : ToolsNode()
        , m_name( name )
    {
        KRG_ASSERT( !m_name.empty() );
        CreateOutputPin( "Value", type );
    }

    NodeIndex ControlParameterToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        switch ( GetValueType() )
        {
            case NodeValueType::Bool:
            {
                ControlParameterBoolNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterBoolNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case NodeValueType::ID:
            {
                ControlParameterIDNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterIDNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case NodeValueType::Int:
            {
                ControlParameterIntNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterIntNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case NodeValueType::Float:
            {
                ControlParameterFloatNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterFloatNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case NodeValueType::Vector:
            {
                ControlParameterVectorNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterVectorNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case NodeValueType::Target:
            {
                ControlParameterTargetNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterTargetNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case NodeValueType::BoneMask:
            {
                ControlParameterBoneMaskNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterBoneMaskNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;
        }

        KRG_UNREACHABLE_CODE();
        return InvalidIndex;
    }

    //-------------------------------------------------------------------------

    ControlParameterReferenceToolsNode::ControlParameterReferenceToolsNode( ControlParameterToolsNode const* pParameter )
        : ToolsNode()
        , m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
        CreateOutputPin( "Value", pParameter->GetValueType() );
    }

    NodeIndex ControlParameterReferenceToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        return m_pParameter->Compile( context );
    }

    //-------------------------------------------------------------------------

    VirtualParameterToolsNode::VirtualParameterToolsNode( String const& name, NodeValueType type )
        : ToolsNode()
        , m_name( name )
    {
        KRG_ASSERT( !m_name.empty() );
        CreateOutputPin( "Value", type );

        auto pParameterGraph = KRG::New<ToolsGraph>( GraphType::ValueTree );
        pParameterGraph->CreateNode<ResultToolsNode>( type );
        m_pSecondaryGraph = pParameterGraph;
    }

    NodeIndex VirtualParameterToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        auto const resultNodes = m_pSecondaryGraph->FindAllNodesOfType<ResultToolsNode>( false );
        KRG_ASSERT( resultNodes.size() == 1 );
        return GetConnectedInputNode<ToolsNode>( 0 )->Compile( context );
    }

    //-------------------------------------------------------------------------

    VirtualParameterReferenceToolsNode::VirtualParameterReferenceToolsNode( VirtualParameterToolsNode const* pParameter )
        : ToolsNode()
        , m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
        CreateOutputPin( "Value", pParameter->GetValueType() );
    }

    NodeIndex VirtualParameterReferenceToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        return m_pParameter->Compile( context );
    }
}