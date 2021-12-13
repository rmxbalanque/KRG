#include "AnimationGraphEditor_GraphView.h"
#include "ToolsGraph/AnimationToolsGraph_Definition.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/Nodes/AnimationToolsNode_State.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/Nodes/AnimationToolsNode_Transitions.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void GraphView::FlowGraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        KRG_ASSERT( m_pGraph != nullptr );

        auto pGraphDefinition = m_parentGraphView.GetGraphDefinition();
        auto pToolsFlowGraph = static_cast<FlowGraph*>( m_pGraph );

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Control Parameters" ) )
        {
            TVector<Tools_ControlParameterNode*> sortedControlParameters = pGraphDefinition->GetControlParameters();
            eastl::sort( sortedControlParameters.begin(), sortedControlParameters.end(), [] ( Tools_ControlParameterNode* const& pA, Tools_ControlParameterNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

            TVector<Tools_VirtualParameterNode*> sortedVirtualParameters = pGraphDefinition->GetVirtualParameters();
            eastl::sort( sortedVirtualParameters.begin(), sortedVirtualParameters.end(), [] ( Tools_VirtualParameterNode* const& pA, Tools_VirtualParameterNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

            //-------------------------------------------------------------------------

            if ( pGraphDefinition->GetControlParameters().empty() && pGraphDefinition->GetVirtualParameters().empty() )
            {
                ImGui::Text( "No Parameters" );
            }
            else
            {
                for ( auto pParameter : sortedControlParameters )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pParameter->GetNodeColor() );
                    if ( ImGui::MenuItem( pParameter->GetDisplayName() ) )
                    {
                        GraphEditor::ScopedGraphModification sgm( m_pGraph );
                        auto pNode = pToolsFlowGraph->CreateNode<Tools_ControlParameterReferenceNode>( pParameter );
                        pNode->SetCanvasPosition( mouseCanvasPos );
                    }
                    ImGui::PopStyleColor();
                }

                if ( !sortedVirtualParameters.empty() )
                {
                    ImGui::Separator();
                }

                for ( auto pParameter : sortedVirtualParameters )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pParameter->GetNodeColor() );
                    if ( ImGui::MenuItem( pParameter->GetDisplayName() ) )
                    {
                        GraphEditor::ScopedGraphModification sgm( m_pGraph );
                        auto pNode = pToolsFlowGraph->CreateNode<Tools_VirtualParameterReferenceNode>( pParameter );
                        pNode->SetCanvasPosition( mouseCanvasPos );
                    }
                    ImGui::PopStyleColor();
                }
            }

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------

        ImGui::Separator();

        //-------------------------------------------------------------------------

        DrawNodeTypeCategoryContextMenu( mouseCanvasPos, pToolsFlowGraph, m_parentGraphView.GetNodeTypes() );
    }

    void GraphView::FlowGraphView::DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, FlowGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category )
    {
        KRG_ASSERT( m_pGraph != nullptr );
       
        for ( auto const& childCategory : category.m_childCategories )
        {
            if ( ImGui::BeginMenu( childCategory.m_name.c_str() ) )
            {
                DrawNodeTypeCategoryContextMenu( mouseCanvasPos, pGraph, childCategory );
                ImGui::EndMenu();
            }
        }

        //-------------------------------------------------------------------------

        for ( auto const& item : category.m_items )
        {
            auto pDefaultNode = Cast<Tools_GraphNode>( item.m_data->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( m_pGraph->CanCreateNode( item.m_data ) && pDefaultNode->GetAllowedParentGraphTypes().AreAnyFlagsSet( pGraph->GetType() ) )
            {
                if ( ImGui::MenuItem( item.m_name.c_str() ) )
                {
                    GraphEditor::ScopedGraphModification sgm( m_pGraph );
                    auto pToolsGraph = static_cast<FlowGraph*>( m_pGraph );
                    auto pNode = pToolsGraph->CreateNode( item.m_data );
                    pNode->SetCanvasPosition( mouseCanvasPos );
                }
            }
            else
            {
                ImGui::TextColored( ImColor( 0xFF0000FF ), item.m_name.c_str() );
            }
        }
    }

    void GraphView::FlowGraphView::OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );

        auto pParentNode = pGraph->GetParentNode();
        if ( pParentNode != nullptr )
        {
            m_parentGraphView.NavigateTo( pParentNode->GetParentGraph() );
        }
    }

    void GraphView::FlowGraphView::OnNodeDoubleClick( GraphEditor::BaseNode* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        if ( pNode->HasChildGraph() )
        {
            m_parentGraphView.NavigateTo( pNode->GetChildGraph() );
        }
    }

    //-------------------------------------------------------------------------

    void GraphView::StateMachineGraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        KRG_ASSERT( m_pGraph != nullptr );
        auto pToolsGraph = static_cast<StateMachineGraph*>( m_pGraph );

        if ( ImGui::MenuItem( "Blend Tree State") )
        {
            pToolsGraph->CreateNewState( mouseCanvasPos );
        }

        if ( ImGui::MenuItem( "Off State" ) )
        {
            pToolsGraph->CreateNewOffState( mouseCanvasPos );
        }
    }

    void GraphView::StateMachineGraphView::DrawContextMenuForNode( ImVec2 const& mouseCanvasPos, GraphEditor::SM::Node* pNode )
    {
        auto pStateNode = TryCast<GraphEditor::SM::State>( pNode );
        if ( pStateNode != nullptr )
        {
            if ( ImGui::MenuItem( "Make Default Entry State" ) )
            {
                m_pGraph->SetDefaultEntryState( pStateNode->GetID() );
            }
        }
    }

    void GraphView::StateMachineGraphView::OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );

        auto pParentNode = pGraph->GetParentNode();
        if ( pParentNode != nullptr )
        {
            m_parentGraphView.NavigateTo( pParentNode->GetParentGraph() );
        }
    }

    void GraphView::StateMachineGraphView::OnNodeDoubleClick( GraphEditor::BaseNode* pNode )
    {
        if ( pNode->HasChildGraph() )
        {
            m_parentGraphView.NavigateTo( pNode->GetChildGraph() );
        }
    }

    void GraphView::StateMachineGraphView::DrawExtraInformation( GraphEditor::DrawContext const& ctx )
    {
        auto pToolsGraph = static_cast<StateMachineGraph*>( m_pGraph );
        auto pGlobalTransitionsNode = pToolsGraph->GetGlobalTransitionsNode();

        auto const stateNodes = pToolsGraph->FindAllNodesOfType<Tools_StateBaseNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );
        for ( auto pStateNode : stateNodes )
        {
            ImRect const nodeRect = GetNodeWindowRect( pStateNode );
            ImVec2 const iconSize = ImGui::CalcTextSize( KRG_ICON_ASTERISK );
            ImVec2 iconOffset( 0, iconSize.y + 4.0f );

            // Draw entry state marker
            if ( pStateNode->GetID() == pToolsGraph->GetDefaultEntryStateID() )
            {
                ctx.m_pDrawList->AddText( nodeRect.Min + ctx.m_windowRect.Min - iconOffset, ImGuiX::ConvertColor( Colors::LimeGreen ), KRG_ICON_ASTERISK );
                iconOffset.x -= iconSize.x + 4.0f;
            }

            // Draw global transition marker
            if ( pGlobalTransitionsNode->HasGlobalTransitionForState( pStateNode->GetID() ) )
            {
                ctx.m_pDrawList->AddText( nodeRect.Min + ctx.m_windowRect.Min - iconOffset, ImGuiX::ConvertColor( Colors::OrangeRed ), KRG_ICON_BOLT );
            }
        }
    }

    //-------------------------------------------------------------------------

    GraphView::GraphView( TypeSystem::TypeRegistry const& typeRegistry, AnimationGraphToolsDefinition* pGraphDefinition )
        : m_pGraphDefinition( pGraphDefinition )
        , m_primaryFlowGraphView( *this )
        , m_primaryStateMachineGraphView( *this )
        , m_secondaryFlowGraphView( *this )
    {
        KRG_ASSERT( pGraphDefinition != nullptr );

        // Create DB of all node types
        //-------------------------------------------------------------------------

        m_registeredNodeTypes = typeRegistry.GetAllDerivedTypes( Tools_GraphNode::GetStaticTypeID(), false, false );

        for ( auto pNodeType : m_registeredNodeTypes )
        {
            auto pDefaultNode = Cast<Tools_GraphNode const>( pNodeType->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( pDefaultNode->IsUserCreatable() )
            {
                m_categorizedNodeTypes.AddItem( pDefaultNode->GetCategory(), pDefaultNode->GetTypeName(), pNodeType );
            }
        }

        // Show Root Graph
        //-------------------------------------------------------------------------

        NavigateTo( pGraphDefinition->GetRootGraph() );
    }

    TVector<GraphEditor::BaseGraphView::SelectedNode> const& GraphView::GetSelectedNodes() const
    {
        static TVector<GraphEditor::BaseGraphView::SelectedNode> const emptySelection;

        if ( m_pFocusedGraphView != nullptr )
        {
            return m_pFocusedGraphView->GetSelectedNodes();
        }

        return emptySelection;
    }

    void GraphView::OnUndoRedo()
    {
        auto pFoundGraph = m_pGraphDefinition->GetRootGraph()->FindPrimaryGraph( m_primaryViewGraphID );
        if ( pFoundGraph != nullptr )
        {
            if ( auto pSM = TryCast<GraphEditor::StateMachineGraph>( pFoundGraph ) )
            {
                if ( m_primaryStateMachineGraphView.GetViewedGraph() != pFoundGraph )
                {
                    m_primaryFlowGraphView.SetGraphToView( nullptr );
                    m_primaryStateMachineGraphView.SetGraphToView( pSM, true );
                    m_pPrimaryGraphView = &m_primaryStateMachineGraphView;
                }
            }
            // Show the flow graph
            else if ( auto pFlowGraph = Cast<GraphEditor::FlowGraph>( pFoundGraph ) )
            {
                if ( m_primaryFlowGraphView.GetViewedGraph() != pFoundGraph )
                {
                    m_primaryStateMachineGraphView.SetGraphToView( nullptr );
                    m_primaryFlowGraphView.SetGraphToView( pFlowGraph, true );
                    m_pPrimaryGraphView = &m_primaryFlowGraphView;
                }
            }
            else
            {
                KRG_UNREACHABLE_CODE();
            }

            UpdateSecondaryViewState();
        }
        else
        {
            NavigateTo( m_pGraphDefinition->GetRootGraph() );
            m_secondaryFlowGraphView.SetGraphToView( nullptr );
        }
    }

    void GraphView::NavigateTo( GraphEditor::BaseNode* pNode )
    {
        KRG_ASSERT( pNode != nullptr );
        auto pParentGraph = pNode->GetParentGraph();
        KRG_ASSERT( pParentGraph != nullptr );
        NavigateTo( pParentGraph );

        // Select node
        if ( m_pPrimaryGraphView->GetViewedGraph()->FindNode( pNode->GetID() ) )
        {
            m_pPrimaryGraphView->SelectNode( pNode );
        }
        else if ( m_secondaryFlowGraphView.GetViewedGraph() != nullptr && m_secondaryFlowGraphView.GetViewedGraph()->FindNode( pNode->GetID() ) )
        {
            m_secondaryFlowGraphView.SelectNode( pNode );
        }
    }

    void GraphView::NavigateTo( GraphEditor::BaseGraph* pGraph )
    {
        // Show the state machine graph
        if ( auto pSM = TryCast<GraphEditor::StateMachineGraph>( pGraph ) )
        {
            if ( m_primaryStateMachineGraphView.GetViewedGraph() != pGraph )
            {
                m_primaryFlowGraphView.SetGraphToView( nullptr );
                m_primaryStateMachineGraphView.SetGraphToView( pSM );
                m_pPrimaryGraphView = &m_primaryStateMachineGraphView;
                m_primaryViewGraphID = pGraph->GetID();
            }
        }
        // Show the flow graph
        else if( auto pFlowGraph = Cast<GraphEditor::FlowGraph>( pGraph ) )
        {
            if ( m_primaryFlowGraphView.GetViewedGraph() != pGraph )
            {
                m_primaryStateMachineGraphView.SetGraphToView( nullptr );
                m_primaryFlowGraphView.SetGraphToView( pFlowGraph );
                m_pPrimaryGraphView = &m_primaryFlowGraphView;
                m_primaryViewGraphID = pGraph->GetID();
            }
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }
    }

    void GraphView::UpdateSecondaryViewState()
    {
        if ( m_pPrimaryGraphView->HasSelectedNodes() )
        {
            auto const& lastSelectedNode = m_pPrimaryGraphView->GetSelectedNodes().back();
            if ( auto pSecondaryGraph = TryCast<GraphEditor::FlowGraph>( lastSelectedNode.m_pNode->GetSecondaryGraph() ) )
            {
                m_secondaryFlowGraphView.SetGraphToView( pSecondaryGraph );
            }
            else
            {
                m_secondaryFlowGraphView.SetGraphToView( nullptr );
            }
        }
        else
        {
            m_secondaryFlowGraphView.SetGraphToView( nullptr );
        }
    }

    void GraphView::UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        bool isGraphViewFocused = false;
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName ) )
        {
            isGraphViewFocused = ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows );

            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

            // Primary View
            //-------------------------------------------------------------------------

            auto pPrimaryGraph = m_pPrimaryGraphView->GetViewedGraph();
            auto pParentNode = pPrimaryGraph->GetParentNode();
            if ( pParentNode != nullptr )
            {
                auto path = pParentNode->GetPathFromRoot();
                ImGui::Text( path.c_str() );
            }
            else
            {
                ImGui::Text( "root" );
            }

            m_pPrimaryGraphView->UpdateAndDraw( m_primaryGraphViewHeight, pDebugContext );

            // Splitter
            //-------------------------------------------------------------------------
            
            ImGui::PushStyleColor( ImGuiCol_Button, ImGuiX::Style::s_backgroundColorSemiDark.Value );
            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImGuiX::Style::s_backgroundColorSemiLight.Value );
            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImGuiX::Style::s_backgroundColorSemiLight.Value );
            ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
            ImGui::Button( "##GraphViewSplitter", ImVec2( -1, 3 ) );
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();

            if ( ImGui::IsItemHovered() )
            {
                ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNS );
            }

            if ( ImGui::IsItemActive() )
            {
                m_primaryGraphViewHeight += ImGui::GetIO().MouseDelta.y;
                m_primaryGraphViewHeight = Math::Max( 25.0f, m_primaryGraphViewHeight );
            }

            // SecondaryView
            //-------------------------------------------------------------------------

            UpdateSecondaryViewState();
            m_secondaryFlowGraphView.UpdateAndDraw( 0.0f, pDebugContext );

            //-------------------------------------------------------------------------

            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar();

        // Handle Focus and selection 
        //-------------------------------------------------------------------------

        HandleFocusAndSelectionChanges();
    }

    void GraphView::HandleFocusAndSelectionChanges()
    {
        GraphEditor::BaseGraphView* pCurrentlyFocusedGraphView = nullptr;

        // Get the currently focused view
        //-------------------------------------------------------------------------

        if ( m_primaryStateMachineGraphView.HasFocus() )
        {
            pCurrentlyFocusedGraphView = &m_primaryStateMachineGraphView;
        }
        else if ( m_primaryFlowGraphView.HasFocus() )
        {
            pCurrentlyFocusedGraphView = &m_primaryFlowGraphView;
        }
        else if ( m_secondaryFlowGraphView.HasFocus() )
        {
            pCurrentlyFocusedGraphView = &m_secondaryFlowGraphView;
        }

        // Has the focus within the graph editor tool changed?
        //-------------------------------------------------------------------------

        if ( pCurrentlyFocusedGraphView != nullptr && pCurrentlyFocusedGraphView != m_pFocusedGraphView )
        {
            m_pFocusedGraphView = pCurrentlyFocusedGraphView;
        }
    }

    void GraphView::ClearSelection()
    {
        m_primaryFlowGraphView.ClearSelection();
        m_primaryStateMachineGraphView.ClearSelection();
        m_secondaryFlowGraphView.ClearSelection();
    }
}