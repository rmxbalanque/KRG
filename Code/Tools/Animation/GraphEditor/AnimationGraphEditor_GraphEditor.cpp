#include "AnimationGraphEditor_GraphEditor.h"
#include "Tools/Animation/Graph/Nodes/AnimationToolsNode_Parameters.h"
#include "Tools/Animation/Graph/AnimationGraphTools_AnimationGraph.h"
#include "Tools/Animation/Graph/AnimationGraphTools_StateMachineGraph.h"
#include "Tools/Animation/Graph/StateMachine/AnimationToolsNode_State.h"
#include "Tools/Animation/Graph/StateMachine/AnimationToolsNode_Transitions.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void GraphEditorView::FlowGraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        KRG_ASSERT( m_pGraph != nullptr );
        auto pToolsGraph = static_cast<FlowToolGraph*>( m_pGraph );

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Control Parameters" ) )
        {
            TVector<ControlParameterToolsNode*> sortedControlParameters = m_model.GetGraph()->GetControlParameters();
            eastl::sort( sortedControlParameters.begin(), sortedControlParameters.end(), [] ( ControlParameterToolsNode* const& pA, ControlParameterToolsNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

            TVector<VirtualParameterToolsNode*> sortedVirtualParameters = m_model.GetGraph()->GetVirtualParameters();
            eastl::sort( sortedVirtualParameters.begin(), sortedVirtualParameters.end(), [] ( VirtualParameterToolsNode* const& pA, VirtualParameterToolsNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

            //-------------------------------------------------------------------------

            if ( m_model.GetGraph()->GetControlParameters().empty() && m_model.GetGraph()->GetVirtualParameters().empty() )
            {
                ImGui::Text( "No Parameters" );
            }
            else
            {
                for ( auto pParameter : sortedControlParameters )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pParameter->GetHighlightColor() );
                    if ( ImGui::MenuItem( pParameter->GetDisplayName() ) )
                    {
                        auto pNode = pToolsGraph->CreateNode<ControlParameterReferenceToolsNode>( pParameter );
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
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pParameter->GetHighlightColor() );
                    if ( ImGui::MenuItem( pParameter->GetDisplayName() ) )
                    {
                        auto pNode = pToolsGraph->CreateNode<VirtualParameterReferenceToolsNode>( pParameter );
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

        DrawNodeTypeCategoryContextMenu( mouseCanvasPos, pToolsGraph, m_model.GetNodeTypes() );
    }

    void GraphEditorView::FlowGraphView::DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, FlowToolGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category )
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
            auto pDefaultNode = Cast<FlowToolsNode>( item.m_data->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( m_pGraph->CanCreateNode( item.m_data ) && pDefaultNode->GetAllowedParentGraphTypes().AreAnyFlagsSet( pGraph->GetType() ) )
            {
                if ( ImGui::MenuItem( item.m_name.c_str() ) )
                {
                    auto pToolsGraph = static_cast<FlowToolGraph*>( m_pGraph );
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

    void GraphEditorView::FlowGraphView::OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );

        auto pParentNode = pGraph->GetParentNode();
        if ( pParentNode != nullptr )
        {
            m_model.NavigateTo( pParentNode->GetParentGraph() );
        }
    }

    void GraphEditorView::FlowGraphView::OnNodeDoubleClick( GraphEditor::BaseNode* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        if ( pNode->HasChildGraph() )
        {
            m_model.NavigateTo( pNode->GetChildGraph() );
        }
    }

    //-------------------------------------------------------------------------

    void GraphEditorView::StateMachineGraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        KRG_ASSERT( m_pGraph != nullptr );
        auto pToolsGraph = static_cast<StateMachineToolsGraph*>( m_pGraph );

        if ( ImGui::MenuItem( "Blend Tree State") )
        {
            pToolsGraph->CreateNewState( mouseCanvasPos );
        }

        if ( ImGui::MenuItem( "Off State" ) )
        {
            pToolsGraph->CreateNewOffState( mouseCanvasPos );
        }
    }

    void GraphEditorView::StateMachineGraphView::DrawContextMenuForNode( ImVec2 const& mouseCanvasPos, GraphEditor::SM::Node* pNode )
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

    void GraphEditorView::StateMachineGraphView::OnGraphDoubleClick( GraphEditor::BaseGraph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );

        auto pParentNode = pGraph->GetParentNode();
        if ( pParentNode != nullptr )
        {
            m_model.NavigateTo( pParentNode->GetParentGraph() );
        }
    }

    void GraphEditorView::StateMachineGraphView::OnNodeDoubleClick( GraphEditor::BaseNode* pNode )
    {
        if ( pNode->HasChildGraph() )
        {
            m_model.NavigateTo( pNode->GetChildGraph() );
        }
    }

    void GraphEditorView::StateMachineGraphView::DrawExtraInformation( GraphEditor::DrawingContext const& ctx )
    {
        auto pToolsGraph = static_cast<StateMachineToolsGraph*>( m_pGraph );
        auto pGlobalTransitionsNode = pToolsGraph->GetGlobalTransitionsNode();

        auto const stateNodes = pToolsGraph->FindAllNodesOfType<StateBaseToolsNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );
        for ( auto pStateNode : stateNodes )
        {
            if ( pGlobalTransitionsNode->HasGlobalTransitionForState( pStateNode->GetID() ) )
            {
                auto const nodeRect = GetNodeWindowRect( pStateNode );
                ImVec2 const iconSize = ImGui::CalcTextSize( KRG_ICON_BOLT );
                ImVec2 const iconOffset( 0, iconSize.y + 4.0f );
                
                ctx.m_pDrawList->AddText( nodeRect.Min + ctx.m_windowRect.Min - iconOffset, ImGuiX::ConvertColor( Colors::OrangeRed ), KRG_ICON_BOLT );
            }
        }
    }

    //-------------------------------------------------------------------------

    GraphEditorView::GraphEditorView( GraphEditorModel& model )
        : m_model( model )
        , m_primaryFlowGraphView( static_cast<GraphEditorModel&>( model ) )
        , m_primaryStateMachineGraphView( static_cast<GraphEditorModel&>( model ) )
        , m_secondaryFlowGraphView( static_cast<GraphEditorModel&>( model ) )
    {}

    void GraphEditorView::RefreshView()
    {
        m_primaryFlowGraphView.RefreshVisualState();
        m_primaryStateMachineGraphView.RefreshVisualState();
        m_secondaryFlowGraphView.RefreshVisualState();
    }

    void GraphEditorView::UpdatePrimaryViewState()
    {
        if ( m_model.GetGraph()->IsValid() )
        {
            auto pDesiredViewedGraph = m_model.GetCurrentlyViewedGraph();
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

    void GraphEditorView::UpdateSecondaryViewState()
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

    void GraphEditorView::Draw( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        if ( ImGui::Begin( "Graph Editor" ) )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

            // Primary View
            //-------------------------------------------------------------------------

            UpdatePrimaryViewState();
            m_pPrimaryGraphView->Draw( m_primaryGraphViewHeight );

            // Splitter
            //-------------------------------------------------------------------------
            
            ImGui::PushStyleColor( ImGuiCol_Button, ImGuiX::Theme::s_backgroundColorSemiDark );
            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImGuiX::Theme::s_backgroundColorSemiLight );
            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImGuiX::Theme::s_backgroundColorSemiLight );
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
            m_secondaryFlowGraphView.Draw();

            //-------------------------------------------------------------------------

            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar();

        // Handle Focus and selection 
        //-------------------------------------------------------------------------

        HandleFocusAndSelectionChanges();
    }

    void GraphEditorView::HandleFocusAndSelectionChanges()
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

        bool hasUpdatedFocus = false;
        if ( pCurrentlyFocusedGraphView != nullptr && pCurrentlyFocusedGraphView != m_pFocusedGraphView )
        {
            m_pFocusedGraphView = pCurrentlyFocusedGraphView;
            hasUpdatedFocus = true;
        }

        // Update selection (if necessary)
        //-------------------------------------------------------------------------

        if ( pCurrentlyFocusedGraphView != nullptr && ( hasUpdatedFocus || pCurrentlyFocusedGraphView->HasSelectionChanged() ) )
        {
            m_model.ClearSelection();

            for ( auto pNode : pCurrentlyFocusedGraphView->GetSelectedNodes() )
            {
                m_model.AddToSelection( pNode );
            }
        }
    }
}