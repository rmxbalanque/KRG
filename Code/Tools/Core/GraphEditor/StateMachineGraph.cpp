#include "StateMachineGraph.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor::SM
{
    void Node::Draw( DrawingContext const& ctx )
    {
        ImGui::PushID( this );

        // Draw contents
        ctx.m_pDrawList->ChannelsSetCurrent( 2 );
        ImGui::SetCursorPos( m_position - ctx.m_viewOffset );
        ImGui::BeginGroup();
        DrawTitle( ctx );
        DrawPins( ctx );
        DrawControls( ctx );
        ImGui::EndGroup();

        //-------------------------------------------------------------------------

        // Update rect and check for hover
        m_size = ImGui::GetItemRectSize();
        m_isHovered = GetCanvasRect().Contains( ctx.m_mouseCanvasPos );

        // Draw background
        ctx.m_pDrawList->ChannelsSetCurrent( 1 );
        DrawBackground( ctx );

        //-------------------------------------------------------------------------

        if ( m_isHovered )
        {
            HandleInput();
        }

        ImGui::PopID();
    }

    void Node::DrawTitle( DrawingContext const& ctx )
    {
        ImGui::Text( GetTitle() );
    }

    void Node::DrawPins( DrawingContext const& ctx )
    {

    }

    void Node::DrawControls( DrawingContext const& ctx )
    {
        ImGui::Text( "Value: 4" );
    }

    void Node::DrawBackground( DrawingContext const& ctx )
    {
        ImVec2 const windowNodePosition = ctx.m_windowRect.Min - ctx.m_viewOffset + m_position;
        ImVec2 const rectMin = windowNodePosition - GetNodePadding();
        ImVec2 const rectMax = windowNodePosition + m_size + GetNodePadding();

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        pDrawList->AddRectFilled( rectMin, rectMax, m_isHovered ? IM_COL32( 200, 0, 0, 255 ) : IM_COL32( 200, 200, 0, 255 ), 3 );
    }

    void Node::HandleInput()
    {}

    //-------------------------------------------------------------------------

    void Canvas::Draw( Graph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr );

        //-------------------------------------------------------------------------
        // Begin Canvas
        //-------------------------------------------------------------------------

        ImGui::PushStyleColor( ImGuiCol_ChildBg, Settings::s_gridBackgroundColor );
        ImGui::BeginChild( "GraphCanvas", ImVec2( 0.f, 0.f ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse );

        auto pWindow = ImGui::GetCurrentWindow();
        ImVec2 const mousePos = ImGui::GetMousePos();

        DrawingContext drawingContext;
        drawingContext.m_pDrawList = ImGui::GetWindowDrawList();
        drawingContext.m_viewOffset = m_viewOffset;
        drawingContext.m_windowRect = pWindow->Rect();
        drawingContext.m_canvasVisibleRect = ImRect( m_viewOffset, m_viewOffset + drawingContext.m_windowRect.Max );
        drawingContext.m_mouseCanvasPos = drawingContext.ScreenPositionToCanvasPosition( mousePos );

        DrawCanvasGridAndTitle( drawingContext, pGraph->GetTitle() );

        //-------------------------------------------------------------------------
        // Draw canvas title and background
        //-------------------------------------------------------------------------

        drawingContext.m_pDrawList->ChannelsSplit( 3 );

        //-------------------------------------------------------------------------
        // Draw Nodes
        //-------------------------------------------------------------------------

        m_pHoveredNode = nullptr;
        m_pHoveredPin = nullptr;

        for ( auto pNode : pGraph->m_nodes )
        {
            ImRect nodeCanvasRect = pNode->GetCanvasRect();

            // If we have a rect width, perform culling
            if ( pNode->m_size.x > 0 )
            {
                if ( !drawingContext.IsItemVisible( nodeCanvasRect ) )
                {
                    pNode->m_isHovered = false;
                    continue;
                }
            }

            pNode->Draw( drawingContext );
            if ( pNode->m_isHovered )
            {
                m_pHoveredNode = pNode;
                m_pHoveredPin = pNode->m_pHoveredPin;
            }
        }

        drawingContext.m_pDrawList->ChannelsMerge();

        //-------------------------------------------------------------------------
        // End Canvas
        //-------------------------------------------------------------------------

        ImGui::EndChild();
        ImGui::PopStyleColor();

        //-------------------------------------------------------------------------
        // Input
        //-------------------------------------------------------------------------

        if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left, 1 ) )
        {
            if ( m_dragMode == DragMode::None && m_pHoveredNode != nullptr )
            {
                m_dragMode = DragMode::Node;
                m_pDraggedNode = m_pHoveredNode;
                m_dragStartValue = m_pHoveredNode->m_position;
            }
        }
        else if ( ImGui::IsMouseDragging( ImGuiMouseButton_Middle ) )
        {
            if ( m_dragMode == DragMode::None && drawingContext.m_windowRect.Contains( mousePos ) )
            {
                m_dragMode = DragMode::View;
                m_dragStartValue = m_viewOffset;
            }
        }
        else
        {
            m_dragMode = DragMode::None;
            m_pDraggedNode = nullptr;
        }

        //-------------------------------------------------------------------------

        if ( m_dragMode == DragMode::Node )
        {
            ImVec2 const mouseDragDelta = ImGui::GetMouseDragDelta( ImGuiMouseButton_Left );
            m_pDraggedNode->m_position = m_dragStartValue + mouseDragDelta;
        }
        else if ( m_dragMode == DragMode::Connection )
        {

        }
        else if ( m_dragMode == DragMode::View )
        {
            ImVec2 const mouseDragDelta = ImGui::GetMouseDragDelta( ImGuiMouseButton_Middle );
            m_viewOffset = m_dragStartValue - mouseDragDelta;
        }
    }
}