#include "Animation_EditorGraph_FlowGraph.h"
#include "Animation_EditorGraph_Variations.h"
#include "Animation_EditorGraph_Compilation.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Parameters.h"

//-------------------------------------------------------------------------
// CORE EDITOR NODES
//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    static void TraverseHierarchy( VisualGraph::BaseNode const* pNode, TVector<VisualGraph::BaseNode const*>& nodePath )
    {
        KRG_ASSERT( pNode != nullptr );
        nodePath.emplace_back( pNode );

        if ( pNode->HasParentGraph() && !pNode->GetParentGraph()->IsRootGraph() )
        {
            TraverseHierarchy( pNode->GetParentGraph()->GetParentNode(), nodePath );
        }
    }

    //-------------------------------------------------------------------------

    void EditorGraphNode::DrawExtraControls( VisualGraph::DrawContext const& ctx )
    {
        GraphNodeIndex runtimeNodeIdx = InvalidIndex;
        auto pDebugContext = reinterpret_cast<DebugContext*>( ctx.m_pUserContext );
        if ( pDebugContext != nullptr )
        {
            runtimeNodeIdx = pDebugContext->GetRuntimeGraphNodeIndex( GetID() );

            // Some nodes dont have runtime representations
            if ( runtimeNodeIdx == InvalidIndex )
            {
                return;
            }
        }

        //-------------------------------------------------------------------------
        // Draw Pose Node
        //-------------------------------------------------------------------------

        if ( GetValueType() == GraphValueType::Pose )
        {
            if ( pDebugContext != nullptr && pDebugContext->IsNodeActive( runtimeNodeIdx ) )
            {
                PoseNodeDebugInfo const debugInfo = pDebugContext->GetPoseNodeDebugInfo( runtimeNodeIdx );
                DrawPoseNodeDebugInfo( ctx, GetSize().x, debugInfo );
            }
            else
            {
                DrawEmptyPoseNodeDebugInfo( ctx, GetSize().x );
            }
        }

        //-------------------------------------------------------------------------
        // Draw Value Node
        //-------------------------------------------------------------------------

        else
        {
            if ( pDebugContext != nullptr && pDebugContext->IsNodeActive( runtimeNodeIdx ) )
            {
                if ( HasOutputPin() )
                {
                    GraphValueType const valueType = GetValueType();
                    switch ( valueType )
                    {
                        case GraphValueType::Bool:
                        {
                            auto const value = pDebugContext->GetRuntimeNodeValue<bool>( runtimeNodeIdx );
                            ImGui::Text( value ? "True" : "False" );
                        }
                        break;

                        case GraphValueType::ID:
                        {
                            auto const value = pDebugContext->GetRuntimeNodeValue<StringID>( runtimeNodeIdx );
                            if ( value.IsValid() )
                            {
                                ImGui::Text( value.c_str() );
                            }
                            else
                            {
                                ImGui::Text( "Invalid" );
                            }
                        }
                        break;

                        case GraphValueType::Int:
                        {
                            auto const value = pDebugContext->GetRuntimeNodeValue<int32>( runtimeNodeIdx );
                            ImGui::Text( "%d", value );
                        }
                        break;

                        case GraphValueType::Float:
                        {
                            auto const value = pDebugContext->GetRuntimeNodeValue<float>( runtimeNodeIdx );
                            ImGui::Text( "%.3f", value );
                        }
                        break;

                        case GraphValueType::Vector:
                        {
                            auto const value = pDebugContext->GetRuntimeNodeValue<Vector>( runtimeNodeIdx );
                            ImGuiX::DisplayVector4( value );
                        }
                        break;

                        case GraphValueType::Target:
                        {
                            auto const value = pDebugContext->GetRuntimeNodeValue<Target>( runtimeNodeIdx );
                            if ( value.IsBoneTarget() )
                            {
                                if ( value.GetBoneID().IsValid() )
                                {
                                    ImGui::Text( value.GetBoneID().c_str() );
                                }
                                else
                                {
                                    ImGui::Text( "Invalid" );
                                }
                            }
                            else
                            {
                                ImGui::Text( "Target TODO" );
                            }
                        }
                        break;

                        case GraphValueType::BoneMask:
                        case GraphValueType::Pose:
                        default:
                        break;
                    }
                }
            }
            else
            {
                ImGui::NewLine();
            }
        }

        //-------------------------------------------------------------------------

        DrawInfoText( ctx );
    }

    //-------------------------------------------------------------------------

    bool DataSlotEditorNode::AreSlotValuesValid() const
    {
        if ( m_defaultResourceID.GetResourceTypeID() != GetSlotResourceTypeID() )
        {
            return false;
        }

        for ( auto const& variation : m_overrides )
        {
            if ( !variation.m_variationID.IsValid() )
            {
                return false;
            }

            if ( variation.m_resourceID.GetResourceTypeID() != GetSlotResourceTypeID() )
            {
                return false;
            }
        }

        return true;
    }

    ResourceID DataSlotEditorNode::GetValue( VariationHierarchy const& variationHierarchy, StringID variationID ) const
    {
        KRG_ASSERT( variationHierarchy.IsValidVariation( variationID ) );

        if ( variationID == GraphVariation::DefaultVariationID )
        {
            return m_defaultResourceID;
        }

        //-------------------------------------------------------------------------

        ResourceID resourceID;

        auto TryGetResourceID = [this, &resourceID] ( StringID variationID )
        {
            if ( variationID == GraphVariation::DefaultVariationID )
            {
                resourceID = m_defaultResourceID;
                return true;
            }

            for ( auto const& variation : m_overrides )
            {
                if ( variation.m_variationID == variationID )
                {
                    resourceID = variation.m_resourceID;
                    return true;
                }
            }

            return false;
        };

        //-------------------------------------------------------------------------

        // Try get the resource ID for this variation
        if ( TryGetResourceID( variationID ) )
        {
            return resourceID;
        }

        // Go up the hierarchy and return the first if a override exists
        StringID parentVariationID = variationHierarchy.GetParentVariationID( variationID );
        while ( parentVariationID.IsValid() )
        {
            if ( TryGetResourceID( parentVariationID ) )
            {
                break;
            }

            parentVariationID = variationHierarchy.GetParentVariationID( parentVariationID );
        }

        return resourceID;
    }

    ResourceID* DataSlotEditorNode::GetOverrideValueForVariation( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() );

        if ( variationID == GraphVariation::DefaultVariationID )
        {
            return &m_defaultResourceID;
        }

        for ( auto& variation : m_overrides )
        {
            if ( variation.m_variationID == variationID )
            {
                return &variation.m_resourceID;
            }
        }

        return nullptr;
    }

    void DataSlotEditorNode::SetOverrideValueForVariation( StringID variationID, ResourceID const& resourceID )
    {
        KRG_ASSERT( variationID.IsValid() );
        KRG_ASSERT( !resourceID.IsValid() || resourceID.GetResourceTypeID() == GetSlotResourceTypeID() );

        if ( variationID == GraphVariation::DefaultVariationID )
        {
            m_defaultResourceID = resourceID;
            return;
        }

        //-------------------------------------------------------------------------

        KRG_ASSERT( HasOverrideForVariation( variationID ) );

        for ( auto& variation : m_overrides )
        {
            if ( variation.m_variationID == variationID )
            {
                variation.m_resourceID = resourceID;
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    void DataSlotEditorNode::CreateOverride( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() && variationID != GraphVariation::DefaultVariationID );
        KRG_ASSERT( !HasOverrideForVariation( variationID ) );

        auto& createdOverride = m_overrides.emplace_back();
        createdOverride.m_variationID = variationID;
    }

    void DataSlotEditorNode::RenameOverride( StringID oldVariationID, StringID newVariationID )
    {
        KRG_ASSERT( oldVariationID.IsValid() && newVariationID.IsValid() );
        KRG_ASSERT( oldVariationID != GraphVariation::DefaultVariationID && newVariationID != GraphVariation::DefaultVariationID );

        for ( auto& overrideValue : m_overrides )
        {
            if ( overrideValue.m_variationID == oldVariationID )
            {
                overrideValue.m_variationID = newVariationID;
            }
        }
    }

    void DataSlotEditorNode::RemoveOverride( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() && variationID != GraphVariation::DefaultVariationID );

        for ( auto iter = m_overrides.begin(); iter != m_overrides.end(); ++iter )
        {
            if ( iter->m_variationID == variationID )
            {
                m_overrides.erase_unsorted( iter );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    //-------------------------------------------------------------------------

    ResultEditorNode::ResultEditorNode( GraphValueType valueType )
        : m_valueType( valueType )
    {}

    void ResultEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateInputPin( "Out", m_valueType );
    }

    GraphNodeIndex ResultEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        // Get connected node and compile it
        auto pConnectedNode = GetConnectedInputNode<EditorGraphNode>( 0 );
        if ( pConnectedNode != nullptr )
        {
            return pConnectedNode->Compile( context );
        }

        context.LogError( this, "Result node not connected" );
        return InvalidIndex;
    }
}

//-------------------------------------------------------------------------
// PARAMETERS
//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void ParameterReferenceEditorNode::RefreshParameterReferences( VisualGraph::BaseGraph* pRootGraph )
    {
        KRG_ASSERT( pRootGraph != nullptr && pRootGraph->IsRootGraph() );
        auto const controlParameters = pRootGraph->FindAllNodesOfType<ControlParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Exact );
        auto const virtualParameters = pRootGraph->FindAllNodesOfType<VirtualParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Exact );

        //-------------------------------------------------------------------------

        TInlineVector<ParameterReferenceEditorNode*, 10> invalidReferenceNodes;
        auto parameterReferenceNodes = pRootGraph->FindAllNodesOfType<ParameterReferenceEditorNode>( VisualGraph::SearchMode::Recursive, VisualGraph::SearchTypeMatch::Exact );
        for ( auto pReferenceNode : parameterReferenceNodes )
        {
            EditorGraphNode* pFoundParameterNode = nullptr;

            for ( auto pParameter : controlParameters )
            {
                if ( pParameter->GetID() == pReferenceNode->GetReferencedParameterID() )
                {
                    pFoundParameterNode = pParameter;
                    break;
                }
            }

            if ( pFoundParameterNode == nullptr )
            {
                for ( auto pParameter : virtualParameters )
                {
                    if ( pParameter->GetID() == pReferenceNode->GetReferencedParameterID() )
                    {
                        pFoundParameterNode = pParameter;
                        break;
                    }
                }
            }

            if ( pFoundParameterNode != nullptr && ( pReferenceNode->GetParameterValueType() == pFoundParameterNode->GetValueType() ) )
            {
                pReferenceNode->m_pParameter = pFoundParameterNode;
            }
            else // flag invalid references nodes for destruction
            {
                invalidReferenceNodes.emplace_back( pReferenceNode );
            }
        }

        // Destroy any invalid reference nodes
        //-------------------------------------------------------------------------

        if ( !invalidReferenceNodes.empty() )
        {
            VisualGraph::ScopedGraphModification const sgm( pRootGraph );
            for ( auto pInvalidNode : invalidReferenceNodes )
            {
                pInvalidNode->Destroy();
            }
        }
    }

    //-------------------------------------------------------------------------

    ControlParameterEditorNode::ControlParameterEditorNode( String const& name, GraphValueType type )
        : m_name( name )
        , m_type( type )
    {
        KRG_ASSERT( !m_name.empty() );
    }

    void ControlParameterEditorNode::Initialize( VisualGraph::BaseGraph* pParentGraph )
    {
        EditorGraphNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_type, true );
    }

    GraphNodeIndex ControlParameterEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        switch ( GetValueType() )
        {
            case GraphValueType::Bool:
            {
                GraphNodes::ControlParameterBoolNode::Settings* pSettings = nullptr;
                context.GetSettings<GraphNodes::ControlParameterBoolNode>( this, pSettings );
                return pSettings->m_nodeIdx;
            }
            break;

            case GraphValueType::ID:
            {
                GraphNodes::ControlParameterIDNode::Settings* pSettings = nullptr;
                context.GetSettings<GraphNodes::ControlParameterIDNode>( this, pSettings );
                return pSettings->m_nodeIdx;
            }
            break;

            case GraphValueType::Int:
            {
                GraphNodes::ControlParameterIntNode::Settings* pSettings = nullptr;
                context.GetSettings<GraphNodes::ControlParameterIntNode>( this, pSettings );
                return pSettings->m_nodeIdx;
            }
            break;

            case GraphValueType::Float:
            {
                GraphNodes::ControlParameterFloatNode::Settings* pSettings = nullptr;
                context.GetSettings<GraphNodes::ControlParameterFloatNode>( this, pSettings );
                return pSettings->m_nodeIdx;
            }
            break;

            case GraphValueType::Vector:
            {
                GraphNodes::ControlParameterVectorNode::Settings* pSettings = nullptr;
                context.GetSettings<GraphNodes::ControlParameterVectorNode>( this, pSettings );
                return pSettings->m_nodeIdx;
            }
            break;

            case GraphValueType::Target:
            {
                GraphNodes::ControlParameterTargetNode::Settings* pSettings = nullptr;
                context.GetSettings<GraphNodes::ControlParameterTargetNode>( this, pSettings );
                return pSettings->m_nodeIdx;
            }
            break;

            case GraphValueType::BoneMask:
            {
                GraphNodes::ControlParameterBoneMaskNode::Settings* pSettings = nullptr;
                context.GetSettings<GraphNodes::ControlParameterBoneMaskNode>( this, pSettings );
                return pSettings->m_nodeIdx;
            }
            break;
        }

        KRG_UNREACHABLE_CODE();
        return InvalidIndex;
    }

    //-------------------------------------------------------------------------

    VirtualParameterEditorNode::VirtualParameterEditorNode( String const& name, GraphValueType type )
        : m_name( name )
        , m_type( type )
    {
        KRG_ASSERT( !m_name.empty() );
    }

    void VirtualParameterEditorNode::Initialize( VisualGraph::BaseGraph* pParentGraph )
    {
        EditorGraphNode::Initialize( pParentGraph );

        CreateOutputPin( "Value", m_type, true );

        auto pParameterGraph = KRG::New<FlowGraph>( GraphType::ValueTree );
        pParameterGraph->CreateNode<ResultEditorNode>( m_type );
        SetChildGraph( pParameterGraph );
    }

    GraphNodeIndex VirtualParameterEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        auto const resultNodes = GetChildGraph()->FindAllNodesOfType<ResultEditorNode>();
        KRG_ASSERT( resultNodes.size() == 1 );

        auto pConnectedNode = resultNodes[0]->GetConnectedInputNode<EditorGraphNode>( 0 );
        if ( pConnectedNode == nullptr )
        {
            context.LogError( this, "Empty virtual parameter detected!" );
            return InvalidIndex;
        }

        return resultNodes[0]->GetConnectedInputNode<EditorGraphNode>( 0 )->Compile( context );
    }

    //-------------------------------------------------------------------------

    ParameterReferenceEditorNode::ParameterReferenceEditorNode( VirtualParameterEditorNode* pParameter )
        : m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
        m_parameterValueType = pParameter->GetValueType();
    }

    ParameterReferenceEditorNode::ParameterReferenceEditorNode( ControlParameterEditorNode* pParameter )
        : m_pParameter( pParameter )
    {
        KRG_ASSERT( pParameter != nullptr );
        m_parameterUUID = pParameter->GetID();
        m_parameterValueType = pParameter->GetValueType();
    }

    void ParameterReferenceEditorNode::Initialize( VisualGraph::BaseGraph* pParentGraph )
    {
        EditorGraphNode::Initialize( pParentGraph );
        CreateOutputPin( "Value", m_pParameter->GetValueType(), true );
    }

    GraphNodeIndex ParameterReferenceEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        return m_pParameter->Compile( context );
    }
}

//-------------------------------------------------------------------------
// BASE ANIMATION FLOW GRAPH
//-------------------------------------------------------------------------

namespace KRG::Animation
{
    FlowGraph::FlowGraph( GraphType type )
        : m_type( type )
    {}

    void FlowGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        VisualGraph::FlowGraph::SerializeCustom( typeRegistry, graphObjectValue );
        m_type = (GraphType) graphObjectValue["GraphType"].GetUint();
    }

    void FlowGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        VisualGraph::FlowGraph::SerializeCustom( typeRegistry, writer );

        writer.Key( "GraphType" );
        writer.Uint( (uint8) m_type );
    }

    void FlowGraph::PostPasteNodes( TInlineVector<VisualGraph::BaseNode*, 20> const& pastedNodes )
    {
        VisualGraph::FlowGraph::PostPasteNodes( pastedNodes );
        GraphNodes::ParameterReferenceEditorNode::RefreshParameterReferences( GetRootGraph() );
    }
}