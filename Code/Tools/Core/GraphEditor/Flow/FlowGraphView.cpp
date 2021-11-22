#include "FlowGraphView.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    constexpr static float const g_pinRadius = 5.0f;
    constexpr static float const g_pinSelectionExtraRadius = 5.0f;
    constexpr static float const g_spacingBetweenTitleAndNodeContents = 6.0f;
    constexpr static float const g_spacingBetweenInputOutputPins = 16.0f;

    //-------------------------------------------------------------------------

    void FlowGraphView::SetGraphToView( FlowGraph* pGraph )
    {
        if ( m_pGraph == pGraph )
        {
            return;
        }

        ResetInternalState();
        m_pGraph = pGraph;
        if ( m_pGraph != nullptr )
        {
            m_pGraph->OnShowGraph();
        }
    }

    void FlowGraphView::ResetInternalState()
    {
        BaseGraphView::ResetInternalState();
        m_pHoveredNode = nullptr;
        m_pHoveredPin = nullptr;
        m_contextMenuState.Reset();
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void FlowGraphView::DrawNodeTitle( DrawingContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize )
    {
        KRG_ASSERT( pNode != nullptr );

        ImGui::BeginGroup();
        ImGuiX::ScopedFont fontOverride( ImGuiX::Font::Small, ImColor( VisualSettings::s_genericNodeTitleTextColor ) );
        ImGui::Text( pNode->GetDisplayName() );
        ImGui::EndGroup();

        newNodeSize = pNode->m_titleRectSize = ImGui::GetItemRectSize();

        ImGui::SetCursorPosY( ImGui::GetCursorPos().y + g_spacingBetweenTitleAndNodeContents );
        newNodeSize.y += g_spacingBetweenTitleAndNodeContents;
    }

    void FlowGraphView::DrawNodePins( DrawingContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize )
    {
        KRG_ASSERT( pNode != nullptr );

        pNode->m_pHoveredPin = nullptr;

        //-------------------------------------------------------------------------

        ImVec2 const nodeMargin = pNode->GetNodeMargin();

        //-------------------------------------------------------------------------

        ImVec2 pinRectSize( 0, 0 );
        int32 const numPinRows = Math::Max( pNode->GetNumInputPins(), pNode->GetNumOutputPins() );
        for ( auto i = 0; i < numPinRows; i++ )
        {
            bool const hasInputPin = i < pNode->m_inputPins.size();
            bool const hasOutputPin = i < pNode->m_outputPins.size();
            float estimatedSpacingBetweenPins = 0.0f;

            // Input Pin
            //-------------------------------------------------------------------------

            if ( hasInputPin )
            {
                ImGui::BeginGroup();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( pNode->m_inputPins[i].m_name.c_str() );
                if ( pNode->DrawPinControls( pNode->m_inputPins[i] ) )
                {
                    ImGui::SameLine( 0, 0 );
                }
                ImGui::EndGroup();

                // Record estimate pin label rect width and update node size
                pNode->m_inputPins[i].m_size = ImGui::GetItemRectSize();

                // Check hover state
                ImRect const rect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
                ImColor pinColor = pNode->GetPinColor( pNode->m_inputPins[i] );
                pNode->m_inputPins[i].m_screenPosition = rect.Min + ImVec2( -nodeMargin.x, rect.GetHeight() / 2 );
                bool const isPinHovered = Vector( pNode->m_inputPins[i].m_screenPosition ).GetDistance2( ImGui::GetMousePos() ) < ( g_pinRadius + g_pinSelectionExtraRadius );
                if ( isPinHovered )
                {
                    pNode->m_pHoveredPin = &pNode->m_inputPins[i];
                    pinColor = ImGuiX::AdjustColorBrightness( pinColor, 1.25f );
                }

                // Draw pin
                ctx.m_pDrawList->AddCircleFilled( pNode->m_inputPins[i].m_screenPosition, g_pinRadius, pinColor );
            }

            // Update Cursor Position
            //-------------------------------------------------------------------------

            if ( hasOutputPin )
            {
                // Ensure that we correctly move the cursor to the next line for the output pins
                if ( !hasInputPin )
                {
                    ImGui::NewLine();
                }

                float const inputPinWidth = hasInputPin ? pNode->m_inputPins[i].m_size.x : 0;
                estimatedSpacingBetweenPins = pNode->m_size.x - inputPinWidth - pNode->m_outputPins[i].m_size.x;
                estimatedSpacingBetweenPins = Math::Max( estimatedSpacingBetweenPins, g_spacingBetweenInputOutputPins );
                ImGui::SameLine( 0, estimatedSpacingBetweenPins );
            }

            // Output Pin
            //-------------------------------------------------------------------------

            if ( hasOutputPin )
            {
                ImGui::BeginGroup();
                ImGui::AlignTextToFramePadding();
                if ( pNode->DrawPinControls( pNode->m_outputPins[i] ) )
                {
                    ImGui::SameLine( 0, 0 );
                }
                ImGui::Text( pNode->m_outputPins[i].m_name.c_str() );
                ImGui::EndGroup();

                // Get the size of the above group
                pNode->m_outputPins[i].m_size = ImGui::GetItemRectSize();

                // Check hover state
                ImRect const rect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
                ImColor pinColor = pNode->GetPinColor( pNode->m_outputPins[i] );
                pNode->m_outputPins[i].m_screenPosition = rect.Max + ImVec2( nodeMargin.x, -rect.GetHeight() / 2 );
                bool const isPinHovered = Vector( pNode->m_outputPins[i].m_screenPosition ).GetDistance2( ImGui::GetMousePos() ) < ( g_pinRadius + g_pinSelectionExtraRadius );
                if ( isPinHovered )
                {
                    pNode->m_pHoveredPin = &pNode->m_outputPins[i];
                    pinColor = ImGuiX::AdjustColorBrightness( pinColor, 1.25f );
                }

                ctx.m_pDrawList->AddCircleFilled( pNode->m_outputPins[i].m_screenPosition, g_pinRadius, pinColor );
            }

            // Update Rect Size
            //-------------------------------------------------------------------------

            ImVec2 pinRowRectSize(0, 0);

            if ( hasInputPin )
            {
                pinRowRectSize.x += pNode->m_inputPins[i].m_size.x;
                pinRowRectSize.y = pNode->m_inputPins[i].m_size.y;
            }

            if ( hasOutputPin )
            {
                pinRowRectSize.x += pNode->m_outputPins[i].m_size.x;
                pinRowRectSize.y = Math::Max( pinRowRectSize.y, pNode->m_outputPins[i].m_size.y );
            }

            pinRowRectSize.x += estimatedSpacingBetweenPins;
            pinRectSize.x = Math::Max( pinRectSize.x, pinRowRectSize.x );
            pinRectSize.y += pinRowRectSize.y;
        }

        // Update node size with the max width of all output pins
        //-------------------------------------------------------------------------

        newNodeSize.x = Math::Max( newNodeSize.x, pinRectSize.x );
        newNodeSize.y += pinRectSize.y;
    }

    void FlowGraphView::DrawNodeBackground( DrawingContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize )
    {
        KRG_ASSERT( pNode != nullptr );

        ImVec2 const windowNodePosition = ctx.m_windowRect.Min - ctx.m_viewOffset + pNode->m_canvasPosition;
        ImVec2 const nodeMargin = pNode->GetNodeMargin();
        ImVec2 const rectMin = windowNodePosition - nodeMargin;
        ImVec2 const rectMax = windowNodePosition + newNodeSize + nodeMargin;
        ImVec2 const rectTitleBarMax = windowNodePosition + ImVec2( newNodeSize.x, pNode->m_titleRectSize.y ) + nodeMargin;

        // Draw
        //-------------------------------------------------------------------------

        ImColor nodeBackgroundColor( VisualSettings::s_genericNodeBackgroundColor );
        ctx.m_pDrawList->AddRectFilled( rectMin, rectMax, nodeBackgroundColor, 3, ImDrawFlags_RoundCornersAll );
        ctx.m_pDrawList->AddRectFilled( rectMin, rectTitleBarMax, pNode->GetNodeColor(), 3, ImDrawFlags_RoundCornersTop );

        if ( IsNodeSelected( pNode ) || ( pNode->m_isHovered && pNode->m_pHoveredPin == nullptr ) )
        {
            nodeBackgroundColor = VisualSettings::s_genericNodeBackgroundColorSelected;
            ctx.m_pDrawList->AddRect( rectMin, rectMax, VisualSettings::s_genericNodeBorderColorSelected, 3, ImDrawFlags_RoundCornersAll, 1.0f );
        }
    }

    void FlowGraphView::DrawNode( DrawingContext const& ctx, Flow::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        ImGui::PushID( pNode );

        //-------------------------------------------------------------------------
        // Draw Node
        //-------------------------------------------------------------------------

        ctx.m_pDrawList->ChannelsSetCurrent( (uint8) DrawChannel::NodeForeground );

        ImVec2 newNodeSize( 0, 0 );
        ImGui::SetCursorPos( ImVec2( pNode->m_canvasPosition ) - ctx.m_viewOffset );
        ImGui::BeginGroup();
        {
            DrawNodeTitle( ctx, pNode, newNodeSize );

            //-------------------------------------------------------------------------

            DrawNodePins( ctx, pNode, newNodeSize );

            //-------------------------------------------------------------------------

            ImGui::BeginGroup();
            pNode->DrawExtraControls( ctx );
            ImGui::EndGroup();

            ImVec2 const extraControlsRectSize = ImGui::GetItemRectSize();
            newNodeSize.x = Math::Max( newNodeSize.x, extraControlsRectSize.x );
            newNodeSize.y += extraControlsRectSize.y;
        }
        ImGui::EndGroup();

        // Draw background
        //-------------------------------------------------------------------------

        ctx.m_pDrawList->ChannelsSetCurrent( (uint8) DrawChannel::NodeBackground );
        DrawNodeBackground( ctx, pNode, newNodeSize );
        pNode->m_size = newNodeSize;

        //-------------------------------------------------------------------------

        ImGui::PopID();

        //-------------------------------------------------------------------------

        pNode->m_isHovered = GetNodeCanvasRect( pNode ).Contains( ctx.m_mouseCanvasPos ) || pNode->m_pHoveredPin != nullptr;
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

    void FlowGraphView::Draw( float childHeightOverride, void* pUserContext )
    {
        if( BeginDrawCanvas( childHeightOverride ) && m_pGraph != nullptr )
        {
            auto pWindow = ImGui::GetCurrentWindow();

            DrawingContext drawingContext;
            drawingContext.m_pDrawList = ImGui::GetWindowDrawList();
            drawingContext.m_viewOffset = m_viewOffset;
            drawingContext.m_windowRect = pWindow->Rect();
            drawingContext.m_canvasVisibleRect = ImRect( m_viewOffset, m_viewOffset + drawingContext.m_windowRect.Max );
            drawingContext.m_mouseScreenPos = ImGui::GetMousePos();
            drawingContext.m_mouseCanvasPos = drawingContext.ScreenPositionToCanvasPosition( drawingContext.m_mouseScreenPos );
            drawingContext.m_pUserContext = pUserContext;

            //-------------------------------------------------------------------------
            // Draw Nodes
            //-------------------------------------------------------------------------

            m_pHoveredNode = nullptr;
            m_pHoveredPin = nullptr;

            // Draw nodes
            for ( auto pNode : m_pGraph->m_nodes )
            {
                auto pFlowNode = Cast<Flow::Node>( pNode );

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

            //-------------------------------------------------------------------------
            // Draw connections
            //-------------------------------------------------------------------------

            drawingContext.m_pDrawList->ChannelsSetCurrent( (uint8) DrawChannel::Connections );

            m_hoveredConnectionID.Clear();
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

                if ( IsHoveredOverLink( p1, p2, p3, p4, drawingContext.m_mouseScreenPos, VisualSettings::s_connectionSelectionExtraRadius ) )
                {
                    m_hoveredConnectionID = connection.m_ID;
                    connectionColor = ImGuiX::ConvertColor( VisualSettings::s_connectionColorHovered );
                }

                drawingContext.m_pDrawList->AddBezierCubic( p1, p2, p3, p4, connectionColor, 3.0f );
            }

            //-------------------------------------------------------------------------
            // Extra
            //-------------------------------------------------------------------------

            DrawExtraInformation( drawingContext );

            //-------------------------------------------------------------------------

            HandleContextMenu( drawingContext );
            HandleDragging( drawingContext );
            HandleClicks( drawingContext );
        }

        EndDrawCanvas();
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

        if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
        {
            if ( m_pHoveredNode != nullptr )
            {
                OnNodeDoubleClick( m_pHoveredNode );
            }
            else
            {
                OnGraphDoubleClick( m_pGraph );
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
                if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left, 1 ) )
                {
                    if ( m_pHoveredNode != nullptr )
                    {
                        if ( m_pHoveredPin != nullptr )
                        {
                            StartDraggingConnection( ctx );
                        }
                        else
                        {
                            StartDraggingNode( ctx );
                        }
                    }
                    else
                    {
                        StartDraggingSelection( ctx );
                    }
                }
                else if ( ImGui::IsMouseClicked( ImGuiMouseButton_Middle ) )
                {
                    StartDraggingView( ctx );
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
            if ( ImGui::BeginMenu( "Node Info" ) )
            {
                auto const IDString = pNode->GetID().ToString();
                if ( ImGui::MenuItem( IDString.c_str() ) )
                {
                    ImGui::SetClipboardText( IDString.c_str() );
                }
                ImGui::EndMenu();
            }

            //-------------------------------------------------------------------------

            if ( pNode->SupportsDynamicInputPins() )
            {
                ImGui::Separator();

                if ( ImGui::MenuItem( "Add Input" ) )
                {
                    m_pGraph->CreateDynamicPin( pNode->GetID() );
                }

                if ( pPin != nullptr && pPin->IsDynamicPin() )
                {
                    if ( ImGui::MenuItem( "Remove Input" ) )
                    {
                        m_pGraph->DestroyDynamicPin( pNode->GetID(), pPin->m_ID );
                        return;
                    }
                }
            }

            //-------------------------------------------------------------------------

            DrawContextMenuForNode( mouseCanvasPos, pNode, pPin );

            //-------------------------------------------------------------------------

            if ( pNode->IsDestroyable() )
            {
                ImGui::Separator();

                if ( ImGui::MenuItem( "Delete Node" ) )
                {
                    ClearSelection();
                    pNode->Destroy();
                    return;
                }
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

    //-------------------------------------------------------------------------

    void FlowGraphView::StartDraggingView( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::View;
        m_dragState.m_startValue = m_viewOffset;
    }

    void FlowGraphView::OnDragView( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::View );

        if ( !ImGui::IsMouseDown( ImGuiMouseButton_Middle ) )
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

    void FlowGraphView::StartDraggingSelection( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Selection;
        m_dragState.m_startValue = ImGui::GetMousePos();
    }

    void FlowGraphView::OnDragSelection( DrawingContext const& ctx )
    {
        if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            StopDraggingSelection( ctx );
            return;
        }

        ctx.m_pDrawList->AddRectFilled( m_dragState.m_startValue, ImGui::GetMousePos(), ImGuiX::Style::s_selectionBoxFillColor );
        ctx.m_pDrawList->AddRect( m_dragState.m_startValue, ImGui::GetMousePos(), ImGuiX::Style::s_selectionBoxOutlineColor );
    }

    void FlowGraphView::StopDraggingSelection( DrawingContext const& ctx )
    {
        ImVec2 const mousePos = ImGui::GetMousePos();
        ImVec2 const min( Math::Min( m_dragState.m_startValue.x, mousePos.x ), Math::Min( m_dragState.m_startValue.y, mousePos.y ) );
        ImVec2 const max( Math::Max( m_dragState.m_startValue.x, mousePos.x ), Math::Max( m_dragState.m_startValue.y, mousePos.y ) );
        ImRect const selectionWindowRect( min - ctx.m_windowRect.Min, max - ctx.m_windowRect.Min );

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

    void FlowGraphView::StartDraggingNode( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Node;
        m_dragState.m_pDraggedNode = m_pHoveredNode;
        m_dragState.m_startValue = m_pHoveredNode->m_canvasPosition;
    }

    void FlowGraphView::OnDragNode( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::Node );

        if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
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

    void FlowGraphView::StartDraggingConnection( DrawingContext const& ctx )
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

        if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            StopDraggingConnection( ctx );
            return;
        }

        //-------------------------------------------------------------------------

        ImColor connectionColor = m_dragState.m_pDraggedNode->GetPinColor( *m_dragState.m_pDraggedPin );

        if ( m_pHoveredPin != nullptr )
        {
            // Trying to make an invalid connection to a pin with the same direction or the same node
            if ( m_pHoveredPin->m_direction == m_dragState.m_pDraggedPin->m_direction || m_pHoveredNode == m_dragState.m_pDraggedNode )
            {
                connectionColor = VisualSettings::s_connectionColorInvalid;
            }
            else // Check connection validity
            {
                if ( m_dragState.m_pDraggedPin->IsOutputPin() )
                {
                    if ( !m_pGraph->IsValidConnection( m_dragState.m_pDraggedNode, m_dragState.m_pDraggedPin, m_pHoveredNode, m_pHoveredPin ) )
                    {
                        connectionColor = VisualSettings::s_connectionColorInvalid;
                    }
                }
                else // The hovered pin is the output pin
                {
                    if ( !m_pGraph->IsValidConnection( m_pHoveredNode, m_pHoveredPin, m_dragState.m_pDraggedNode, m_dragState.m_pDraggedPin ) )
                    {
                        connectionColor = VisualSettings::s_connectionColorInvalid;
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
}