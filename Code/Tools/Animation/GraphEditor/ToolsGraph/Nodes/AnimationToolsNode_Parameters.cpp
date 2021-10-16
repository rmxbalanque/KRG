#include "AnimationToolsNode_Parameters.h"
#include "AnimationToolsNode_Results.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Parameters.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    Tools_ControlParameterNode::Tools_ControlParameterNode( String const& name, ValueType type )
        : m_name( name )
        , m_type( type )
    {
        KRG_ASSERT( !m_name.empty() );
    }

    void Tools_ControlParameterNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        Tools_GraphNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_type, true );
    }

    NodeIndex Tools_ControlParameterNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        switch ( GetValueType() )
        {
            case ValueType::Bool:
            {
                ControlParameterBoolNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterBoolNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case ValueType::ID:
            {
                ControlParameterIDNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterIDNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case ValueType::Int:
            {
                ControlParameterIntNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterIntNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case ValueType::Float:
            {
                ControlParameterFloatNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterFloatNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case ValueType::Vector:
            {
                ControlParameterVectorNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterVectorNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case ValueType::Target:
            {
                ControlParameterTargetNode::Settings* pSettings = nullptr;
                if ( context.GetSettings<ControlParameterTargetNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
                {
                    pSettings->m_nodeID = StringID( m_name );
                }
                return pSettings->m_nodeIdx;
            }
            break;

            case ValueType::BoneMask:
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

    Tools_ControlParameterReferenceNode::Tools_ControlParameterReferenceNode( Tools_ControlParameterNode const* pParameter )
        : m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
    }

    void Tools_ControlParameterReferenceNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        Tools_GraphNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_pParameter->GetValueType(), true );
    }

    NodeIndex Tools_ControlParameterReferenceNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        return m_pParameter->Compile( context );
    }

    //-------------------------------------------------------------------------

    Tools_VirtualParameterNode::Tools_VirtualParameterNode( String const& name, ValueType type )
        : m_name( name )
        , m_type( type )
    {
        KRG_ASSERT( !m_name.empty() );
    }

    void Tools_VirtualParameterNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        Tools_GraphNode::Initialize( pParentGraph );

        CreateOutputPin( "Value", m_type, true );

        auto pParameterGraph = KRG::New<FlowGraph>( GraphType::ValueTree );
        pParameterGraph->CreateNode<Tools_ResultNode>( m_type );
        SetSecondaryGraph( pParameterGraph );
    }

    NodeIndex Tools_VirtualParameterNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        auto const resultNodes = GetSecondaryGraph()->FindAllNodesOfType<Tools_ResultNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        return GetConnectedInputNode<Tools_GraphNode>( 0 )->Compile( context );
    }

    //-------------------------------------------------------------------------

    Tools_VirtualParameterReferenceNode::Tools_VirtualParameterReferenceNode( Tools_VirtualParameterNode const* pParameter )
        : m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
    }

    void Tools_VirtualParameterReferenceNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        Tools_GraphNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_pParameter->GetValueType(), true );
    }

    NodeIndex Tools_VirtualParameterReferenceNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        return m_pParameter->Compile( context );
    }
}