#include "AnimationGraphEditor_GraphEditor.h"
#include "Tools/Animation/Graph/Nodes/AnimationToolsNode_Parameters.h"
#include "Tools/Animation/Graph/AnimationGraphTools_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    constexpr static float const g_splitterHeight = 4.0f;

    //-------------------------------------------------------------------------

    void GraphEditorView::GraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        KRG_ASSERT( m_pGraph != nullptr );
        auto pToolsGraph = static_cast<ToolsGraph*>( m_pGraph );

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
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pParameter->GetColor() );
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
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pParameter->GetColor() );
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

    void GraphEditorView::GraphView::DrawNodeTypeCategoryContextMenu( ImVec2 const& mouseCanvasPos, ToolsGraph* pGraph, Category<TypeSystem::TypeInfo const*> const& category )
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
            auto pDefaultNode = SafeCast<ToolsNode>( item.m_data->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( pDefaultNode->GetAllowedParentGraphTypes().AreAnyFlagsSet( pGraph->GetType() ) )
            {
                if ( ImGui::MenuItem( item.m_name.c_str() ) )
                {
                    auto pToolsGraph = static_cast<ToolsGraph*>( m_pGraph );
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

    //-------------------------------------------------------------------------

    GraphEditorView::GraphEditorView( EditorModel* pModel )
        : TEditorTool<GraphEditorModel>( pModel )
        , m_primaryFlowGraphView( static_cast<GraphEditorModel&>( *pModel ) )
        , m_secondaryFlowGraphView( static_cast<GraphEditorModel&>( *pModel ) )
    {}

    void GraphEditorView::Initialize( UpdateContext const& context )
    {
        TEditorTool<GraphEditorModel>::Initialize( context );
    }

    void GraphEditorView::Shutdown( UpdateContext const& context )
    {
        TEditorTool<GraphEditorModel>::Shutdown( context );
    }

    void GraphEditorView::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        // Primary graph selection
        //-------------------------------------------------------------------------

        if ( m_model.GetGraph()->IsValid() )
        {
            if ( m_model.GetGraph()->GetRootGraph() != m_primaryFlowGraphView.GetViewedGraph() )
            {
                m_primaryFlowGraphView.SetGraphToView( m_model.GetGraph()->GetRootGraph() );
            }
        }

        // Secondary graph selection
        //-------------------------------------------------------------------------
        if ( m_primaryFlowGraphView.HasSelectedNodes() )
        {
            auto const& selectedNodes = m_primaryFlowGraphView.GetSelectedNodes();
            auto const pLastSelectedNode = static_cast<GraphEditor::Flow::Node*>( selectedNodes.back() );
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

        // Draw
        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 2, 2 ) );
        if ( ImGui::Begin( "Graph Editor" ) )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

            m_primaryFlowGraphView.Draw( m_primaryGraphViewHeight );

            //-------------------------------------------------------------------------
            
            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImGuiX::Theme::s_backgroundColorLight );
            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImGuiX::Theme::s_backgroundColorLight );
            ImGui::Button( "##GraphViewSplitter", ImVec2( -1, g_splitterHeight ) );
            ImGui::PopStyleColor(2);

            if ( ImGui::IsItemHovered() )
            {
                ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNS );
            }

            if ( ImGui::IsItemActive() )
            {
                m_primaryGraphViewHeight += ImGui::GetIO().MouseDelta.y;
                m_primaryGraphViewHeight = Math::Max( 25.0f, m_primaryGraphViewHeight );
            }

            //-------------------------------------------------------------------------

            m_secondaryFlowGraphView.Draw();

            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        HandleFocusChanges();
    }

    void GraphEditorView::HandleFocusChanges()
    {
        if ( m_primaryFlowGraphView.HasFocus() )
        {
            m_model.ClearSelection();
            for ( auto pNode : m_primaryFlowGraphView.GetSelectedNodes() )
            {
                if ( auto pToolsNode = TryCast<ToolsNode>( pNode ) )
                {
                    m_model.AddToSelection( pToolsNode );
                }
            }

            m_lastFocusedGraph = ViewID::Primary;
        }
        else if ( m_secondaryFlowGraphView.HasFocus() )
        {
            m_model.ClearSelection();
            for ( auto pNode : m_secondaryFlowGraphView.GetSelectedNodes() )
            {
                if ( auto pToolsNode = TryCast<ToolsNode>( pNode ) )
                {
                    m_model.AddToSelection( pToolsNode );
                }
            }

            m_lastFocusedGraph = ViewID::Secondary;
        }
    }
}