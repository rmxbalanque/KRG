#include "FlowGraphView.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    void FlowGraphView::SetGraphToView( FlowGraph* pGraph )
    {
        if ( m_pGraph == pGraph )
        {
            return;
        }

        m_viewOffset = ImVec2( 0, 0 );
        m_pHoveredNode = nullptr;
        m_pHoveredPin = nullptr;
        m_contextMenuState.Reset();
        m_dragState.Reset();
        ClearSelection();

        //-------------------------------------------------------------------------

        m_pGraph = pGraph;
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::DrawNode( DrawingContext const& ctx, Flow::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        ImGui::PushID( pNode );

        // Draw contents
        //-------------------------------------------------------------------------

        ctx.m_pDrawList->ChannelsSetCurrent( 2 );
        ImGui::SetCursorPos( ImVec2( pNode->m_canvasPosition ) - ctx.m_viewOffset );
        ImGui::BeginGroup();
        DrawNodeTitle( ctx, pNode );
        DrawNodePins( ctx, pNode );
        pNode->DrawExtraControls( ctx );
        ImGui::EndGroup();

        // Update node size
        //-------------------------------------------------------------------------

        if ( pNode->m_isCalculatingSizes )
        {
            pNode->m_size = ImGui::GetItemRectSize();

            float maxInputPinRowWidth = 0;
            float maxOutputPinRowWidth = 0;

            for ( auto const& pin : pNode->m_inputPins )
            {
                maxInputPinRowWidth = Math::Max( maxInputPinRowWidth, pin.m_estimatedWidth );
            }

            for ( auto const& pin : pNode->m_outputPins )
            {
                maxOutputPinRowWidth = Math::Max( maxOutputPinRowWidth, pin.m_estimatedWidth );
            }

            pNode->m_size.x = Math::Max( pNode->m_size.x, ( maxInputPinRowWidth + maxOutputPinRowWidth + 4 ) );
        }

        //-------------------------------------------------------------------------

        // Draw background
        ctx.m_pDrawList->ChannelsSetCurrent( 1 );
        DrawNodeBackground( ctx, pNode );

        //-------------------------------------------------------------------------

        ImGui::PopID();

        //-------------------------------------------------------------------------

        pNode->m_isHovered = GetNodeCanvasRect( pNode ).Contains( ctx.m_mouseCanvasPos ) || pNode->m_pHoveredPin != nullptr;
        pNode->m_isCalculatingSizes = false;
    }

    void FlowGraphView::DrawNodeTitle( DrawingContext const& ctx, Flow::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        ImGui::Text( pNode->GetDisplayName() );

        if ( pNode->m_isCalculatingSizes )
        {
            ImGui::Separator();
        }
        else
        {
            auto CursorPos = ImGui::GetCursorScreenPos();
            ctx.m_pDrawList->AddLine( CursorPos + ImVec2( 0, -2 ), CursorPos + ImVec2( pNode->m_size.x, -2 ), ImGui::GetColorU32( ImGuiCol_Separator ) );
        }
    }

    void FlowGraphView::DrawNodePins( DrawingContext const& ctx, Flow::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        pNode->m_pHoveredPin = nullptr;

        //-------------------------------------------------------------------------

        ImVec2 const nodeMargin = pNode->GetNodeMargin();

        //-------------------------------------------------------------------------

        int32 const numPinRows = Math::Max( pNode->GetNumInputPins(), pNode->GetNumOutputPins() );
        for ( auto i = 0; i < numPinRows; i++ )
        {
            // Inputs
            //-------------------------------------------------------------------------

            bool const hasInputPin = i < pNode->m_inputPins.size();
            if ( hasInputPin )
            {
                ImGui::BeginGroup();
                ImGui::Text( pNode->m_inputPins[i].m_name.c_str() );
                ImGui::SameLine( 0, 0 );
                pNode->DrawPinControls( pNode->m_inputPins[i] );
                ImGui::EndGroup();

                // Get the size of the above group
                ImRect const rect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
                if ( pNode->m_isCalculatingSizes )
                {
                    // Record estimate pin rect width
                    pNode->m_inputPins[i].m_estimatedWidth = rect.GetWidth();
                }

                // Check hover state
                ImColor pinColor = pNode->GetPinColor( pNode->m_inputPins[i] );
                pNode->m_inputPins[i].m_screenPosition = rect.Min + ImVec2( -nodeMargin.x, rect.GetHeight() / 2 );
                bool const isPinHovered = Vector( pNode->m_inputPins[i].m_screenPosition ).GetDistance2( ImGui::GetMousePos() ) < ( s_pinRadius + s_pinSelectionExtraRadius );
                if ( isPinHovered )
                {
                    pNode->m_pHoveredPin = &pNode->m_inputPins[i];
                    pinColor = ImGuiX::AdjustColorBrightness( pinColor, 1.25f );
                }

                // Draw pin
                ctx.m_pDrawList->AddCircleFilled( pNode->m_inputPins[i].m_screenPosition, s_pinRadius, pinColor );
            }

            // Outputs
            //-------------------------------------------------------------------------

            if ( i < pNode->m_outputPins.size() )
            {
                if ( hasInputPin )
                {
                    ImGui::SameLine( 0, 0 );
                }

                // Add spacer based on estimated sizes
                if ( !pNode->m_isCalculatingSizes )
                {
                    if ( hasInputPin )
                    {
                        ImGui::InvisibleButton( "Spacer", ImVec2( pNode->m_size.x - pNode->m_inputPins[i].m_estimatedWidth - pNode->m_outputPins[i].m_estimatedWidth, 10 ) );
                    }
                    else
                    {
                        ImGui::InvisibleButton( "Spacer", ImVec2( pNode->m_size.x - pNode->m_outputPins[i].m_estimatedWidth, 10 ) );
                    }
                    ImGui::SameLine( 0, 0 );
                }

                //-------------------------------------------------------------------------

                ImGui::BeginGroup();
                pNode->DrawPinControls( pNode->m_outputPins[i] );
                ImGui::SameLine( 0, 0 );
                ImGui::Text( pNode->m_outputPins[i].m_name.c_str() );
                ImGui::EndGroup();

                // Get the size of the above group
                ImRect const rect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
                if ( pNode->m_isCalculatingSizes )
                {
                    // Record estimate pin rect size
                    pNode->m_outputPins[i].m_estimatedWidth = rect.GetWidth();
                }

                // Check hover state
                ImColor pinColor = pNode->GetPinColor( pNode->m_outputPins[i] );
                pNode->m_outputPins[i].m_screenPosition = rect.Max + ImVec2( nodeMargin.x, -rect.GetHeight() / 2 );
                bool const isPinHovered = Vector( pNode->m_outputPins[i].m_screenPosition ).GetDistance2( ImGui::GetMousePos() ) < ( s_pinRadius + s_pinSelectionExtraRadius );
                if ( isPinHovered )
                {
                    pNode->m_pHoveredPin = &pNode->m_outputPins[i];
                    pinColor = ImGuiX::AdjustColorBrightness( pinColor, 1.25f );
                }

                ctx.m_pDrawList->AddCircleFilled( pNode->m_outputPins[i].m_screenPosition, s_pinRadius, pinColor );
            }
        }
    }

    void FlowGraphView::DrawNodeBackground( DrawingContext const& ctx, Flow::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        ImVec2 const windowNodePosition = ctx.m_windowRect.Min - ctx.m_viewOffset + pNode->m_canvasPosition;
        ImVec2 const nodeMargin = pNode->GetNodeMargin();
        ImVec2 const rectMin = windowNodePosition - nodeMargin;
        ImVec2 const rectMax = windowNodePosition + pNode->m_size + nodeMargin;

        ImColor nodeBackgroundColor( Settings::s_nodeBackgroundColor );
        if ( pNode->m_isHovered && pNode->m_pHoveredPin == nullptr ) // Dont highlight when hovering over a pin
        {
            nodeBackgroundColor = Settings::s_nodeBackgroundHoveredColor;
        }
        else if ( IsNodeSelected( pNode ) )
        {
            nodeBackgroundColor = Settings::s_nodeBackgroundSelectedColor;
        }

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        pDrawList->AddRectFilled( rectMin, rectMax, nodeBackgroundColor, 3 );
    }

    //-------------------------------------------------------------------------

    inline bool IsHoveredOverLink( ImVec2 const& p1, ImVec2 const& p2, ImVec2 const& p3, ImVec2 const& p4, ImVec2 const& mousePosition, float hoverThreshold )
    {
        ImVec2 const min = ImVec2( ImMin( p1.x, p4.x ), ImMin( p1.y, p4.y ) );
        ImVec2 const max = ImVec2( ImMax( p1.x, p4.x ), ImMax( p1.y, p4.y ) );

        ImRect rect( min, max );
        rect.Add( p2 );
        rect.Add( p3 );
        rect.Expand( ImVec2( hoverThreshold, hoverThreshold ) );

        if ( rect.Contains( mousePosition ) )
        {
            ImVec2 const closetPointToCurve = ImBezierCubicClosestPointCasteljau( p1, p2, p3, p4, mousePosition, ImGui::GetStyle().CurveTessellationTol );
            if ( Vector( closetPointToCurve ).GetDistance2( mousePosition ) < hoverThreshold )
            {
                return true;
            }
        }

        return false;
    }

    void FlowGraphView::Draw( float childHeightOverride )
    {
        ImGui::PushID( this );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, Settings::s_gridBackgroundColor );
        if( ImGui::BeginChild( "GraphCanvas", ImVec2( 0.f, childHeightOverride ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse ) )
        {
            m_hasFocus = ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows );
            auto pWindow = ImGui::GetCurrentWindow();
            ImVec2 const mousePos = ImGui::GetMousePos();

            DrawingContext drawingContext;
            drawingContext.m_pDrawList = ImGui::GetWindowDrawList();
            drawingContext.m_viewOffset = m_viewOffset;
            drawingContext.m_windowRect = pWindow->Rect();
            drawingContext.m_canvasVisibleRect = ImRect( m_viewOffset, m_viewOffset + drawingContext.m_windowRect.Max );
            drawingContext.m_mouseScreenPos = mousePos;
            drawingContext.m_mouseCanvasPos = drawingContext.ScreenPositionToCanvasPosition( mousePos );

            if ( m_pGraph == nullptr )
            {
                DrawEmptyGrid();
            }
            else
            {
                drawingContext.m_pDrawList->ChannelsSplit( 3 );

                drawingContext.m_pDrawList->ChannelsSetCurrent( 0 );
                DrawCanvasGridAndTitle( drawingContext, m_pGraph->GetTitle() );

                //-------------------------------------------------------------------------
                // Draw Nodes
                //-------------------------------------------------------------------------

                m_pHoveredNode = nullptr;
                m_pHoveredPin = nullptr;

                // Draw nodes
                for ( auto pNode : m_pGraph->m_nodes )
                {
                    auto pFlowNode = SafeCast<Flow::Node>( pNode );

                    ImRect nodeCanvasRect = GetNodeCanvasRect( pFlowNode );

                    // If we have a rect width, perform culling
                    if ( pFlowNode->m_size.x > 0 )
                    {
                        if ( !drawingContext.IsItemVisible( nodeCanvasRect ) )
                        {
                            pFlowNode->m_isHovered = false;
                            continue;
                        }
                    }

                    DrawNode( drawingContext, pFlowNode );
                    if ( pFlowNode->m_isHovered )
                    {
                        m_pHoveredNode = pFlowNode;
                        m_pHoveredPin = pFlowNode->m_pHoveredPin;
                    }
                }

                // Draw connections
                m_hoveredConnectionID.Reset();
                for ( auto const& connection : m_pGraph->m_connections )
                {
                    auto pStartPin = connection.m_pStartNode->GetOutputPin( connection.m_startPinID );
                    auto pEndPin = connection.m_pEndNode->GetInputPin( connection.m_endPinID );

                    ImColor connectionColor = connection.m_pStartNode->GetPinColor( *pStartPin );

                    bool const invertOrder = pStartPin->m_screenPosition.x > pEndPin->m_screenPosition.x;
                    ImVec2 const& p1 = invertOrder ? pEndPin->m_screenPosition : pStartPin->m_screenPosition;
                    ImVec2 const& p4 = invertOrder ? pStartPin->m_screenPosition : pEndPin->m_screenPosition;
                    ImVec2 const p2 = p1 + ImVec2( 50, 0 );
                    ImVec2 const p3 = p4 + ImVec2( -50, 0 );

                    if ( IsHoveredOverLink( p1, p2, p3, p4, drawingContext.m_mouseScreenPos, s_connectionSelectionExtraRadius ) )
                    {
                        m_hoveredConnectionID = connection.m_ID;
                        connectionColor = ImGuiX::ConvertColor( Colors::HotPink );
                    }

                    drawingContext.m_pDrawList->AddBezierCubic( p1, p2, p3, p4, connectionColor, 3.0f );
                }

                drawingContext.m_pDrawList->ChannelsMerge();

                //-------------------------------------------------------------------------

                if ( m_hasFocus )
                {
                    HandleContextMenu( drawingContext );
                    HandleDragging( drawingContext );
                    HandleClicks( drawingContext );
                }
            }
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    void FlowGraphView::HandleClicks( DrawingContext const& ctx )
    {
        if ( !ctx.IsMouseInViewWindow() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
        {
            if ( ImGui::GetIO().KeyCtrl )
            {
                if ( m_pHoveredNode != nullptr )
                {
                    if ( IsNodeSelected( m_pHoveredNode ) )
                    {
                        RemoveFromSelection( m_pHoveredNode );
                    }
                    else
                    {
                        AddToSelection( m_pHoveredNode );
                    }
                }
            }
            else if ( ImGui::GetIO().KeyAlt )
            {
                if ( m_hoveredConnectionID.IsValid() )
                {
                    m_pGraph->BreakConnection( m_hoveredConnectionID );
                }
                else if ( m_pHoveredPin != nullptr )
                {
                    m_pGraph->BreakAnyConnectionsForPin( m_pHoveredPin->m_ID );
                }
            }
            else // No modifier
            {
                if ( m_pHoveredNode != nullptr )
                {
                    if ( !IsNodeSelected( m_pHoveredNode ) )
                    {
                        UpdateSelection( m_pHoveredNode );
                    }
                }
                else
                {
                    ClearSelection();
                }
            }
        }
    }

    void FlowGraphView::HandleContextMenu( DrawingContext const& ctx )
    {
        if ( ctx.IsMouseInViewWindow() && ImGui::IsMouseReleased( ImGuiMouseButton_Right ) )
        {
            m_contextMenuState.m_mouseCanvasPos = ctx.m_mouseCanvasPos;
            m_contextMenuState.m_pNode = m_pHoveredNode;
            m_contextMenuState.m_pPin = m_pHoveredPin;
            m_contextMenuState.m_menuOpened = true;

            // if we are opening a context menu for a node, set that node as selected
            if ( m_pHoveredNode != nullptr )
            {
                UpdateSelection( m_pHoveredNode );
            }

            ImGui::OpenPopupEx( GImGui->CurrentWindow->GetID( "GraphContextMenu" ) );
        }

        if ( IsContextMenuOpen() )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 4 ) );
            if ( ImGui::BeginPopupContextItem( "GraphContextMenu" ) )
            {
                DrawContextMenu( m_contextMenuState.m_mouseCanvasPos, m_contextMenuState.m_pNode, m_contextMenuState.m_pPin );
                ImGui::EndPopup();
            }
            else
            {
                m_contextMenuState.Reset();
            }
            ImGui::PopStyleVar( 2 );
        }
    }

    void FlowGraphView::HandleDragging( DrawingContext const& ctx )
    {
        if ( !ctx.IsMouseInViewWindow() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        switch ( m_dragState.m_mode )
        {
            case DragMode::None:
            {
                if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left, 1 ) )
                {
                    if ( m_pHoveredNode != nullptr )
                    {
                        if ( m_pHoveredPin != nullptr )
                        {
                            StartDraggingConnection();
                        }
                        else
                        {
                            StartDraggingNode();
                        }
                    }
                    else
                    {
                        StartDraggingSelection();
                    }
                }
                else if ( ImGui::IsMouseDragging( ImGuiMouseButton_Middle ) )
                {
                    StartDraggingView();
                }
            }
            break;

            case DragMode::Node:
            {
                OnDragNode( ctx );
            }
            break;

            case DragMode::Connection:
            {
                OnDragConnection( ctx );
            }
            break;

            case DragMode::Selection:
            {
                OnDragSelection( ctx );
            }
            break;

            case DragMode::View:
            {
                OnDragView( ctx );
            }
            break;
        }
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::DrawContextMenu( ImVec2 const& mouseCanvasPos, Flow::Node* pNode, Flow::Pin* pPin )
    {
        if ( pNode != nullptr )
        {
            DrawContextMenuForNode( mouseCanvasPos, pNode );

            if ( pPin != nullptr )
            {
                DrawContextMenuForPin( mouseCanvasPos, pNode, pPin );
            }
        }
        else
        {
            DrawContextMenuForGraph( mouseCanvasPos );
        }
    }

    void FlowGraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        ImGui::Text( "No Options" );
    }

    void FlowGraphView::DrawContextMenuForNode( ImVec2 const& mouseCanvasPos, Flow::Node* pNode )
    {
        if ( ImGui::BeginMenu( "Node Info" ) )
        {
            auto const IDString = pNode->GetID().ToString();
            if ( ImGui::MenuItem( IDString.c_str() ) )
            {
                ImGui::SetClipboardText( IDString.c_str() );
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if ( pNode->IsDestroyable() )
        {
            if ( ImGui::MenuItem( "Delete Node" ) )
            {
                pNode->Destroy();
                return;
            }
        }

        if ( pNode->SupportsDynamicInputPins() )
        {
            if ( ImGui::MenuItem( "Add Input" ) )
            {
                m_pGraph->CreateDynamicPin( pNode->GetID() );
            }

            ImGui::Separator();
        }
    }

    void FlowGraphView::DrawContextMenuForPin( ImVec2 const& mouseCanvasPos, Flow::Node* pNode, Flow::Pin* pPin )
    {
        if ( pPin->IsDynamicPin() )
        {
            ImGui::Separator();

            if ( ImGui::MenuItem( "Remove Input" ) )
            {
                m_pGraph->DestroyDynamicPin( pNode->GetID(), pPin->m_ID );
                return;
            }
        }
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::StartDraggingView()
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::View;
        m_dragState.m_startValue = m_viewOffset;
    }

    void FlowGraphView::OnDragView( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::View );

        if ( !ImGui::IsMouseDragging( ImGuiMouseButton_Middle ) )
        {
            StopDraggingView( ctx );
            return;
        }

        //-------------------------------------------------------------------------

        ImVec2 const mouseDragDelta = ImGui::GetMouseDragDelta( ImGuiMouseButton_Middle );
        m_viewOffset = m_dragState.m_startValue - mouseDragDelta;
    }

    void FlowGraphView::StopDraggingView( DrawingContext const& ctx )
    {
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::StartDraggingSelection()
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Selection;
        m_dragState.m_startValue = ImGui::GetMousePos();
    }

    void FlowGraphView::OnDragSelection( DrawingContext const& ctx )
    {
        if ( !ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
        {
            StopDraggingSelection( ctx );
            return;
        }

        ctx.m_pDrawList->AddRectFilled( m_dragState.m_startValue, ImGui::GetMousePos(), Settings::s_selectionBoxFillColor );
        ctx.m_pDrawList->AddRect( m_dragState.m_startValue, ImGui::GetMousePos(), Settings::s_selectionBoxOutlineColor );
    }

    void FlowGraphView::StopDraggingSelection( DrawingContext const& ctx )
    {
        ImRect const selectionWindowRect( m_dragState.m_startValue - ctx.m_windowRect.Min, ImGui::GetMousePos() - ctx.m_windowRect.Min );

        TVector<BaseNode*> newSelection;
        for ( auto pNode : m_pGraph->m_nodes )
        {
            auto const nodeWindowRect = GetNodeWindowRect( pNode );
            if ( selectionWindowRect.Contains( nodeWindowRect ) )
            {
                newSelection.emplace_back( pNode );
            }
        }

        UpdateSelection( eastl::move( newSelection ) );
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::StartDraggingNode()
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Node;
        m_dragState.m_pDraggedNode = m_pHoveredNode;
        m_dragState.m_startValue = m_pHoveredNode->m_canvasPosition;
    }

    void FlowGraphView::OnDragNode( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::Node );

        if ( !ImGui::IsMouseDragging( ImGuiMouseButton_Left, 1 ) )
        {
            StopDraggingNode( ctx );
            return;
        }

        //-------------------------------------------------------------------------

        ImVec2 const mouseDragDelta = ImGui::GetMouseDragDelta( ImGuiMouseButton_Left );
        Float2 const frameDragDelta = mouseDragDelta - m_dragState.m_lastFrameDragDelta;
        m_dragState.m_lastFrameDragDelta = mouseDragDelta;

        for ( auto pSelectedNode : m_selectedNodes )
        {
            auto pFlowNode = static_cast<Flow::Node*>( pSelectedNode );
            pFlowNode->m_canvasPosition = pFlowNode->m_canvasPosition + frameDragDelta;
        }
    }

    void FlowGraphView::StopDraggingNode( DrawingContext const& ctx )
    {
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::StartDraggingConnection()
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Connection;
        m_dragState.m_pDraggedNode = m_pHoveredNode;
        m_dragState.m_startValue = m_pHoveredPin->m_screenPosition;
        m_dragState.m_pDraggedPin = m_pHoveredPin;
    }

    void FlowGraphView::OnDragConnection( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::Connection );

        if ( !ImGui::IsMouseDragging( ImGuiMouseButton_Left, 1 ) )
        {
            StopDraggingConnection( ctx );
            return;
        }

        //-------------------------------------------------------------------------

        ImColor connectionColor = m_dragState.m_pDraggedNode->GetPinColor( *m_dragState.m_pDraggedPin );

        if ( m_pHoveredPin != nullptr )
        {
            if ( m_pHoveredPin->m_direction == m_dragState.m_pDraggedPin->m_direction || m_pHoveredNode == m_dragState.m_pDraggedNode )
            {
                connectionColor = Settings::s_invalidConnectionColor;
            }
            else // Check connection validity
            {
                if ( m_dragState.m_pDraggedPin->IsOutputPin() )
                {
                    if ( !m_pGraph->IsValidConnection( m_dragState.m_pDraggedNode, m_dragState.m_pDraggedPin, m_pHoveredNode, m_pHoveredPin ) )
                    {
                        connectionColor = Settings::s_invalidConnectionColor;
                    }
                }
                else // The hovered pin is the output pin
                {
                    if ( !m_pGraph->IsValidConnection( m_pHoveredNode, m_pHoveredPin, m_dragState.m_pDraggedNode, m_dragState.m_pDraggedPin ) )
                    {
                        connectionColor = Settings::s_invalidConnectionColor;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        bool const invertOrder = m_dragState.m_pDraggedPin->m_screenPosition.x > ctx.m_mouseScreenPos.x;
        auto const& p1 = invertOrder ? ctx.m_mouseScreenPos : m_dragState.m_pDraggedPin->m_screenPosition;
        auto const& p2 = invertOrder ? m_dragState.m_pDraggedPin->m_screenPosition : ctx.m_mouseScreenPos;
        ctx.m_pDrawList->AddBezierCubic( p1, p1 + ImVec2( +50, 0 ), p2 + ImVec2( -50, 0 ), p2, connectionColor, 3.0f );
    }

    void FlowGraphView::StopDraggingConnection( DrawingContext const& ctx )
    {
        if ( m_pHoveredPin != nullptr && m_pHoveredPin->m_direction != m_dragState.m_pDraggedPin->m_direction )
        {
            if ( m_dragState.m_pDraggedPin->IsOutputPin() )
            {
                m_pGraph->TryMakeConnection( m_dragState.m_pDraggedNode, m_dragState.m_pDraggedPin, m_pHoveredNode, m_pHoveredPin );
            }
            else // The hovered pin is the output pin
            {
                m_pGraph->TryMakeConnection( m_pHoveredNode, m_pHoveredPin, m_dragState.m_pDraggedNode, m_dragState.m_pDraggedPin );
            }
        }
        else
        {
            m_pGraph->BreakAnyConnectionsForPin( m_dragState.m_pDraggedPin->m_ID );
        }

        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::ClearSelection()
    {
        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        OnSelectionChanged( oldSelection, m_selectedNodes );
    }

    void FlowGraphView::UpdateSelection( BaseNode* pNewSelectedNode )
    {
        KRG_ASSERT( pNewSelectedNode != nullptr );

        // Avoid calling the notification if the selection hasn't changed
        if ( m_selectedNodes.size() == 1 && m_selectedNodes[0] == pNewSelectedNode )
        {
            return;
        }

        //-------------------------------------------------------------------------

        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.emplace_back( pNewSelectedNode );
        OnSelectionChanged( oldSelection, m_selectedNodes );
    }

    void FlowGraphView::UpdateSelection( TVector<BaseNode*>&& newSelection )
    {
        for ( auto pNode : newSelection )
        {
            KRG_ASSERT( pNode != nullptr );
        }

        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.swap( newSelection );
        OnSelectionChanged( oldSelection, m_selectedNodes );
    }

    void FlowGraphView::AddToSelection( BaseNode* pNodeToAdd )
    {
        KRG_ASSERT( pNodeToAdd != nullptr );
        KRG_ASSERT( !IsNodeSelected( pNodeToAdd ) );

        TVector<BaseNode*> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.emplace_back( pNodeToAdd );
        OnSelectionChanged( oldSelection, m_selectedNodes );
    }

    void FlowGraphView::RemoveFromSelection( BaseNode* pNodeToRemove )
    {
        KRG_ASSERT( pNodeToRemove != nullptr );
        KRG_ASSERT( IsNodeSelected( pNodeToRemove ) );

        TVector<BaseNode*> oldSelection = m_selectedNodes;
        m_selectedNodes.erase_first( pNodeToRemove );
        OnSelectionChanged( oldSelection, m_selectedNodes );
    }
}