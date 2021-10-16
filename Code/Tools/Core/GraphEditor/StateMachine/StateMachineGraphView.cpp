#include "StateMachineGraphView.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    constexpr static float const g_transitionArrowWidth = 3.0f;
    constexpr static float const g_transitionArrowOffset = 4.0f;
    constexpr static float const g_spacingBetweenTitleAndNodeContents = 6.0f;

    //-------------------------------------------------------------------------

    void StateMachineGraphView::SetGraphToView( StateMachineGraph* pGraph )
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

    void StateMachineGraphView::ResetInternalState()
    {
        BaseGraphView::ResetInternalState();
        m_pHoveredNode = nullptr;
        m_contextMenuState.Reset();
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void StateMachineGraphView::DrawNodeTitle( DrawingContext const& ctx, SM::Node* pNode, ImVec2& newNodeSize )
    {
        KRG_ASSERT( pNode != nullptr );

        ImGui::BeginGroup();
        ImGuiX::ScopedFont fontOverride( ImGuiX::Font::Small, ImColor( SM::VisualSettings::s_nodeTitleColor ) );
        ImGui::Text( pNode->GetDisplayName() ); 
        ImGui::EndGroup();

        newNodeSize = pNode->m_titleRectSize = ImGui::GetItemRectSize();

        ImGui::SetCursorPosY( ImGui::GetCursorPos().y + g_spacingBetweenTitleAndNodeContents );
        newNodeSize.y += g_spacingBetweenTitleAndNodeContents;
    }

    void StateMachineGraphView::DrawNodeBackground( DrawingContext const& ctx, SM::Node* pNode, ImVec2& newNodeSize )
    {
        KRG_ASSERT( pNode != nullptr );

        ImVec2 const windowNodePosition = ctx.m_windowRect.Min - ctx.m_viewOffset + pNode->m_canvasPosition;
        ImVec2 const nodeMargin = pNode->GetNodeMargin();
        ImVec2 const rectMin = windowNodePosition - nodeMargin;
        ImVec2 const rectMax = windowNodePosition + pNode->m_size + nodeMargin;
        ImVec2 const rectTitleBarMax = windowNodePosition + ImVec2( newNodeSize.x, pNode->m_titleRectSize.y ) + nodeMargin;

        // Colors
        //-------------------------------------------------------------------------

        ImColor nodeTitleColor( pNode->GetNodeColor() );
        ImColor const nodeBackgroundColor( VisualSettings::s_genericNodeBackgroundColor );

        // Border
        //-------------------------------------------------------------------------

        bool drawBorder = false;
        ImColor nodeBorderColor( pNode->GetNodeColor() );

        if ( pNode->m_ID == m_pGraph->m_entryStateID )
        {
            nodeBorderColor = SM::VisualSettings::s_defaultStateColor;
            nodeTitleColor = SM::VisualSettings::s_defaultStateColor;
            drawBorder = true;
        }
        
        if ( IsNodeSelected( pNode ) || pNode->m_isHovered )
        {
            nodeBorderColor = VisualSettings::s_genericSelectionColor;
            drawBorder = true;
        }

        // Draw
        //-------------------------------------------------------------------------

        if ( IsOfType<SM::State>( pNode ) )
        {
            ctx.m_pDrawList->AddRectFilled( rectMin, rectMax, nodeBackgroundColor, 3, ImDrawFlags_RoundCornersAll );
            ctx.m_pDrawList->AddRectFilled( rectMin, rectTitleBarMax, nodeTitleColor, 3, ImDrawFlags_RoundCornersTop );
            
            if ( drawBorder )
            {
                ctx.m_pDrawList->AddRect( rectMin, rectMax, nodeBorderColor, 3, ImDrawFlags_RoundCornersAll, 1.0f );
            }
        }
        else // Non-state node
        {
            ctx.m_pDrawList->AddRectFilled( rectMin, rectMax, nodeBackgroundColor, 3 );
            ctx.m_pDrawList->AddRect( rectMin, rectMax, nodeBorderColor, 3, ImDrawFlags_RoundCornersAll, 1.0f );
        }
    }

    void StateMachineGraphView::DrawNode( DrawingContext const& ctx, SM::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        ImGui::PushID( pNode );

        //-------------------------------------------------------------------------
        // Draw contents
        //-------------------------------------------------------------------------

        ctx.m_pDrawList->ChannelsSetCurrent( 2 );

        ImVec2 newNodeSize( 0, 0 );
        ImGui::SetCursorPos( ImVec2( pNode->m_canvasPosition ) - ctx.m_viewOffset );
        ImGui::BeginGroup();
        {
            DrawNodeTitle( ctx, pNode, newNodeSize );

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
        
        ctx.m_pDrawList->ChannelsSetCurrent( 1 );
        DrawNodeBackground( ctx, pNode, newNodeSize );
        pNode->m_size = newNodeSize;

        //-------------------------------------------------------------------------

        ImGui::PopID();

        //-------------------------------------------------------------------------

        pNode->m_isHovered = GetNodeCanvasRect( pNode ).Contains( ctx.m_mouseCanvasPos );
    }

    //-------------------------------------------------------------------------

    static void GetConnectionPointsBetweenNodes( ImRect const& startNodeRect, ImRect const& endNodeRect, ImVec2& startPoint, ImVec2& endPoint )
    {
        startPoint = startNodeRect.GetCenter();
        endPoint = endNodeRect.GetCenter();
        ImVec2 const midPoint = startPoint + ( ( endPoint - startPoint ) / 2 );

        startPoint = ImGuiX::GetClosestPointOnRect( startNodeRect, midPoint );
        endPoint = ImGuiX::GetClosestPointOnRect( endNodeRect, midPoint );
    }

    void StateMachineGraphView::DrawTransitionConduit( DrawingContext const& ctx, SM::TransitionConduit* pTransition )
    {
        KRG_ASSERT( pTransition != nullptr );
        KRG_ASSERT( pTransition->m_startStateID.IsValid() && pTransition->m_endStateID.IsValid() );

        auto pStartState = Cast<SM::State>( m_pGraph->FindNode( pTransition->m_startStateID ) );
        auto pEndState = Cast<SM::State>( m_pGraph->FindNode( pTransition->m_endStateID ) );

        auto const startNodeRect = GetNodeCanvasRect( pStartState );
        auto const endNodeRect = GetNodeCanvasRect( pEndState );

        // Calculate transition arrow start and end points
        //-------------------------------------------------------------------------

        ImVec2 startPoint, endPoint;
        GetConnectionPointsBetweenNodes( startNodeRect, endNodeRect, startPoint, endPoint );

        Vector const arrowDir = Vector( endPoint - startPoint ).GetNormalized2();
        Vector const orthogonalDir = arrowDir.Orthogonal2D();
        ImVec2 const offset = ( orthogonalDir * g_transitionArrowOffset ).ToFloat2();

        startPoint += offset;
        endPoint += offset;

        // Check hover state
        //-------------------------------------------------------------------------

        pTransition->m_isHovered = false;
        ImColor transitionColor = pTransition->GetNodeColor();
        ImVec2 const closestPointOnTransitionToMouse = ImLineClosestPoint( startPoint, endPoint, ctx.m_mouseCanvasPos );
        if ( ImLengthSqr( ctx.m_mouseCanvasPos - closestPointOnTransitionToMouse ) < Math::Pow( VisualSettings::s_connectionSelectionExtraRadius, 2 ) )
        {
            transitionColor = VisualSettings::s_connectionColorHovered;
            pTransition->m_isHovered = true;
        }

        if ( IsNodeSelected( pTransition ) )
        {
            transitionColor = VisualSettings::s_genericSelectionColor;
        }

        // Draw
        //-------------------------------------------------------------------------

        ImGuiX::DrawArrow( ctx.m_pDrawList, ctx.CanvasPositionToScreenPosition( startPoint ), ctx.CanvasPositionToScreenPosition( endPoint ), transitionColor, g_transitionArrowWidth );

        // Update transition position and size
        //-------------------------------------------------------------------------

        ImVec2 const min( Math::Min( startPoint.x, endPoint.x ), Math::Min( startPoint.y, endPoint.y ) );
        ImVec2 const max( Math::Max( startPoint.x, endPoint.x ), Math::Max( startPoint.y, endPoint.y ) );
        pTransition->m_canvasPosition = min;
        pTransition->m_size = max - min;
    }

    //-------------------------------------------------------------------------

    void StateMachineGraphView::Draw( float childHeightOverride, void* pUserContext )
    {
        if ( BeginDrawCanvas( childHeightOverride ) && m_pGraph != nullptr )
        {
            auto pWindow = ImGui::GetCurrentWindow();
            ImVec2 const mousePos = ImGui::GetMousePos();

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

            // Draw nodes
            for ( auto pNode : m_pGraph->m_nodes )
            {
                auto pStateMachineNode = Cast<SM::Node>( pNode );

                ImRect nodeCanvasRect = GetNodeCanvasRect( pStateMachineNode );

                // If we have a rect width, perform culling
                if ( pStateMachineNode->m_size.x > 0 )
                {
                    if ( !drawingContext.IsItemVisible( nodeCanvasRect ) )
                    {
                        pStateMachineNode->m_isHovered = false;
                        continue;
                    }
                }

                if ( auto pTransition = TryCast<SM::TransitionConduit>( pNode ) )
                {
                    DrawTransitionConduit( drawingContext, pTransition );
                }
                else
                {
                    DrawNode( drawingContext, pStateMachineNode );
                }

                if ( pStateMachineNode->m_isHovered )
                {
                    m_pHoveredNode = pStateMachineNode;
                }
            }

            drawingContext.m_pDrawList->ChannelsMerge();

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

    void StateMachineGraphView::HandleClicks( DrawingContext const& ctx )
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
                if ( m_pHoveredNode != nullptr && IsOfType<SM::TransitionConduit>( m_pHoveredNode ) )
                {
                    ClearSelection();
                    m_pGraph->DestroyNode( m_pHoveredNode->GetID() );
                    m_pHoveredNode = nullptr;
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

    void StateMachineGraphView::HandleContextMenu( DrawingContext const& ctx )
    {
        if ( ctx.IsMouseInViewWindow() && ImGui::IsMouseReleased( ImGuiMouseButton_Right ) )
        {
            m_contextMenuState.m_mouseCanvasPos = ctx.m_mouseCanvasPos;
            m_contextMenuState.m_pNode = m_pHoveredNode;
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
                DrawContextMenu( m_contextMenuState.m_mouseCanvasPos, m_contextMenuState.m_pNode );
                ImGui::EndPopup();
            }
            else
            {
                m_contextMenuState.Reset();
            }
            ImGui::PopStyleVar( 2 );
        }
    }

    void StateMachineGraphView::HandleDragging( DrawingContext const& ctx )
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
                        if ( ImGui::GetIO().KeyAlt && IsOfType<SM::State>( m_pHoveredNode ) )
                        {
                            StartDraggingConnection( ctx );
                        }
                        else if( !IsOfType<SM::TransitionConduit>( m_pHoveredNode ) )
                        {
                            StartDraggingNode( ctx );
                        }
                    }
                    else
                    {
                        StartDraggingSelection( ctx );
                    }
                }
                else if ( ImGui::IsMouseDragging( ImGuiMouseButton_Middle ) )
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

    void StateMachineGraphView::DrawContextMenu( ImVec2 const& mouseCanvasPos, SM::Node* pNode )
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

            DrawContextMenuForNode( mouseCanvasPos, pNode );

            //-------------------------------------------------------------------------

            if ( pNode->IsDestroyable() && m_pGraph->CanDeleteNode( pNode ) )
            {
                ImGui::Separator();

                if ( ImGui::MenuItem( "Delete Node" ) )
                {
                    ClearSelection();
                    pNode->Destroy();
                    m_contextMenuState.Reset();
                    return;
                }
            }
        }
        else
        {
            DrawContextMenuForGraph( mouseCanvasPos );
        }
    }

    void StateMachineGraphView::DrawContextMenuForGraph( ImVec2 const& mouseCanvasPos )
    {
        ImGui::Text( "No Options" );
    }

    //-------------------------------------------------------------------------

    void StateMachineGraphView::StartDraggingView( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::View;
        m_dragState.m_startValue = m_viewOffset;
    }

    void StateMachineGraphView::OnDragView( DrawingContext const& ctx )
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

    void StateMachineGraphView::StopDraggingView( DrawingContext const& ctx )
    {
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void StateMachineGraphView::StartDraggingSelection( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Selection;
        m_dragState.m_startValue = ImGui::GetMousePos();
    }

    void StateMachineGraphView::OnDragSelection( DrawingContext const& ctx )
    {
        if ( !ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
        {
            StopDraggingSelection( ctx );
            return;
        }

        ctx.m_pDrawList->AddRectFilled( m_dragState.m_startValue, ImGui::GetMousePos(), VisualSettings::s_selectionBoxFillColor );
        ctx.m_pDrawList->AddRect( m_dragState.m_startValue, ImGui::GetMousePos(), VisualSettings::s_selectionBoxOutlineColor );
    }

    void StateMachineGraphView::StopDraggingSelection( DrawingContext const& ctx )
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

    void StateMachineGraphView::StartDraggingNode( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Node;
        m_dragState.m_pNode = m_pHoveredNode;
        m_dragState.m_startValue = m_pHoveredNode->m_canvasPosition;
    }

    void StateMachineGraphView::OnDragNode( DrawingContext const& ctx )
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
            auto pFlowNode = static_cast<SM::Node*>( pSelectedNode );
            pFlowNode->m_canvasPosition = pFlowNode->m_canvasPosition + frameDragDelta;
        }
    }

    void StateMachineGraphView::StopDraggingNode( DrawingContext const& ctx )
    {
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void StateMachineGraphView::StartDraggingConnection( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Connection;
        m_dragState.m_pNode = m_pHoveredNode;
        m_dragState.m_startValue = ctx.CanvasPositionToScreenPosition( m_pHoveredNode->GetCanvasPosition() );
    }

    void StateMachineGraphView::OnDragConnection( DrawingContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::Connection );

        if ( !ImGui::IsMouseDragging( ImGuiMouseButton_Left, 1 ) )
        {
            StopDraggingConnection( ctx );
            return;
        }

        //-------------------------------------------------------------------------

        ImColor connectionColor = VisualSettings::s_connectionColorInvalid;

        auto pEndState = TryCast<SM::State>( m_pHoveredNode );
        if ( pEndState != nullptr && m_pGraph->CanCreateTransitionConduit( Cast<SM::State>( m_dragState.m_pNode ), pEndState ) )
        {
            connectionColor = VisualSettings::s_connectionColorValid;
        }

        //-------------------------------------------------------------------------

        ImGuiX::DrawArrow( ctx.m_pDrawList, ctx.CanvasPositionToScreenPosition( GetNodeCanvasRect( m_dragState.m_pNode ).GetCenter() ), ctx.m_mouseScreenPos, connectionColor, g_transitionArrowWidth );
    }

    void StateMachineGraphView::StopDraggingConnection( DrawingContext const& ctx )
    {
        auto pStartState = Cast<SM::State>( m_dragState.m_pNode );
        auto pEndState = TryCast<SM::State>( m_pHoveredNode );
        if ( pEndState != nullptr && m_pGraph->CanCreateTransitionConduit( pStartState, pEndState ) )
        {
            m_pGraph->CreateTransitionConduit( pStartState, pEndState );
        }

        m_dragState.Reset();
    }
}