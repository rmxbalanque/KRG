#include "VisualGraph_View.h"

//-------------------------------------------------------------------------

namespace KRG::VisualGraph
{
    constexpr static float const g_transitionArrowWidth = 3.0f;
    constexpr static float const g_transitionArrowOffset = 4.0f;
    constexpr static float const g_spacingBetweenTitleAndNodeContents = 6.0f;
    constexpr static float const g_pinRadius = 5.0f;
    constexpr static float const g_pinSelectionExtraRadius = 5.0f;
    constexpr static float const g_spacingBetweenInputOutputPins = 16.0f;

    //-------------------------------------------------------------------------

    static void GetConnectionPointsBetweenStateMachineNodes( ImRect const& startNodeRect, ImRect const& endNodeRect, ImVec2& startPoint, ImVec2& endPoint )
    {
        startPoint = startNodeRect.GetCenter();
        endPoint = endNodeRect.GetCenter();
        ImVec2 const midPoint = startPoint + ( ( endPoint - startPoint ) / 2 );

        startPoint = ImGuiX::GetClosestPointOnRect( startNodeRect, midPoint );
        endPoint = ImGuiX::GetClosestPointOnRect( endNodeRect, midPoint );
    }

    static bool IsHoveredOverCurve( ImVec2 const& p1, ImVec2 const& p2, ImVec2 const& p3, ImVec2 const& p4, ImVec2 const& mousePosition, float hoverThreshold )
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

    //-------------------------------------------------------------------------

    void GraphView::SetGraphToView( BaseGraph* pGraph, bool tryMaintainSelection )
    {
        if ( m_pGraph == pGraph )
        {
            return;
        }

        TVector<SelectedNode> oldSelection;
        oldSelection.swap( m_selectedNodes );

        ResetInternalState();
        m_pGraph = pGraph;
        if ( m_pGraph != nullptr )
        {
            m_pGraph->OnShowGraph();
        }

        if ( tryMaintainSelection )
        {
            for ( auto const& oldSelectedNode : oldSelection )
            {
                auto pFoundNode = GetViewedGraph()->FindNode( oldSelectedNode.m_nodeID );
                if ( pFoundNode != nullptr )
                {
                    AddToSelection( pFoundNode );
                }
            }
        }
    }

    void GraphView::ResetInternalState()
    {
        m_viewOffset = ImVec2( 0, 0 );
        m_hasFocus = false;
        m_selectionChanged = false;
        m_pHoveredNode = nullptr;
        m_pHoveredPin = nullptr;
        m_contextMenuState.Reset();
        m_dragState.Reset();
        ClearSelection();
    }

    //-------------------------------------------------------------------------

    bool GraphView::BeginDrawCanvas( float childHeightOverride )
    {
        m_selectionChanged = false;

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGuiX::Style::s_gridBackgroundColor.Value );
        bool const childVisible = ImGui::BeginChild( "GraphCanvas", ImVec2( 0.f, childHeightOverride ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse );
        if ( childVisible )
        {
            if ( ImGui::InvisibleButton( "InvisibleCanvasButton", ImGui::GetContentRegionAvail(), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle | ImGuiButtonFlags_MouseButtonRight ) )
            {
                int i = 5;
            }

            auto pWindow = ImGui::GetCurrentWindow();
            auto pDrawList = ImGui::GetWindowDrawList();

            m_hasFocus = ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows );
            pDrawList->ChannelsSplit( 4 );

            // Background
            //-------------------------------------------------------------------------

            pDrawList->ChannelsSetCurrent( (uint8) DrawChannel::Background );

            ImRect const windowRect = pWindow->Rect();
            ImVec2 const windowTL = windowRect.GetTL();
            float const canvasWidth = windowRect.GetWidth();
            float const canvasHeight = windowRect.GetHeight();

            // Draw Grid
            for ( float x = Math::FModF( 0, VisualSettings::s_gridSpacing ); x < canvasWidth; x += VisualSettings::s_gridSpacing )
            {
                pDrawList->AddLine( windowTL + ImVec2( x, 0.0f ), windowTL + ImVec2( x, canvasHeight ), ImGuiX::Style::s_gridLineColor );
            }

            for ( float y = Math::FModF( 0, VisualSettings::s_gridSpacing ); y < canvasHeight; y += VisualSettings::s_gridSpacing )
            {
                pDrawList->AddLine( windowTL + ImVec2( 0.0f, y ), windowTL + ImVec2( canvasWidth, y ), ImGuiX::Style::s_gridLineColor );
            }

            // Draw title
            auto pViewedGraph = GetViewedGraph();
            if ( pViewedGraph != nullptr )
            {
                ImGuiX::ScopedFont font( ImGuiX::Font::LargeBold );
                pDrawList->AddText( windowRect.Min + VisualSettings::s_graphTitleMargin, VisualSettings::s_graphTitleColor, pViewedGraph->GetTitle() );
            }
        }

