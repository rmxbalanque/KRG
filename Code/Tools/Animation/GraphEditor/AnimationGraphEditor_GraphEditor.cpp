#include "AnimationGraphEditor_GraphEditor.h"
#include "EditorGraph/Animation_EditorGraph_Definition.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void GraphEditor::GraphView::DrawContextMenuForGraph()
    {
        KRG_ASSERT( m_pGraph != nullptr );

        if ( IsViewingStateMachineGraph() )
        {
            KRG_ASSERT( m_pGraph != nullptr );
            auto pStateMachineEditorGraph = static_cast<StateMachineGraph*>( m_pGraph );

            if ( ImGui::MenuItem( "Blend Tree State" ) )
            {
                pStateMachineEditorGraph->CreateNewState( m_contextMenuState.m_mouseCanvasPos );
            }

            if ( ImGui::MenuItem( "Off State" ) )
            {
                pStateMachineEditorGraph->CreateNewOffState( m_contextMenuState.m_mouseCanvasPos );
            }
        }
        else // Flow Graph
        {
            auto pGraphDefinition = m_graphEditor.GetGraphDefinition();
            auto pFlowEditorGraph = static_cast<FlowGraph*>( m_pGraph );

            //-------------------------------------------------------------------------

            if ( ImGui::BeginMenu( "Control Parameters" ) )
            {
                TInlineVector<ControlParameterEditorNode*, 20> sortedControlParameters = pGraphDefinition->GetControlParameters();
                eastl::sort( sortedControlParameters.begin(), sortedControlParameters.end(), [] ( ControlParameterEditorNode* const& pA, ControlParameterEditorNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

                TInlineVector<VirtualParameterEditorNode*, 20> sortedVirtualParameters = pGraphDefinition->GetVirtualParameters();
                eastl::sort( sortedVirtualParameters.begin(), sortedVirtualParameters.end(), [] ( VirtualParameterEditorNode* const& pA, VirtualParameterEditorNode* const& pB ) { return strcmp( pA->GetDisplayName(), pB->GetDisplayName() ) < 0; } );

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
                            VisualGraph::ScopedGraphModification sgm( m_pGraph );
                            auto pNode = pFlowEditorGraph->CreateNode<ParameterReferenceEditorNode>( pParameter );
                            pNode->SetCanvasPosition( m_contextMenuState.m_mouseCanvasPos );
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
                            VisualGraph::ScopedGraphModification sgm( m_pGraph );
                            auto pNode = pFlowEditorGraph->CreateNode<ParameterReferenceEditorNode>( pParameter );
                            pNode->SetCanvasPosition( m_contextMenuState.m_mouseCanvasPos );
                        }
                        ImGui::PopStyleColor();
                    }
                }

                ImGui::EndMenu();
            }

            //-------------------------------------------------------------------------

            DrawNodeTypeCategoryContextMenu( m_contextMenuState.m_mouseCanvasPos, pFlowEditorGraph, m_graphEditor.GetCategorizedNodeTypes() );
        }
    }

    void GraphEditor::GraphView::DrawContextMenuForNode()
    {
        if ( IsViewingStateMachineGraph() )
        {
            auto pStateNode = TryCast<VisualGraph::SM::State>( m_contextMenuState.m_pNode );
            if ( pStateNode != nullptr )
            {
                if ( ImGui::MenuItem( "Make Default Entry State" ) )
                {
                    GetStateMachineGraph()->SetDefaultEntryState(pStateNode->GetID());
                }
            }
        }
    }

    bool GraphEditor::GraphView::DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, FlowGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category )
    {
        KRG_ASSERT( m_pGraph != nullptr );
        KRG_ASSERT( IsViewingFlowGraph() );

        auto NodeFilter = [this, pGraph] ( CategoryItem<TypeSystem::TypeInfo const*> const& item )
        {
            // Parameter references are already handled
            if ( item.m_data->m_ID == ParameterReferenceEditorNode::GetStaticTypeID() )
            {
                return false;
            }

            // Is this a valid node for this graph
            if ( !m_pGraph->CanCreateNode( item.m_data ) )
            {
                return false;
            }

            // Check the graphs specific restrictions
            auto pDefaultNode = Cast<EditorGraphNode>( item.m_data->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( !pDefaultNode->GetAllowedParentGraphTypes().AreAnyFlagsSet( pGraph->GetType() ) )
            {
                return false;
            }

            return true;
        };

        if ( !category.HasItemsThatMatchFilter( NodeFilter ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        auto DrawItems = [this, &NodeFilter, &category, &mouseCanvasPos, &pGraph] ()
        {
            for ( auto const& childCategory : category.m_childCategories )
            {
                DrawNodeTypeCategoryContextMenu( mouseCanvasPos, pGraph, childCategory );
            }

            if ( category.m_depth == -1 )
            {
                ImGui::Separator();
            }

            for ( auto const& item : category.m_items )
            {
                if ( !NodeFilter( item ) )
                {
                    continue;
                }

                if ( ImGui::MenuItem( item.m_name.c_str() ) )
                {
                    VisualGraph::ScopedGraphModification sgm( m_pGraph );
                    auto pEditorGraph = static_cast<FlowGraph*>( m_pGraph );
                    auto pNode = pEditorGraph->CreateNode( item.m_data );
                    pNode->SetCanvasPosition( mouseCanvasPos );
                }
            }
        };

        if ( category.m_depth == -1 )
        {
            DrawItems();
        }
        else if ( category.m_depth == 0 )
        {
            ImGuiX::TextSeparator( category.m_name.c_str() );
            DrawItems();
        }
        else
        {
            if ( ImGui::BeginMenu( category.m_name.c_str() ) )
            {
                DrawItems();
                ImGui::EndMenu();
            }
        }

        return true;
    }

    void GraphEditor::GraphView::OnGraphDoubleClick( VisualGraph::BaseGraph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );

        auto pParentNode = pGraph->GetParentNode();
        if ( pParentNode != nullptr )
        {
            m_graphEditor.NavigateTo( pParentNode->GetParentGraph() );
        }
    }

    void GraphEditor::GraphView::OnNodeDoubleClick( VisualGraph::BaseNode* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        if ( pNode->HasChildGraph() )
        {
            m_graphEditor.NavigateTo( pNode->GetChildGraph() );
        }
    }

    void GraphEditor::GraphView::DrawExtraInformation( VisualGraph::DrawContext const& ctx )
    {
        if ( IsViewingStateMachineGraph() )
        {
            auto pEditorGraph = static_cast<StateMachineGraph*>( m_pGraph );

            auto const stateNodes = pEditorGraph->FindAllNodesOfType<StateBaseEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Derived );
            for ( auto pStateNode : stateNodes )
            {
                ImRect const nodeRect = GetNodeWindowRect( pStateNode );
                ImVec2 const iconSize = ImGui::CalcTextSize( KRG_ICON_ASTERISK );
                ImVec2 iconOffset( 0, iconSize.y + 4.0f );

                // Draw entry state marker
                if ( pStateNode->GetID() == pEditorGraph->GetDefaultEntryStateID() )
                {
                    ctx.m_pDrawList->AddText( nodeRect.Min + ctx.m_windowRect.Min - iconOffset, ImGuiX::ConvertColor( Colors::LimeGreen ), KRG_ICON_ASTERISK );
                    iconOffset.x -= iconSize.x + 4.0f;
                }

                // Draw global transition marker
                if ( pEditorGraph->HasGlobalTransitionForState( pStateNode->GetID() ) )
                {
                    ctx.m_pDrawList->AddText( nodeRect.Min + ctx.m_windowRect.Min - iconOffset, ImGuiX::ConvertColor( Colors::OrangeRed ), KRG_ICON_BOLT );
                }
            }
        }
    }

    //-------------------------------------------------------------------------

    GraphEditor::GraphEditor( TypeSystem::TypeRegistry const& typeRegistry, AnimationGraphEditorDefinition* pGraphDefinition )
        : m_pGraphDefinition( pGraphDefinition )
        , m_primaryGraphView( *this )
        , m_secondaryGraphView( *this )
    {
        KRG_ASSERT( pGraphDefinition != nullptr );

        // Create DB of all node types
        //-------------------------------------------------------------------------

        m_registeredNodeTypes = typeRegistry.GetAllDerivedTypes( EditorGraphNode::GetStaticTypeID(), false, false );

        for ( auto pNodeType : m_registeredNodeTypes )
        {
            auto pDefaultNode = Cast<EditorGraphNode const>( pNodeType->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( pDefaultNode->IsUserCreatable() )
            {
                m_categorizedNodeTypes.AddItem( pDefaultNode->GetCategory(), pDefaultNode->GetTypeName(), pNodeType );
            }
        }

        // Show Root Graph
        //-------------------------------------------------------------------------

        NavigateTo( pGraphDefinition->GetRootGraph() );
    }

    TVector<VisualGraph::SelectedNode> const& GraphEditor::GetSelectedNodes() const
    {
        static TVector<VisualGraph::SelectedNode> const emptySelection;

        if ( m_pFocusedGraphView != nullptr )
        {
            return m_pFocusedGraphView->GetSelectedNodes();
        }

        return emptySelection;
    }

    void GraphEditor::OnUndoRedo()
    {
        auto pFoundGraph = m_pGraphDefinition->GetRootGraph()->FindPrimaryGraph( m_primaryViewGraphID );
        if ( pFoundGraph != nullptr )
        {
            if ( m_primaryGraphView.GetViewedGraph() != pFoundGraph )
            {
                m_primaryGraphView.SetGraphToView( pFoundGraph );
            }

            UpdateSecondaryViewState();
        }
        else
        {
            NavigateTo( m_pGraphDefinition->GetRootGraph() );
        }
    }

    void GraphEditor::NavigateTo( VisualGraph::BaseNode* pNode )
    {
        KRG_ASSERT( pNode != nullptr );
        auto pParentGraph = pNode->GetParentGraph();
        KRG_ASSERT( pParentGraph != nullptr );
        NavigateTo( pParentGraph );

        // Select node
        if ( m_primaryGraphView.GetViewedGraph()->FindNode( pNode->GetID() ) )
        {
            m_primaryGraphView.SelectNode( pNode );
        }
        else if ( m_secondaryGraphView.GetViewedGraph() != nullptr && m_secondaryGraphView.GetViewedGraph()->FindNode( pNode->GetID() ) )
        {
            m_secondaryGraphView.SelectNode( pNode );
        }
    }

    void GraphEditor::NavigateTo( VisualGraph::BaseGraph* pGraph )
    {
        if ( m_primaryGraphView.GetViewedGraph() != pGraph )
        {
            m_primaryGraphView.SetGraphToView( pGraph );
            m_primaryViewGraphID = pGraph->GetID();
        }
    }

    void GraphEditor::UpdateSecondaryViewState()
    {
        if ( m_primaryGraphView.HasSelectedNodes() )
        {
            auto const& lastSelectedNode = m_primaryGraphView.GetSelectedNodes().back();
            if ( auto pSecondaryGraph = TryCast<VisualGraph::FlowGraph>( lastSelectedNode.m_pNode->GetSecondaryGraph() ) )
            {
                m_secondaryGraphView.SetGraphToView( pSecondaryGraph );
            }
            else
            {
                m_secondaryGraphView.SetGraphToView( nullptr );
            }
        }
        else
        {
            m_secondaryGraphView.SetGraphToView( nullptr );
        }
    }

    void GraphEditor::UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        bool isGraphViewFocused = false;
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName ) )
        {
            auto pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();

            isGraphViewFocused = ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows );

            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

            // Navigation Bar
            //-------------------------------------------------------------------------

            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 2 ) );
            ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 1, 1 ) );
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 1 ) );
            if( ImGui::BeginChild( "NavBar", ImVec2( ImGui::GetContentRegionAvail().x, 24 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding ) )
            {
                ImVec2 const navBarDimensions = ImGui::GetContentRegionAvail();

                if ( ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_HOME"##GoHome", ImVec2( 20, -1 ) ) )
                {
                    NavigateTo( m_pGraphDefinition->GetRootGraph() );
                }

                //-------------------------------------------------------------------------

                TInlineVector<VisualGraph::BaseGraph*,10> pathToRoot;

                auto pGraph = m_primaryGraphView.GetViewedGraph();
                while ( pGraph != nullptr && pGraph != m_pGraphDefinition->GetRootGraph() )
                {
                    pathToRoot.emplace_back( pGraph );
                    if ( pGraph->GetParentNode() != nullptr )
                    {
                        pGraph = pGraph->GetParentNode()->GetParentGraph();
                    }
                    else
                    {
                        pGraph = nullptr;
                    }
                }

                ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
                for ( auto i = (int32) pathToRoot.size() - 1; i >= 0; i-- )
                {
                    ImGui::SameLine( 0, -1 );
                    ImGui::Text( "/" );

                    InlineString<128> const str( InlineString<128>::CtorSprintf(), "%s##%s", pathToRoot[i]->GetTitle(), pathToRoot[i]->GetID().ToString().c_str() );
                    ImGui::SameLine( 0, -1 );
                    if ( ImGui::Button( str.c_str() ) )
                    {
                        NavigateTo( pathToRoot[i] );
                    }
                }
                ImGui::PopStyleColor();

                //-------------------------------------------------------------------------

                if ( m_primaryGraphView.IsViewingStateMachineGraph() )
                {
                    constexpr static float const buttonWidth = 80;

                    ImGuiX::ScopedFont const sf( ImGuiX::Font::SmallBold );

                    ImGui::SameLine( navBarDimensions.x - buttonWidth * 2, 0 );
                    ImGui::AlignTextToFramePadding();
                    if ( ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_SIGN_IN" Entry", ImVec2( buttonWidth, -1 ) ) )
                    {
                        auto pSM = Cast<StateMachineGraph>( m_primaryGraphView.GetViewedGraph() );
                        NavigateTo( pSM->GetEntryStateOverrideConduit() );
                    }

                    ImGui::SameLine( 0, -1 );
                    if ( ImGuiX::ColoredButton( Colors::OrangeRed, Colors::White, KRG_ICON_BOLT"Global", ImVec2( buttonWidth, -1 ) ) )
                    {
                        auto pSM = Cast<StateMachineGraph>( m_primaryGraphView.GetViewedGraph() );
                        NavigateTo( pSM->GetGlobalTransitionConduit() );
                    }
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleVar( 3 );

            // Primary View
            //-------------------------------------------------------------------------

            m_primaryGraphView.UpdateAndDraw( *pTypeRegistry, m_primaryGraphViewHeight, pDebugContext );

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
            m_secondaryGraphView.UpdateAndDraw( *pTypeRegistry, 0.0f, pDebugContext );

            //-------------------------------------------------------------------------

            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar();

        // Handle Focus and selection 
        //-------------------------------------------------------------------------

        HandleFocusAndSelectionChanges();
    }

    void GraphEditor::HandleFocusAndSelectionChanges()
    {
        GraphEditor::GraphView* pCurrentlyFocusedGraphView = nullptr;

        // Get the currently focused view
        //-------------------------------------------------------------------------

        if ( m_primaryGraphView.HasFocus() )
        {
            pCurrentlyFocusedGraphView = &m_primaryGraphView;
        }
        else if ( m_secondaryGraphView.HasFocus() )
        {
            pCurrentlyFocusedGraphView = &m_secondaryGraphView;
        }

        // Has the focus within the graph editor tool changed?
        //-------------------------------------------------------------------------

        if ( pCurrentlyFocusedGraphView != nullptr && pCurrentlyFocusedGraphView != m_pFocusedGraphView )
        {
            m_pFocusedGraphView = pCurrentlyFocusedGraphView;
        }
    }

    void GraphEditor::ClearSelection()
    {
        m_primaryGraphView.ClearSelection();
        m_secondaryGraphView.ClearSelection();
    }
}