#include "TimelineEditor.h"
#include "System/Core/Logging/Log.h"
#include "System/DevTools/ImguiX.h" 

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        static ImColor const g_headerBackgroundColor( 0xFF3D3837 );
        static ImColor const g_headerLineColor( 0xFF606060 );
        static ImColor const g_headerLabelColor( 0xFFBBBBBB );

        static float g_timelineHeaderHeight = 24;
        static float g_timelineControlsWidth = 200;
        static float g_timelineMinimumWidthForLargeInterval = 100;
        static float g_timelineLabelLeftPadding = 4.0f;
        static float g_timelineLargeLineOffset = 4;
        static float g_timelineMediumLineOffset = 10;
        static float g_timelineSmallLineOffset = 16;

        static float g_horizontalScrollbarHeight = 16;
        static float g_verticalScrollbarHeight = 16;

        //-------------------------------------------------------------------------

        static ImVec4 const g_playheadDefaultColor = ImColor::HSV( 0.33f, 0.0f, 0.50f );
        static ImVec4 const g_playheadHoveredColor = Float4( g_playheadDefaultColor ) * 1.20f;
        static ImVec4 const g_playheadShadowColor( 0.0f, 0.0f, 0.0f, 0.1f );
        static ImVec4 const g_playheadBorderColor = Float4( g_playheadDefaultColor ) * 1.25f;
        static ImU32 const g_playheadMarkerLineColor( 0x99999999 );
        static ImU32 const g_timeRangeEndMarkerLineColor( 0x990000FF );

        float const g_playHeadVerticalPadding = 4.0f;
        float const g_playheadHalfWidth = 6.0f;
    }

    TimelineEditor::TimelineEditor( TRange<int32> const& inTimeRange )
        : m_timeRange( inTimeRange )
        , m_viewRange( inTimeRange )
    {
        KRG_ASSERT( inTimeRange.IsSetAndValid() );
    }

    void TimelineEditor::SetPlayState( PlayState newPlayState )
    {
        if ( m_playState == newPlayState )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( newPlayState == PlayState::Playing )
        {
            m_viewUpdateMode = ViewUpdateMode::TrackPlayhead;
            m_playState = PlayState::Playing;
        }
        else
        {
            m_viewUpdateMode = ViewUpdateMode::None;
            m_playState = PlayState::Paused;
        }

        OnPlayStateChanged();
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::DrawTimelineControls( ImRect const& controlsRect )
    {
        static ImVec2 const buttonSize = ImVec2( 20, 0 );
        constexpr static float const buttonSeperation = 2;

        //-------------------------------------------------------------------------

        if ( ImGui::Button( KRG_ICON_FAST_BACKWARD "##GoToStart", buttonSize ) )
        {
            SetViewToStart();
        }

        ImGuiX::ItemTooltip( "Rewind to start" );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );
        if ( m_playState == PlayState::Playing )
        {
            if ( ImGui::Button( KRG_ICON_PAUSE "##Pause", buttonSize ) )
            {
                SetPlayState( PlayState::Paused );
            }

            ImGuiX::ItemTooltip( "Pause" );
        }
        else // Paused
        {
            if ( ImGui::Button( KRG_ICON_PLAY "##Play", buttonSize ) )
            {
                SetPlayState( PlayState::Playing );
            }

            ImGuiX::ItemTooltip( "Play" );
        }

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );
        if ( ImGui::Button( KRG_ICON_FAST_FORWARD "##GoToEnd", buttonSize ) )
        {
            SetViewToEnd();
        }

        ImGuiX::ItemTooltip( "Fast-forward to end" );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );
        if ( ImGuiX::ButtonColored( m_isFrameSnappingEnabled ? ImGuiX::Theme::s_textColor : ImGuiX::Theme::s_textColorDisabled, KRG_ICON_MAGNET "##Snap", buttonSize ) )
        {
            m_isFrameSnappingEnabled = !m_isFrameSnappingEnabled;
        }

        ImGuiX::ItemTooltip( m_isFrameSnappingEnabled ? "Disable frame snapping" : "Enable frame snapping" );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );
        if ( IsLoopingEnabled() )
        {
            if ( ImGui::Button( KRG_ICON_EXCHANGE "##PlayOnce", buttonSize ) )
            {
                m_isLoopingEnabled = false;
            }

            ImGuiX::ItemTooltip( "Disable looping" );
        }
        else // Playing Once
        {
            if ( ImGui::Button( KRG_ICON_LONG_ARROW_RIGHT "##Loop", buttonSize ) )
            {
                m_isLoopingEnabled = true;
            }

            ImGuiX::ItemTooltip( "Enable looping" );
        }

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );
        ImGuiX::ButtonColored( Colors::LimeGreen.ToFloat4(), KRG_ICON_PLUS "##AddTrack", buttonSize );
        ImGuiX::ItemTooltip( "Add Track" );
    }

    void TimelineEditor::DrawTimeline( ImRect const& timelineRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        // Draw timeline
        //-------------------------------------------------------------------------

        int32 const visibleRangeLength = m_viewRange.GetLength();

        int32 numFramesForLargeInterval = 10;
        int32 numFramesForSmallInterval = 1;
        while ( ( numFramesForLargeInterval * m_pixelsPerFrame ) < g_timelineMinimumWidthForLargeInterval )
        {
            numFramesForLargeInterval *= 2;
            numFramesForSmallInterval *= 2;
        };

        int32 const NumFramesForMediumInterval = numFramesForLargeInterval / 2;

        //-------------------------------------------------------------------------

        float startPosX = timelineRect.GetTL().x;
        float startPosY = timelineRect.GetTL().y;
        float endPosX = timelineRect.GetBR().x;
        float endPosY = timelineRect.GetBR().y;

        for ( int32 i = 0; i <= visibleRangeLength; i += numFramesForSmallInterval )
        {
            bool const isLargeLine = ( ( i % numFramesForLargeInterval ) == 0 ) || ( i == m_viewRange.GetLength() || i == 0 );
            bool const isMediumLine = ( i % NumFramesForMediumInterval ) == 0;

            // Draw line
            float const lineOffsetX = startPosX + int32( i * m_pixelsPerFrame );
            float const lineOffsetY = isLargeLine ? g_timelineLargeLineOffset : ( isMediumLine ? g_timelineMediumLineOffset : g_timelineSmallLineOffset );
            if ( lineOffsetX >= startPosX && lineOffsetX <= endPosX )
            {
                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, endPosY ), g_headerLineColor, 1 );
            }

            // Draw text for the base indices
            if ( isLargeLine && lineOffsetX >= startPosX )
            {
                InlineString<256> label;
                label.sprintf( "%d", m_viewRange.m_min + i );
                pDrawList->AddText( ImVec2( lineOffsetX + g_timelineLabelLeftPadding, startPosY ), g_headerLabelColor, label.c_str() );
            }
        }
    }

    void TimelineEditor::DrawPlayhead( ImRect const& playheadRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        float playheadPosX = playheadRect.GetTL().x;
        float playheadPosY = playheadRect.GetTL().y + g_timelineHeaderHeight;

        //-------------------------------------------------------------------------

        float const playheadStartOffsetX = ConvertFramesToPixels( m_playheadTime - m_viewRange.m_min );
        float const playheadHeight = g_timelineHeaderHeight - ( g_playHeadVerticalPadding * 2 );
        ImVec2 const playheadPosition( playheadPosX + playheadStartOffsetX, playheadPosY - g_playHeadVerticalPadding );

        ImGui::SetCursorPos( playheadPosition - ImVec2{ g_playheadHalfWidth, playheadHeight } - ImGui::GetWindowPos() );
        ImGui::SetItemAllowOverlap();
        ImGui::InvisibleButton( "##Playhead", ImVec2( g_playheadHalfWidth * 2.0f, playheadHeight ) );

        // Update playhead position
        //-------------------------------------------------------------------------

        ImVec2 const mousePos = ImGui::GetMousePos();

        // If the mouse is clicked over the header, start dragging operation
        if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
        {
            if ( playheadRect.Contains( mousePos ) && mousePos.y < ( playheadRect.Min.y + g_timelineHeaderHeight ) )
            {
                m_isDraggingPlayhead = true;
            }
        }
        else if ( m_isDraggingPlayhead && ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            // Do Nothing
        }
        else
        {
            m_isDraggingPlayhead = false;
        }

        //-------------------------------------------------------------------------

        if ( m_isDraggingPlayhead )
        {
            // Any play head manipulation, switches back to paused
            if ( IsPlaying() )
            {
                SetPlayState( PlayState::Paused );
            }

            //-------------------------------------------------------------------------

            // The valid range for the playhead, limit it to the current view range but dont let it leave the actual time range
            TRange<float> const playheadValidRange( (float) Math::Max( m_viewRange.m_min, m_timeRange.m_min ), (float) Math::Min( m_viewRange.m_max, m_timeRange.m_max ) );
            m_playheadTime = m_viewRange.m_min + ConvertPixelsToFrames( mousePos.x - playheadRect.Min.x );
            m_playheadTime = playheadValidRange.GetClampedValue( m_playheadTime );

            if ( m_isFrameSnappingEnabled )
            {
                m_playheadTime = Math::Round( m_playheadTime );
            }
        }

        // Draw playhead
        //-------------------------------------------------------------------------

        if ( playheadStartOffsetX < 0 )
        {
            return;
        }

        ImVec4 const playheadColor = ImGui::IsItemHovered() ? g_playheadHoveredColor : g_playheadDefaultColor;

        ImVec2 points[5] =
        {
            playheadPosition,
            playheadPosition - ImVec2{ -g_playheadHalfWidth, playheadHeight / 2.0f },
            playheadPosition - ImVec2{ -g_playheadHalfWidth, playheadHeight },
            playheadPosition - ImVec2{ g_playheadHalfWidth, playheadHeight },
            playheadPosition - ImVec2{ g_playheadHalfWidth, playheadHeight / 2.0f }
        };

        ImVec2 shadow[5];
        for ( int i = 0; i < 5; i++ )
        {
            shadow[i] = points[i] + ImVec2{ 1.0f, 1.0f };
        }

        pDrawList->AddConvexPolyFilled( shadow, 5, ImColor( g_playheadShadowColor ) );
        pDrawList->AddConvexPolyFilled( points, 5, ImColor( playheadColor ) );
        pDrawList->AddPolyline( points, 5, ImColor( g_playheadBorderColor ), true, 1.0f );

        // Draw marker lines
        //-------------------------------------------------------------------------

        if ( m_viewRange.ContainsInclusive( m_timeRange.m_max ) )
        {
            float const pixelOffset = ( m_timeRange.m_max - m_viewRange.m_min ) * m_pixelsPerFrame;
            ImVec2 const endRangeLineStart( playheadRect.GetTL().x + pixelOffset, playheadRect.GetTL().y + g_timelineHeaderHeight );
            ImVec2 const endRangeLineEnd( endRangeLineStart.x, playheadRect.GetBR().y );
            pDrawList->AddLine( endRangeLineStart, endRangeLineEnd, g_timeRangeEndMarkerLineColor );
        }

        pDrawList->AddLine( playheadPosition, ImVec2( playheadPosition.x, playheadRect.GetBR().y ), g_playheadMarkerLineColor );
    }

    void TimelineEditor::Draw()
    {
        ImVec2 const canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 const canvasSize = ImGui::GetContentRegionAvail();

        bool const requiresHorizontalScrollBar = ( m_viewRange.m_min > m_timeRange.m_min || m_viewRange.m_max < m_timeRange.m_max );
        float const horizontalScrollBarHeight = requiresHorizontalScrollBar ? g_horizontalScrollbarHeight : 0.0f;

        //-------------------------------------------------------------------------
        // Manage play state
        //-------------------------------------------------------------------------

        if ( IsPlaying() )
        {
            if ( !m_isLoopingEnabled && m_playheadTime >= m_timeRange.m_max )
            {
                SetPlayState( PlayState::Paused );
            }
        }

        //-------------------------------------------------------------------------
        // Adjust view
        //-------------------------------------------------------------------------

        // Adjust visible range based on the canvas size
        int32 const maxVisibleFrames = Math::FloorToInt( ( canvasSize.x - g_timelineControlsWidth - g_playheadHalfWidth ) / m_pixelsPerFrame );
        if ( m_viewRange.GetLength() > maxVisibleFrames )
        {
            m_viewRange.m_max = m_viewRange.m_min + maxVisibleFrames;
        }

        // Process any update requests
        switch ( m_viewUpdateMode )
        {
            case ViewUpdateMode::GoToStart:
            {
                m_viewRange.m_min = m_timeRange.m_min;
                m_viewRange.m_max = maxVisibleFrames;
                m_playheadTime = (float) m_timeRange.m_min;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::GoToEnd:
            {
                m_viewRange.m_min = Math::Max( m_timeRange.m_min, m_timeRange.m_max - maxVisibleFrames );
                m_viewRange.m_max = m_viewRange.m_min + maxVisibleFrames;
                m_playheadTime = (float) m_timeRange.m_max;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::TrackPlayhead:
            {
                if ( !m_viewRange.ContainsInclusive( (int32) m_playheadTime ) )
                {
                    // If the playhead is in the last visible range
                    if ( m_playheadTime + maxVisibleFrames >= m_timeRange.m_max )
                    {
                        m_viewRange.m_min = m_timeRange.m_max - maxVisibleFrames;
                        m_viewRange.m_max = m_timeRange.m_max;
                    }
                    else
                    {
                        m_viewRange.m_min = (int) m_playheadTime;
                        m_viewRange.m_max = m_viewRange.m_min + maxVisibleFrames;
                    }
                }
            }
            break;
        }

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleColor( ImGuiCol_FrameBg, 0 );
        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 2 ) );
        ImGui::BeginChildFrame( 889, canvasSize );

        //-------------------------------------------------------------------------
        // Header
        //-------------------------------------------------------------------------

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        pDrawList->AddRectFilled( canvasPos, ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + g_timelineHeaderHeight ), g_headerBackgroundColor, 0 );

        ImRect const timelineControlsRect( canvasPos, ImVec2( canvasPos.x + g_timelineControlsWidth, canvasPos.y + g_timelineHeaderHeight ) );

        ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4) g_headerBackgroundColor );
        DrawTimelineControls( timelineControlsRect );
        ImGui::PopStyleColor();

        ImRect const timelineRect( ImVec2( canvasPos.x + g_timelineControlsWidth, canvasPos.y ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + g_timelineHeaderHeight ) );
        DrawTimeline( timelineRect );

        //-------------------------------------------------------------------------
        // Tracks
        //-------------------------------------------------------------------------

        ImRect const trackAreaRect( ImVec2( canvasPos.x + g_timelineControlsWidth, canvasPos.y + g_timelineHeaderHeight ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ) );

        //-------------------------------------------------------------------------
        // Playhead
        //-------------------------------------------------------------------------

        ImRect const playheadRect( ImVec2( canvasPos.x + g_timelineControlsWidth, canvasPos.y ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ) );
        DrawPlayhead( playheadRect );

        //-------------------------------------------------------------------------
        // Horizontal Scrollbar
        //-------------------------------------------------------------------------

        ImRect const horizontalScrollBarRect( ImVec2( canvasPos.x + g_timelineControlsWidth, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y ) );
        float const currentViewSize = m_viewRange.GetLength() * m_pixelsPerFrame;
        float const totalContentSizeNeeded = m_timeRange.GetLength() * m_pixelsPerFrame;
        float scrollbarPosition = m_viewRange.m_min * m_pixelsPerFrame;

        ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
        ImGuiID const horizontalScrollBarID = pWindow->GetID( "#TimelineScrollbarY" );
        if( ImGui::ScrollbarEx( horizontalScrollBarRect, horizontalScrollBarID, ImGuiAxis_X, &scrollbarPosition, currentViewSize, totalContentSizeNeeded, 0 ) )
        {
            int32 const viewRangeOriginalLength = m_viewRange.GetLength();
            m_viewRange.m_min = Math::FloorToInt( scrollbarPosition / m_pixelsPerFrame );
            m_viewRange.m_max = m_viewRange.m_min + viewRangeOriginalLength;
        }

        //-------------------------------------------------------------------------

        ImGui::EndChildFrame();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopID();
    }
}