        return childVisible;
    }

    void GraphView::EndDrawCanvas()
    {
        ImGui::GetWindowDrawList()->ChannelsMerge();

        //-------------------------------------------------------------------------

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    void GraphView::DrawStateMachineNodeTitle( DrawContext const& ctx, SM::Node* pNode, ImVec2& newNodeSize )
    {
        KRG_ASSERT( pNode != nullptr );

        ImGui::BeginGroup();
        ImGuiX::ScopedFont fontOverride( ImGuiX::Font::Small );
        ImGui::Text( pNode->GetDisplayName() );
        ImGui::EndGroup();

        newNodeSize = pNode->m_titleRectSize = ImGui::GetItemRectSize();

        ImGui::SetCursorPosY( ImGui::GetCursorPos().y + g_spacingBetweenTitleAndNodeContents );
        newNodeSize.y += g_spacingBetweenTitleAndNodeContents;
    }

    void GraphView::DrawStateMachineNodeBackground( DrawContext const& ctx, SM::Node* pNode, ImVec2& newNodeSize )
    {
        KRG_ASSERT( pNode != nullptr );

        ImVec2 const windowNodePosition = ctx.m_windowRect.Min - ctx.m_viewOffset + pNode->m_canvasPosition;
        ImVec2 const nodeMargin = pNode->GetNodeMargin();
        ImVec2 const rectMin = windowNodePosition - nodeMargin;
        ImVec2 const rectMax = windowNodePosition + pNode->m_size + nodeMargin;
        ImVec2 const rectTitleBarMax = windowNodePosition + ImVec2( newNodeSize.x, pNode->m_titleRectSize.y ) + nodeMargin;

        // Colors
        //-------------------------------------------------------------------------

        bool drawBorder = false;
        ImColor const nodeBackgroundColor( VisualSettings::s_genericNodeBackgroundColor );
        ImColor nodeTitleColor( pNode->GetNodeColor() );
        ImColor nodeBorderColor( pNode->GetNodeColor() );

        auto pStateMachineGraph = GetStateMachineGraph();
        if ( pNode->m_ID == pStateMachineGraph->m_entryStateID )
        {
            nodeTitleColor = ImGuiX::ConvertColor( Colors::Green );
        }

        if ( IsNodeSelected( pNode ) )
        {
            nodeBorderColor = VisualSettings::s_genericSelectionColor;
            drawBorder = true;
        }
        else if ( pNode->m_isHovered )
        {
            nodeBorderColor = VisualSettings::s_genericHoverColor;
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
                ctx.m_pDrawList->AddRect( rectMin, rectMax, nodeBorderColor, 3, ImDrawFlags_RoundCornersAll, 2.0f );
            }
        }
        else // Non-state node
        {
            ctx.m_pDrawList->AddRectFilled( rectMin, rectMax, nodeBackgroundColor, 3 );
            ctx.m_pDrawList->AddRect( rectMin, rectMax, nodeBorderColor, 3, ImDrawFlags_RoundCornersAll, 1.0f );
        }
    }

    void GraphView::DrawStateMachineNode( DrawContext const& ctx, SM::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        if ( !pNode->IsVisibleNode() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::PushID( pNode );

        //-------------------------------------------------------------------------
        // Draw contents
        //-------------------------------------------------------------------------

        ctx.m_pDrawList->ChannelsSetCurrent( 2 );

        ImVec2 newNodeSize( 0, 0 );
        ImGui::SetCursorPos( ImVec2( pNode->m_canvasPosition ) - ctx.m_viewOffset );
        ImGui::BeginGroup();
        {
            DrawStateMachineNodeTitle( ctx, pNode, newNodeSize );

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
        DrawStateMachineNodeBackground( ctx, pNode, newNodeSize );
        pNode->m_size = newNodeSize;

        //-------------------------------------------------------------------------

        ImGui::PopID();

        //-------------------------------------------------------------------------

        pNode->m_isHovered = m_hasFocus && GetNodeCanvasRect( pNode ).Contains( ctx.m_mouseCanvasPos );
    }

    void GraphView::DrawStateMachineTransitionConduit( DrawContext const& ctx, SM::TransitionConduit* pTransition )
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
        GetConnectionPointsBetweenStateMachineNodes( startNodeRect, endNodeRect, startPoint, endPoint );

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
        if ( m_hasFocus && ImLengthSqr( ctx.m_mouseCanvasPos - closestPointOnTransitionToMouse ) < Math::Pow( VisualSettings::s_connectionSelectionExtraRadius, 2 ) )
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

    void GraphView::DrawFlowNodeTitle( DrawContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize )
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

    void GraphView::DrawFlowNodePins( DrawContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize )
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

            ImVec2 pinRowRectSize( 0, 0 );

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

    void GraphView::DrawFlowNodeBackground( DrawContext const& ctx, Flow::Node* pNode, ImVec2& newNodeSize )
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

        bool drawBorder = false;
        ImColor nodeBorderColor( pNode->GetNodeColor() );

        if ( IsNodeSelected( pNode ) )
        {
            nodeBorderColor = VisualSettings::s_genericSelectionColor;
            drawBorder = true;
        }
        else if ( pNode->m_isHovered && pNode->m_pHoveredPin == nullptr )
        {
            nodeBorderColor = VisualSettings::s_genericHoverColor;
            drawBorder = true;
        }

        if ( drawBorder )
        {
            ctx.m_pDrawList->AddRect( rectMin, rectMax, nodeBorderColor, 3, ImDrawFlags_RoundCornersAll, 2.0f );
        }
    }

    void GraphView::DrawFlowNode( DrawContext const& ctx, Flow::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        if ( !pNode->IsVisibleNode() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::PushID( pNode );

        //-------------------------------------------------------------------------
        // Draw Node
        //-------------------------------------------------------------------------

        ctx.m_pDrawList->ChannelsSetCurrent( (uint8) DrawChannel::NodeForeground );

        ImVec2 newNodeSize( 0, 0 );
        ImGui::SetCursorPos( ImVec2( pNode->m_canvasPosition ) - ctx.m_viewOffset );
        ImGui::BeginGroup();
        {
            DrawFlowNodeTitle( ctx, pNode, newNodeSize );

            //-------------------------------------------------------------------------

            DrawFlowNodePins( ctx, pNode, newNodeSize );

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
        DrawFlowNodeBackground( ctx, pNode, newNodeSize );
        pNode->m_size = newNodeSize;

        //-------------------------------------------------------------------------

        ImGui::PopID();

        //-------------------------------------------------------------------------

        pNode->m_isHovered = m_hasFocus && GetNodeCanvasRect( pNode ).Contains( ctx.m_mouseCanvasPos ) || pNode->m_pHoveredPin != nullptr;
    }

    void GraphView::UpdateAndDraw( TypeSystem::TypeRegistry const& typeRegistry, float childHeightOverride, void* pUserContext )
    {
        if ( BeginDrawCanvas( childHeightOverride ) && m_pGraph != nullptr )
        {
            auto pWindow = ImGui::GetCurrentWindow();
            ImVec2 const mousePos = ImGui::GetMousePos();

            DrawContext drawingContext;
            drawingContext.m_pDrawList = ImGui::GetWindowDrawList();
            drawingContext.m_viewOffset = m_viewOffset;
            drawingContext.m_windowRect = pWindow->Rect();
            drawingContext.m_canvasVisibleRect = ImRect( m_viewOffset, m_viewOffset + drawingContext.m_windowRect.GetSize() );
            drawingContext.m_mouseScreenPos = ImGui::GetMousePos();
            drawingContext.m_mouseCanvasPos = drawingContext.ScreenPositionToCanvasPosition( drawingContext.m_mouseScreenPos );
            drawingContext.m_pUserContext = pUserContext;

            //-------------------------------------------------------------------------
            // Draw Graph
            //-------------------------------------------------------------------------

            m_pHoveredNode = nullptr;
            m_pHoveredPin = nullptr;

            // State Machine Graph
            if ( IsViewingStateMachineGraph() )
            {
                for ( auto pNode : m_pGraph->m_nodes )
                {
                    // If we have a rect width, perform culling
                    auto pStateMachineNode = Cast<SM::Node>( pNode );
                    if ( pStateMachineNode->GetSize().x > 0 )
                    {
                        ImRect nodeCanvasRect = GetNodeCanvasRect( pStateMachineNode );
                        if ( !drawingContext.IsItemVisible( nodeCanvasRect ) )
                        {
                            pStateMachineNode->m_isHovered = false;
                            continue;
                        }
                    }

                    if ( auto pTransition = TryCast<SM::TransitionConduit>( pNode ) )
                    {
                        DrawStateMachineTransitionConduit( drawingContext, pTransition );
                    }
                    else
                    {
                        DrawStateMachineNode( drawingContext, pStateMachineNode );
                    }

                    if ( pStateMachineNode->m_isHovered )
                    {
                        m_pHoveredNode = pStateMachineNode;
                    }
                }
            }
            else // Flow Graph
            {
                auto pFlowGraph = GetFlowGraph();

                // Draw Nodes
                //-------------------------------------------------------------------------

                for ( auto pNode : m_pGraph->m_nodes )
                {
                    // If we have a rect width, perform culling
                    auto pFlowNode = Cast<Flow::Node>( pNode );
                    if ( pFlowNode->m_size.x > 0 )
                    {
                        ImRect nodeCanvasRect = GetNodeCanvasRect( pFlowNode );
                        if ( !drawingContext.IsItemVisible( nodeCanvasRect ) )
                        {
                            pFlowNode->m_isHovered = false;
                            continue;
                        }
                    }

                    DrawFlowNode( drawingContext, pFlowNode );

                    if ( pFlowNode->m_isHovered )
                    {
                        m_pHoveredNode = pFlowNode;
                        m_pHoveredPin = pFlowNode->m_pHoveredPin;
                    }
                }

                // Draw connections
                //-------------------------------------------------------------------------

                drawingContext.m_pDrawList->ChannelsSetCurrent( (uint8) DrawChannel::Connections );

                m_hoveredConnectionID.Clear();
                for ( auto const& connection : pFlowGraph->m_connections )
                {
                    auto pStartPin = connection.m_pStartNode->GetOutputPin( connection.m_startPinID );
                    auto pEndPin = connection.m_pEndNode->GetInputPin( connection.m_endPinID );

                    ImColor connectionColor = connection.m_pStartNode->GetPinColor( *pStartPin );

                    bool const invertOrder = pStartPin->m_screenPosition.x > pEndPin->m_screenPosition.x;
                    ImVec2 const& p1 = invertOrder ? pEndPin->m_screenPosition : pStartPin->m_screenPosition;
                    ImVec2 const& p4 = invertOrder ? pStartPin->m_screenPosition : pEndPin->m_screenPosition;
                    ImVec2 const p2 = p1 + ImVec2( 50, 0 );
                    ImVec2 const p3 = p4 + ImVec2( -50, 0 );

                    if ( m_hasFocus && IsHoveredOverCurve( p1, p2, p3, p4, drawingContext.m_mouseScreenPos, VisualSettings::s_connectionSelectionExtraRadius ) )
                    {
                        m_hoveredConnectionID = connection.m_ID;
                        connectionColor = ImGuiX::ConvertColor( VisualSettings::s_connectionColorHovered );
                    }

                    drawingContext.m_pDrawList->AddBezierCubic( p1, p2, p3, p4, connectionColor, 3.0f );
                }
            }

            drawingContext.m_pDrawList->ChannelsMerge();

            //-------------------------------------------------------------------------
            // Extra
            //-------------------------------------------------------------------------

            DrawExtraInformation( drawingContext );

            //-------------------------------------------------------------------------

            HandleContextMenu( drawingContext );
            HandleInput( typeRegistry, drawingContext );
        }

        EndDrawCanvas();
    }

    //-------------------------------------------------------------------------

    void GraphView::SelectNode( BaseNode const* pNode )
    {
        KRG_ASSERT( GetViewedGraph()->FindNode( pNode->GetID() ) != nullptr );
        ClearSelection();
        AddToSelection( const_cast<BaseNode*>( pNode ) );
    }

    void GraphView::ClearSelection()
    {
        TVector<SelectedNode> oldSelection;
        oldSelection.swap( m_selectedNodes );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void GraphView::UpdateSelection( BaseNode* pNewSelectedNode )
    {
        KRG_ASSERT( pNewSelectedNode != nullptr );

        // Avoid calling the notification if the selection hasn't changed
        if ( m_selectedNodes.size() == 1 && m_selectedNodes[0].m_pNode == pNewSelectedNode )
        {
            return;
        }

        //-------------------------------------------------------------------------

        TVector<SelectedNode> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.emplace_back( pNewSelectedNode );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void GraphView::UpdateSelection( TVector<SelectedNode>&& newSelection )
    {
        for ( auto& selectedNode : newSelection )
        {
            KRG_ASSERT( selectedNode.m_pNode != nullptr );
        }

        TVector<SelectedNode> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.swap( newSelection );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void GraphView::AddToSelection( BaseNode* pNodeToAdd )
    {
        KRG_ASSERT( pNodeToAdd != nullptr );
        KRG_ASSERT( !IsNodeSelected( pNodeToAdd ) );

        TVector<SelectedNode> oldSelection;
        oldSelection.swap( m_selectedNodes );
        m_selectedNodes.emplace_back( pNodeToAdd );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void GraphView::RemoveFromSelection( BaseNode* pNodeToRemove )
    {
        KRG_ASSERT( pNodeToRemove != nullptr );
        KRG_ASSERT( IsNodeSelected( pNodeToRemove ) );

        TVector<SelectedNode> oldSelection = m_selectedNodes;
        m_selectedNodes.erase_first( pNodeToRemove );
        OnSelectionChangedInternal( oldSelection, m_selectedNodes );
    }

    void GraphView::DestroySelectedNodes()
    {
        auto pGraph = GetViewedGraph();

        ScopedGraphModification sgm( pGraph );
        for ( auto const& selectedNode : m_selectedNodes )
        {
            if ( selectedNode.m_pNode->IsDestroyable() )
            {
                pGraph->DestroyNode( selectedNode.m_nodeID );
            }
        }

        ClearSelection();
    }

    //-------------------------------------------------------------------------

    void GraphView::StartDraggingView( DrawContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::View;
        m_dragState.m_startValue = m_viewOffset;
    }

    void GraphView::OnDragView( DrawContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::View);

        if ( !ImGui::IsMouseDown( ImGuiMouseButton_Middle ) )
        {
            StopDraggingView( ctx );
            return;
        }

        //-------------------------------------------------------------------------

        ImVec2 const mouseDragDelta = ImGui::GetMouseDragDelta( ImGuiMouseButton_Middle );
        m_viewOffset = m_dragState.m_startValue - mouseDragDelta;
    }

    void GraphView::StopDraggingView( DrawContext const& ctx )
    {
        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void GraphView::StartDraggingSelection( DrawContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Selection;
        m_dragState.m_startValue = ImGui::GetMousePos();
        GetViewedGraph()->BeginModification();
    }

    void GraphView::OnDragSelection( DrawContext const& ctx )
    {
        if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            StopDraggingSelection( ctx );
            return;
        }

        ctx.m_pDrawList->AddRectFilled( m_dragState.m_startValue, ImGui::GetMousePos(), ImGuiX::Style::s_selectionBoxFillColor );
        ctx.m_pDrawList->AddRect( m_dragState.m_startValue, ImGui::GetMousePos(), ImGuiX::Style::s_selectionBoxOutlineColor );
    }

    void GraphView::StopDraggingSelection( DrawContext const& ctx )
    {
        ImVec2 const mousePos = ImGui::GetMousePos();
        ImVec2 const min( Math::Min( m_dragState.m_startValue.x, mousePos.x ), Math::Min( m_dragState.m_startValue.y, mousePos.y ) );
        ImVec2 const max( Math::Max( m_dragState.m_startValue.x, mousePos.x ), Math::Max( m_dragState.m_startValue.y, mousePos.y ) );
        ImRect const selectionWindowRect( min - ctx.m_windowRect.Min, max - ctx.m_windowRect.Min );

        TVector<SelectedNode> newSelection;
        for ( auto pNode : GetViewedGraph()->m_nodes )
        {
            auto const nodeWindowRect = GetNodeWindowRect( pNode );
            if ( selectionWindowRect.Overlaps( nodeWindowRect ) )
            {
                newSelection.emplace_back( pNode );
            }
        }

        UpdateSelection( eastl::move( newSelection ) );
        m_dragState.Reset();
        GetViewedGraph()->EndModification();
    }

    //-------------------------------------------------------------------------

    void GraphView::StartDraggingNode( DrawContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        KRG_ASSERT( m_pHoveredNode != nullptr );
        m_dragState.m_mode = DragMode::Node;
        m_dragState.m_pNode = m_pHoveredNode;
        m_dragState.m_startValue = m_pHoveredNode->GetCanvasPosition();

        GetViewedGraph()->BeginModification();
    }

    void GraphView::OnDragNode( DrawContext const& ctx )
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

        for ( auto const& selectedNode : m_selectedNodes )
        {
            auto pNode = selectedNode.m_pNode;
            pNode->SetCanvasPosition( pNode->GetCanvasPosition() + frameDragDelta );
        }
    }

    void GraphView::StopDraggingNode( DrawContext const& ctx )
    {
        m_dragState.Reset();
        GetViewedGraph()->EndModification();
    }

    //-------------------------------------------------------------------------

    void GraphView::StartDraggingConnection( DrawContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::None );
        m_dragState.m_mode = DragMode::Connection;
        m_dragState.m_pNode = m_pHoveredNode;

        if ( IsViewingStateMachineGraph() )
        {
            m_dragState.m_startValue = ctx.CanvasPositionToScreenPosition( m_pHoveredNode->GetCanvasPosition() );
        }
        else
        {
            m_dragState.m_startValue = m_pHoveredPin->m_screenPosition;
            m_dragState.m_pPin = m_pHoveredPin;
        }
    }

    void GraphView::OnDragConnection( DrawContext const& ctx )
    {
        KRG_ASSERT( m_dragState.m_mode == DragMode::Connection );

        if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            StopDraggingConnection( ctx );
            return;
        }

        //-------------------------------------------------------------------------

        if ( IsViewingStateMachineGraph() )
        {
            auto pStateMachineGraph = GetStateMachineGraph();
            auto pEndState = TryCast<SM::State>( m_pHoveredNode );

            bool const isValidConnection = pEndState != nullptr && pStateMachineGraph->CanCreateTransitionConduit( Cast<SM::State>( m_dragState.m_pNode ), pEndState );
            ImColor const connectionColor = isValidConnection ? VisualSettings::s_connectionColorValid : VisualSettings::s_connectionColorInvalid;
            ImGuiX::DrawArrow( ctx.m_pDrawList, ctx.CanvasPositionToScreenPosition( GetNodeCanvasRect( m_dragState.m_pNode ).GetCenter() ), ctx.m_mouseScreenPos, connectionColor, g_transitionArrowWidth );
        }
        else
        {
            auto pFlowGraph = GetFlowGraph();
            auto pDraggedFlowNode = m_dragState.GetAsFlowNode();
            ImColor connectionColor = pDraggedFlowNode->GetPinColor( *m_dragState.m_pPin );

            if ( m_pHoveredPin != nullptr )
            {
                auto pHoveredFlowNode = Cast<Flow::Node>( m_pHoveredNode );

                // Trying to make an invalid connection to a pin with the same direction or the same node
                if ( m_pHoveredPin->m_direction == m_dragState.m_pPin->m_direction || m_pHoveredNode == m_dragState.m_pNode )
                {
                    connectionColor = VisualSettings::s_connectionColorInvalid;
                }
                else // Check connection validity
                {
                    if ( m_dragState.m_pPin->IsOutputPin() )
                    {
                        if ( !pFlowGraph->IsValidConnection( pDraggedFlowNode, m_dragState.m_pPin, pHoveredFlowNode, m_pHoveredPin ) )
                        {
                            connectionColor = VisualSettings::s_connectionColorInvalid;
                        }
                    }
                    else // The hovered pin is the output pin
                    {
                        if ( !pFlowGraph->IsValidConnection( pHoveredFlowNode, m_pHoveredPin, pDraggedFlowNode, m_dragState.m_pPin ) )
                        {
                            connectionColor = VisualSettings::s_connectionColorInvalid;
                        }
                    }
                }
            }

            //-------------------------------------------------------------------------

            bool const invertOrder = m_dragState.m_pPin->m_screenPosition.x > ctx.m_mouseScreenPos.x;
            auto const& p1 = invertOrder ? ctx.m_mouseScreenPos : m_dragState.m_pPin->m_screenPosition;
            auto const& p2 = invertOrder ? m_dragState.m_pPin->m_screenPosition : ctx.m_mouseScreenPos;
            ctx.m_pDrawList->AddBezierCubic( p1, p1 + ImVec2( +50, 0 ), p2 + ImVec2( -50, 0 ), p2, connectionColor, 3.0f );
        }
    }

    void GraphView::StopDraggingConnection( DrawContext const& ctx )
    {
        if ( IsViewingStateMachineGraph() )
        {
            auto pStateMachineGraph = GetStateMachineGraph();
            auto pStartState = Cast<SM::State>( m_dragState.m_pNode );
            auto pEndState = TryCast<SM::State>( m_pHoveredNode );
            if ( pEndState != nullptr && pStateMachineGraph->CanCreateTransitionConduit( pStartState, pEndState ) )
            {
                pStateMachineGraph->CreateTransitionConduit( pStartState, pEndState );
            }
        }
        else
        {
            auto pFlowGraph = GetFlowGraph();

            if( m_pHoveredPin != nullptr && m_pHoveredPin->m_direction != m_dragState.m_pPin->m_direction )
            {
                auto pHoveredFlowNode = Cast<Flow::Node>( m_pHoveredNode );

                if ( m_dragState.m_pPin->IsOutputPin() )
                {
                    pFlowGraph->TryMakeConnection( m_dragState.GetAsFlowNode(), m_dragState.m_pPin, pHoveredFlowNode, m_pHoveredPin );
                }
                else // The hovered pin is the output pin
                {
                    pFlowGraph->TryMakeConnection( pHoveredFlowNode, m_pHoveredPin, m_dragState.GetAsFlowNode(), m_dragState.m_pPin );
                }
            }
            else
            {
                pFlowGraph->BreakAnyConnectionsForPin( m_dragState.m_pPin->m_ID );
            }
        }

        //-------------------------------------------------------------------------

        m_dragState.Reset();
    }

    //-------------------------------------------------------------------------

    void GraphView::HandleContextMenu( DrawContext const& ctx )
    {
        if ( ctx.IsMouseInViewWindow() && ImGui::IsMouseReleased( ImGuiMouseButton_Right ) )
        {
            m_contextMenuState.m_mouseCanvasPos = ctx.m_mouseCanvasPos;
            m_contextMenuState.m_menuOpened = true;
            FillContextMenuState();

            // if we are opening a context menu for a node, set that node as selected
            if ( m_contextMenuState.m_pNode != nullptr )
            {
                UpdateSelection( m_contextMenuState.m_pNode );
            }

            ImGui::OpenPopupEx( GImGui->CurrentWindow->GetID( "GraphContextMenu" ) );
        }

        //-------------------------------------------------------------------------

        if ( IsContextMenuOpen() )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 4 ) );
            if ( ImGui::BeginPopupContextItem( "GraphContextMenu" ) )
            {
                DrawContextMenu();
                ImGui::EndPopup();
            }
            else
            {
                m_contextMenuState.Reset();
            }
            ImGui::PopStyleVar( 2 );
        }
    }

    void GraphView::DrawContextMenuForNode()
    {
        // Do Nothing
    }

    void GraphView::DrawContextMenuForGraph()
    {
        ImGui::Text( "No Options" );
    }

    void GraphView::FillContextMenuState()
    {
        KRG_ASSERT( m_contextMenuState.m_menuOpened );
        m_contextMenuState.m_pNode = m_pHoveredNode;
        m_contextMenuState.m_pPin = m_pHoveredPin;
    }

    void GraphView::DrawContextMenu()
    {
        if ( m_contextMenuState.m_pNode != nullptr )
        {
            if ( ImGui::BeginMenu( "Node Info" ) )
            {
                auto const IDString = m_contextMenuState.m_pNode->GetID().ToString();
                if ( ImGui::MenuItem( IDString.c_str() ) )
                {
                    ImGui::SetClipboardText( IDString.c_str() );
                }
                ImGui::EndMenu();
            }

            //-------------------------------------------------------------------------

            if ( IsViewingFlowGraph() )
            {
                auto pFlowGraph = GetFlowGraph();
                auto pFlowNode = m_contextMenuState.GetAsFlowNode();
                if ( pFlowNode->SupportsDynamicInputPins() )
                {
                    ImGui::Separator();

                    if ( ImGui::MenuItem( "Add Input" ) )
                    {
                        pFlowGraph->CreateDynamicPin( pFlowNode->GetID() );
                    }

                    if ( m_contextMenuState.m_pPin != nullptr && m_contextMenuState.m_pPin->IsDynamicPin() )
                    {
                        if ( ImGui::MenuItem( "Remove Input" ) )
                        {
                            pFlowGraph->DestroyDynamicPin( pFlowNode->GetID(), m_contextMenuState.m_pPin->m_ID );
                            return;
                        }
                    }
                }
            }

            //-------------------------------------------------------------------------

            DrawContextMenuForNode();

            //-------------------------------------------------------------------------

            if ( m_contextMenuState.m_pNode->IsDestroyable() && m_pGraph->CanDeleteNode( m_contextMenuState.m_pNode ) )
            {
                ImGui::Separator();

                if ( ImGui::MenuItem( "Delete Node" ) )
                {
                    ClearSelection();
                    m_contextMenuState.m_pNode->Destroy();
                    m_contextMenuState.Reset();
                    return;
                }
            }
        }
        else
        {
            DrawContextMenuForGraph();
        }
    }

    //-------------------------------------------------------------------------

    void GraphView::HandleInput( TypeSystem::TypeRegistry const& typeRegistry, DrawContext const& ctx )
    {
        if ( !m_hasFocus )
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
                if ( IsViewingFlowGraph() )
                {
                    auto pFlowGraph = GetFlowGraph();

                    if ( m_hoveredConnectionID.IsValid() )
                    {
                        pFlowGraph->BreakConnection( m_hoveredConnectionID );
                    }
                    else if ( m_pHoveredPin != nullptr )
                    {
                        pFlowGraph->BreakAnyConnectionsForPin( m_pHoveredPin->m_ID );
                    }
                }
                else // State Machine
                {
                    if ( m_pHoveredNode != nullptr && IsOfType<SM::TransitionConduit>( m_pHoveredNode ) )
                    {
                        ClearSelection();
                        m_pGraph->DestroyNode( m_pHoveredNode->GetID() );
                        m_pHoveredNode = nullptr;
                    }
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
                else if( ctx.IsMouseInViewWindow() )
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

        switch ( m_dragState.m_mode )
        {
            case DragMode::None:
            {
                if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left, 3 ) )
                {
                    if ( IsViewingFlowGraph() )
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
                    else // State Machine
                    {
                        if ( m_pHoveredNode != nullptr )
                        {
                            if ( ImGui::GetIO().KeyAlt && IsOfType<SM::State>( m_pHoveredNode ) )
                            {
                                StartDraggingConnection( ctx );
                            }
                            else if ( !IsOfType<SM::TransitionConduit>( m_pHoveredNode ) )
                            {
                                StartDraggingNode( ctx );
                            }
                        }
                        else
                        {
                            StartDraggingSelection( ctx );
                        }
                    }
                }
                else if ( ImGui::IsMouseDragging( ImGuiMouseButton_Middle, 3 ) )
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

        // Keyboard
        //-------------------------------------------------------------------------

        if ( ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_C ) ) )
        {
            CopySelectedNodes( typeRegistry );
        }
        else if ( ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_X ) ) )
        {
            CopySelectedNodes( typeRegistry );
            DestroySelectedNodes();
        }
        else if ( ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_V ) ) )
        {
            ImVec2 pasteLocation( 0.0f, 0.0f );

            if ( ctx.IsMouseInViewWindow() )
            {
                pasteLocation = ctx.m_mouseCanvasPos;
            }
            else
            {
                pasteLocation = ctx.m_canvasVisibleRect.GetCenter();
            }

            PasteNodes( typeRegistry, pasteLocation );
        }

        if ( !m_selectedNodes.empty() && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Delete ) ) )
        {
            DestroySelectedNodes();
        }
    }

    //-------------------------------------------------------------------------

    void GraphView::CopySelectedNodes( TypeSystem::TypeRegistry const& typeRegistry )
    {
        if ( m_selectedNodes.empty() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        JsonWriter jsonSerializer;
        auto pWriter = jsonSerializer.GetWriter();

        pWriter->StartObject();

        // Copy Nodes
        //-------------------------------------------------------------------------

        pWriter->Key( s_copiedNodesKey );
        pWriter->StartArray();

        TInlineVector<UUID, 10> copiedNodes;
        for ( auto const& selectedNode : m_selectedNodes )
        {
            if ( selectedNode.m_pNode->IsUserCreatable() )
            {
                selectedNode.m_pNode->Serialize( typeRegistry, *pWriter );
                copiedNodes.emplace_back( selectedNode.m_pNode->GetID() );
            }
        }

        // Ensure that all transitions between copied states are also copied
        if ( IsViewingStateMachineGraph() )
        {
            for ( auto pNode : m_pGraph->m_nodes )
            {
                if ( auto pConduit = TryCast<SM::TransitionConduit>( pNode ) )
                {
                    // If the conduit is already copied, then do nothing
                    if ( VectorContains( copiedNodes, pConduit->GetID() ) )
                    {
                        continue;
                    }

                    // If there exists a non-copied conduit between two copied states, then serialize it
                    if ( VectorContains( copiedNodes, pConduit->GetStartStateID() ) && VectorContains( copiedNodes, pConduit->GetEndStateID() ) )
                    {
                        pConduit->Serialize( typeRegistry, *pWriter );
                    }
                }
            }
        }

        pWriter->EndArray();

        // Serialize node connections
        //-------------------------------------------------------------------------

        if ( IsViewingFlowGraph() )
        {
            auto pFlowGraph = GetFlowGraph();

            pWriter->Key( s_copiedConnectionsKey );
            pWriter->StartArray();

            for ( auto const& connection : pFlowGraph->m_connections )
            {
                if ( VectorContains( copiedNodes, connection.m_pStartNode->GetID() ) && VectorContains( copiedNodes, connection.m_pEndNode->GetID() ) )
                {
                    pFlowGraph->SerializeConnection( *pWriter, connection );
                }
            }

            pWriter->EndArray();
        }

        //-------------------------------------------------------------------------

        pWriter->EndObject();

        //-------------------------------------------------------------------------

        ImGui::SetClipboardText( jsonSerializer.GetStringBuffer().GetString() );
    }

    void GraphView::PasteNodes( TypeSystem::TypeRegistry const& typeRegistry, ImVec2 const& canvasPastePosition )
    {
        JsonReader jsonSerializer;
        if ( !jsonSerializer.ReadFromString( ImGui::GetClipboardText() ) )
        {
            return;
        }

        //-------------------------------------------------------------------------

        auto& document = jsonSerializer.GetDocument();

        auto copiedNodesArrayIter = document.FindMember( s_copiedNodesKey );
        if ( copiedNodesArrayIter == document.MemberEnd() )
        {
            return;
        }

        // Deserialize pasted nodes and regenerated IDs
        //-------------------------------------------------------------------------

        THashMap<UUID, UUID> IDMapping;
        TInlineVector<BaseNode*, 20> pastedNodes;
        for ( auto& nodeObjectValue : copiedNodesArrayIter->value.GetArray() )
        {
            auto pPastedNode = BaseNode::CreateNodeFromSerializedData( typeRegistry, nodeObjectValue, m_pGraph );

            if ( m_pGraph->CanCreateNode( pPastedNode->GetTypeInfo() ) )
            {
                pPastedNode->RegenerateIDs( IDMapping );
                pastedNodes.emplace_back( pPastedNode );
            }
            else
            {
                pPastedNode->Shutdown();
                KRG::Delete( pPastedNode );
            }
        }

        if ( pastedNodes.empty() )
        {
            return;
        }

        // Add nodes to the graph
        //-------------------------------------------------------------------------

        m_pGraph->BeginModification();

        for ( auto pPastedNode : pastedNodes )
        {
            m_pGraph->AddNode( pPastedNode );
        }

        // Serialize and fix connections
        //-------------------------------------------------------------------------

        if ( IsViewingFlowGraph() )
        {
            auto pFlowGraph = GetFlowGraph();
            auto copiedConnectionsArrayIter = document.FindMember( s_copiedConnectionsKey );
            if ( copiedConnectionsArrayIter != document.MemberEnd() )
            {
                for ( auto& connectionObjectValue : copiedConnectionsArrayIter->value.GetArray() )
                {
                    UUID startNodeID = UUID( connectionObjectValue[Flow::Connection::s_startNodeKey].GetString() );
                    UUID endNodeID = UUID( connectionObjectValue[Flow::Connection::s_endNodeKey].GetString() );
                    UUID startPinID = UUID( connectionObjectValue[Flow::Connection::s_startPinKey].GetString() );
                    UUID endPinID = UUID( connectionObjectValue[Flow::Connection::s_endPinKey].GetString() );

                    startNodeID = IDMapping.at( startNodeID );
                    endNodeID = IDMapping.at( endNodeID );
                    startPinID = IDMapping.at( startPinID );
                    endPinID = IDMapping.at( endPinID );

                    Flow::Connection connection;
                    connection.m_pStartNode = pFlowGraph->GetNode( startNodeID );
                    connection.m_pEndNode = pFlowGraph->GetNode( endNodeID );
                    connection.m_startPinID = startPinID;
                    connection.m_endPinID = endPinID;

                    // Only add valid connections (some nodes may have been excluded during the paste)
                    if ( connection.m_pStartNode != nullptr && connection.m_pEndNode != nullptr )
                    {
                        pFlowGraph->m_connections.emplace_back( connection );
                    }
                }
            }
        }
        else // State Machine
        {
            for ( auto pPastedNode : pastedNodes )
            {
                if ( auto pConduit = TryCast<SM::TransitionConduit>( pPastedNode ) )
                {
                    pConduit->m_startStateID = IDMapping.at( pConduit->m_startStateID );
                    pConduit->m_endStateID = IDMapping.at( pConduit->m_endStateID );
                }
            }
        }

        // Updated pasted node positions
        //-------------------------------------------------------------------------

        Float2 leftMostNodePosition( FLT_MAX, FLT_MAX );
        int32 const numPastedNodes = (int32) pastedNodes.size();
        for ( int32 i = 0; i < numPastedNodes; i++ )
        {
            if ( pastedNodes[i]->GetCanvasPosition().m_x < leftMostNodePosition.m_x )
            {
                leftMostNodePosition = pastedNodes[i]->GetCanvasPosition();
            }
        }

        for ( int32 i = 0; i < numPastedNodes; i++ )
        {
            pastedNodes[i]->SetCanvasPosition( pastedNodes[i]->GetCanvasPosition() - leftMostNodePosition + Float2( canvasPastePosition ) );
        }

        // Notify graph that nodes were pasted
        //-------------------------------------------------------------------------

        m_pGraph->PostPasteNodes( pastedNodes );
        m_pGraph->EndModification();
    }
}