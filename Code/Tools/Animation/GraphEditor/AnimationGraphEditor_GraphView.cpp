#include "AnimationGraphEditor_GraphView.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/Nodes/AnimationToolsNode_State.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/Nodes/AnimationToolsNode_Transitions.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void GraphView::FlowGraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        KRG_ASSERT( m_pGraph != nullptr );
        auto pToolsGraph = static_cast<FlowGraph*>( m_pGraph );

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Control Parameters" ) )
        {
            TVector<Tools_ControlParameterNode*> sortedControlParameters = m_graphModel.GetGraph()->GetControlParameters();
            eastl::sort( sortedControlParameters.begin(), sortedControlParameters.end(), [] ( Tools_ControlParameterNode* const& pA, Tools_ControlParameterNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

            TVector<Tools_VirtualParameterNode*> sortedVirtualParameters = m_graphModel.GetGraph()->GetVirtualParameters();
            eastl::sort( sortedVirtualParameters.begin(), sortedVirtualParameters.end(), [] ( Tools_VirtualParameterNode* const& pA, Tools_VirtualParameterNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

            //-------------------------------------------------------------------------

            if ( m_graphModel.GetGraph()->GetControlParameters().empty() && m_graphModel.GetGraph()->GetVirtualParameters().empty() )
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
                        auto pNode = pToolsGraph->CreateNode<Tools_ControlParameterReferenceNode>( pParameter );
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
                        auto pNode = pToolsGraph->CreateNode<Tools_VirtualParameterReferenceNode>( pParameter );
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

        DrawNodeTypeCategoryContextMenu( mouseCanvasPos, pToolsGraph, m_graphModel.GetNodeTypes() );
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
            m_graphModel.NavigateTo( pParentNode->GetParentGraph() );
        }
    }

    void GraphView::FlowGraphView::OnNodeDoubleClick( GraphEditor::BaseNode* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        if ( pNode->HasChildGraph() )
        {
            m_graphModel.NavigateTo( pNode->GetChildGraph() );
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
            m_graphModel.NavigateTo( pParentNode->GetParentGraph() );
        }
    }

    void GraphView::StateMachineGraphView::OnNodeDoubleClick( GraphEditor::BaseNode* pNode )
    {
        if ( pNode->HasChildGraph() )
        {
            m_graphModel.NavigateTo( pNode->GetChildGraph() );
        }
    }

    void GraphView::StateMachineGraphView::DrawExtraInformation( GraphEditor::DrawingContext const& ctx )
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

    GraphView::GraphView( GraphEditorModel& graphModel )
        : m_graphModel( graphModel )
        , m_primaryFlowGraphView( graphModel )
        , m_primaryStateMachineGraphView( graphModel )
        , m_secondaryFlowGraphView( graphModel )
    {}

    void GraphView::UpdatePrimaryViewState()
    {
        if ( m_graphModel.GetGraph()->IsValid() )
        {
            auto pDesiredViewedGraph = m_graphModel.GetCurrentlyViewedGraph();
            if ( auto pSM = TryCast<GraphEditor::StateMachineGraph>( pDesiredViewedGraph ) )
            {
                if ( m_primaryStateMachineGraphView.GetViewedGraph() != pDesiredViewedGraph )
                {
                    m_primaryFlowGraphView.SetGraphToView( nullptr );
                    m_primaryStateMachineGraphView.SetGraphToView( pSM );
                    m_pPrimaryGraphView = &m_primaryStateMachineGraphView;
                }
            }
            else // Show the flow graph
            {
                auto pFlowGraph = Cast<GraphEditor::FlowGraph>( pDesiredViewedGraph );
                if ( m_primaryFlowGraphView.GetViewedGraph() != pDesiredViewedGraph )
                {
                    m_primaryStateMachineGraphView.SetGraphToView( nullptr );
                    m_primaryFlowGraphView.SetGraphToView( pFlowGraph );
                    m_pPrimaryGraphView = &m_primaryFlowGraphView;
                }
            }
        }
        else
        {
            m_pPrimaryGraphView = &m_primaryFlowGraphView;
        }
    }

    void GraphView::UpdateSecondaryViewState()
    {
        if ( m_pPrimaryGraphView->HasSelectedNodes() )
        {
            auto const pLastSelectedNode = m_pPrimaryGraphView->GetSelectedNodes().back();
            if ( auto pSecondaryGraph = TryCast<GraphEditor::FlowGraph>( pLastSelectedNode->GetSecondaryGraph() ) )
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

    void GraphView::UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName ) )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

            // Primary View
            //-------------------------------------------------------------------------

            UpdatePrimaryViewState();

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

            m_pPrimaryGraphView->Draw( m_primaryGraphViewHeight, m_graphModel.GetDebugContext() );

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
            m_secondaryFlowGraphView.Draw( 0.0f, m_graphModel.GetDebugContext() );

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

        // Update selection (if necessary)
        //-------------------------------------------------------------------------

        if ( pCurrentlyFocusedGraphView != nullptr )
        {
            m_graphModel.ClearSelection();

            for ( auto pNode : pCurrentlyFocusedGraphView->GetSelectedNodes() )
            {
                m_graphModel.AddToSelection( pNode );
            }
        }
    }
}