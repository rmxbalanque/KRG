#include "AnimationToolsNode_Parameters.h"
#include "AnimationToolsNode_Results.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"
#include "Tools/Animation/Graph/AnimationGraphTools_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Parameters.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ControlParameterToolsNode::ControlParameterToolsNode( String const& name, NodeValueType type )
        : m_name( name )
        , m_type( type )
    {
        KRG_ASSERT( !m_name.empty() );
    }

    void ControlParameterToolsNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        FlowToolsNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_type);
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
        : m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
    }

    void ControlParameterReferenceToolsNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        FlowToolsNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_pParameter->GetValueType() );
    }

    NodeIndex ControlParameterReferenceToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        return m_pParameter->Compile( context );
    }

    //-------------------------------------------------------------------------

    VirtualParameterToolsNode::VirtualParameterToolsNode( String const& name, NodeValueType type )
        : m_name( name )
        , m_type( type )
    {
        KRG_ASSERT( !m_name.empty() );
    }

    void VirtualParameterToolsNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        FlowToolsNode::Initialize( pParentGraph );

        CreateOutputPin( "Value", m_type );

        auto pParameterGraph = KRG::New<FlowToolGraph>( GraphType::ValueTree );
        pParameterGraph->CreateNode<ResultToolsNode>( m_type );
        SetSecondaryGraph( pParameterGraph );
    }

    NodeIndex VirtualParameterToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        auto const resultNodes = GetSecondaryGraph()->FindAllNodesOfType<ResultToolsNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        return GetConnectedInputNode<FlowToolsNode>( 0 )->Compile( context );
    }

    //-------------------------------------------------------------------------

    VirtualParameterReferenceToolsNode::VirtualParameterReferenceToolsNode( VirtualParameterToolsNode const* pParameter )
        : m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
    }

    void VirtualParameterReferenceToolsNode::Initialize( GraphEditor::BaseGraph* pParentGraph )
    {
        FlowToolsNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_pParameter->GetValueType() );
    }

    NodeIndex VirtualParameterReferenceToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        return m_pParameter->Compile( context );
    }
}