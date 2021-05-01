#include "TimelineEditor.h"
#include "System/Core/Logging/Log.h"
#include "System/DevTools/ImguiX.h" 

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        static float g_headerHeight = 24;
        static float g_trackHeaderWidth = 200;

        static ImColor const g_headerBackgroundColor( 0xFF3D3837 );
        static ImColor const g_headerLabelColor( 0xFFBBBBBB );
        static ImColor const g_timelineLargeLineColor( 0xFF606060 );
        static ImColor const g_timelineMediumLineColor( 0xFF606060 );
        static ImColor const g_timelineSmallLineColor( 0xFF333333 );
        static ImColor const g_timelineRangeEndLineColor( 0x990000FF );

        static float g_timelineMinimumWidthForLargeInterval = 100;
        static float g_timelineLabelLeftPadding = 4.0f;
        static float g_timelineLargeLineOffset = 4;
        static float g_timelineMediumLineOffset = 10;
        static float g_timelineSmallLineOffset = 16;

        //-------------------------------------------------------------------------

        static ImVec4 const g_playheadDefaultColor = ImColor( 0xFF808080 );
        static ImVec4 const g_playheadHoveredColor = Float4( g_playheadDefaultColor ) * 1.20f;
        static ImVec4 const g_playheadShadowColor = ImColor( 0x1A000000 );
        static ImVec4 const g_playheadBorderColor = Float4( g_playheadDefaultColor ) * 1.25f;
        static ImU32 const g_playheadMarkerLineColor( 0x99AAFFAA );

        float const g_playHeadVerticalPadding = 4.0f;
        float const g_playheadHalfWidth = 6.0f;

        //-------------------------------------------------------------------------

        static float g_horizontalScrollbarHeight = 16;
        static float g_verticalScrollbarHeight = 16;

        //-------------------------------------------------------------------------

        static float g_trackHeight = 30;
        static ImColor const g_trackSeparatorColor( 0xFF808080 );
        static float const g_itemMarginY = 2;
        static float const g_itemHandleWidth = 4;
    }

    //-------------------------------------------------------------------------

    Track::~Track()
    {
        for ( auto pItem : m_items )
        {
            KRG::Delete( pItem );
        }

        m_items.clear();
    }

    void Track::DrawHeader()
    {
        ImGui::SameLine( 10 );
        ImGui::AlignTextToFramePadding();
        ImGui::Text( "Track" );
    }

    //-------------------------------------------------------------------------

    TimelineEditor::TimelineEditor( TRange<int32> const& inTimeRange )
        : m_timeRange( inTimeRange )
        , m_viewRange( inTimeRange )
    {
        KRG_ASSERT( inTimeRange.IsSetAndValid() );

        // HACK
        //-------------------------------------------------------------------------

        auto pTrack = m_tracks.emplace_back( KRG::New<Track>() );

        auto pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 0, 9 );

        pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 80, 95 );

        pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 15 );

        pTrack = m_tracks.emplace_back( KRG::New<Track>() );

        pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 35, 75 );

        pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 1, 15 );

        pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 103 );

        pTrack = m_tracks.emplace_back( KRG::New<Track>() );

        pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 1, 50 );

        pItem = pTrack->m_items.emplace_back( KRG::New<Item>() );
        pItem->m_timeRange = TRange<float>( 65, 95 );
    }

    TimelineEditor::~TimelineEditor()
    {
        for ( auto pTrack : m_tracks )
        {
            KRG::Delete( pTrack );
        }
        m_tracks.clear();
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

    void TimelineEditor::UpdateItemEditing()
    {
        KRG_ASSERT( m_itemEditMode != ItemEditMode::None );
        KRG_ASSERT( m_pEditedItem != nullptr && m_pTrackForEditedItem != nullptr );

        if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
        {
            // Calculate valid range for modifications
            //-------------------------------------------------------------------------

            auto const floatViewRange = GetViewRangeAsFloatRange();
            TRange<float> clampRange = GetViewRangeAsFloatRange();
            for ( auto const pOtherItem : m_pTrackForEditedItem->m_items )
            {
                if ( pOtherItem == m_pEditedItem )
                {
                    continue;
                }

                if ( pOtherItem->m_timeRange.m_max < m_editedItemOriginalRange.m_min && pOtherItem->m_timeRange.m_max > clampRange.m_min )
                {
                    clampRange.m_min = pOtherItem->m_timeRange.m_max;
                }

                if ( pOtherItem->m_timeRange.m_min > m_editedItemOriginalRange.m_max && pOtherItem->m_timeRange.m_min < clampRange.m_max )
                {
                    clampRange.m_max = pOtherItem->m_timeRange.m_min;
                }
            }

            // Prevent immediate events ending up on top of other events or outside the range
            if ( m_pEditedItem->IsImmediateItem() )
            {
                clampRange.m_max -= 1;
            }

            // Apply mouse delta to item
            //-------------------------------------------------------------------------

            float const pixelOffset = ImGui::GetMouseDragDelta().x;
            float const timeOffset = pixelOffset / m_pixelsPerFrame;

            if ( m_itemEditMode == ItemEditMode::Move )
            {
                // Moving left
                if ( timeOffset < 0 )
                {
                    m_pEditedItem->m_timeRange.m_min = Math::Max( clampRange.m_min, m_editedItemOriginalRange.m_min + timeOffset );
                    m_pEditedItem->m_timeRange.m_max = m_pEditedItem->m_timeRange.m_min + m_editedItemOriginalRange.GetLength();
                }
                else // Moving to the right
                {
                    m_pEditedItem->m_timeRange.m_max = Math::Min( clampRange.m_max, m_editedItemOriginalRange.m_max + timeOffset );
                    m_pEditedItem->m_timeRange.m_min = m_pEditedItem->m_timeRange.m_max - m_editedItemOriginalRange.GetLength();
                }
            }
            else if ( m_itemEditMode == ItemEditMode::ResizeLeft )
            {
                m_pEditedItem->m_timeRange.m_min = Math::Min( m_editedItemOriginalRange.m_max - 1, m_editedItemOriginalRange.m_min + timeOffset );
                m_pEditedItem->m_timeRange.m_min = Math::Max( clampRange.m_min, m_pEditedItem->m_timeRange.m_min );
            }
            else if ( m_itemEditMode == ItemEditMode::ResizeRight )
            {
                m_pEditedItem->m_timeRange.m_max = Math::Max( m_editedItemOriginalRange.m_min + 1, m_editedItemOriginalRange.m_max + timeOffset );
                m_pEditedItem->m_timeRange.m_max = Math::Min( clampRange.m_max, m_pEditedItem->m_timeRange.m_max );
            }
        }
        else if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            m_itemEditMode = ItemEditMode::None;
            m_pEditedItem = nullptr;
        }
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
        ImGuiX::ButtonColored( Colors::LimeGreen.ToFloat4(), KRG_ICON_PLUS "##AddTrack" );
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
            float const lineOffsetX = startPosX + int32( i * m_pixelsPerFrame );
            if ( lineOffsetX < startPosX || lineOffsetX > endPosX )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            bool const isLargeLine = ( ( i % numFramesForLargeInterval ) == 0 ) || ( i == m_viewRange.GetLength() || i == 0 );
            bool const isMediumLine = ( i % NumFramesForMediumInterval ) == 0;

            //-------------------------------------------------------------------------

            if ( isLargeLine )
            {
                bool const isRangeEndLine = ( i == m_timeRange.m_max );
                float lineOffsetY = g_timelineLargeLineOffset;
                ImColor lineColor = isRangeEndLine ? g_timelineRangeEndLineColor :  g_timelineLargeLineColor;

                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, endPosY ), lineColor, 1 );

                // Draw text label
                InlineString<256> label;
                label.sprintf( "%d", m_viewRange.m_min + i );
                pDrawList->AddText( ImVec2( lineOffsetX + g_timelineLabelLeftPadding, startPosY ), g_headerLabelColor, label.c_str() );
            }
            else if( isMediumLine )
            {
                float const lineOffsetY = g_timelineMediumLineOffset;
                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, endPosY ), g_timelineMediumLineColor, 1 );
            }
            else // Small lines
            {
                float const lineOffsetY = g_timelineSmallLineOffset;
                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, startPosY + g_headerHeight ), g_timelineLargeLineColor, 1 );
                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + g_headerHeight ), ImVec2( lineOffsetX, endPosY ), g_timelineSmallLineColor, 1 );
            }
        }
    }

    void TimelineEditor::DrawPlayhead( ImRect const& playheadRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        float playheadPosX = playheadRect.GetTL().x;
        float playheadPosY = playheadRect.GetTL().y + g_headerHeight;

        //-------------------------------------------------------------------------

        float const playheadStartOffsetX = ConvertFramesToPixels( m_playheadTime - m_viewRange.m_min );
        float const playheadHeight = g_headerHeight - ( g_playHeadVerticalPadding * 2 );
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
            if ( playheadRect.Contains( mousePos ) && mousePos.y < ( playheadRect.Min.y + g_headerHeight ) )
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

        pDrawList->AddLine( playheadPosition, ImVec2( playheadPosition.x, playheadRect.GetBR().y ), g_playheadMarkerLineColor );
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::DrawTrackItemArea( Track const* pTrack, ImRect const& eventAreaRect )
    {
        KRG_ASSERT( pTrack != nullptr );

        auto const floatViewRange = GetViewRangeAsFloatRange();

        //-------------------------------------------------------------------------

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        ImGui::PushClipRect( eventAreaRect.GetTL(), eventAreaRect.GetBR(), false );

        for ( auto const pItem : pTrack->m_items )
        {
            if ( !floatViewRange.Overlaps( pItem->m_timeRange ) )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            float itemEndTime = pItem->m_timeRange.m_max;
            if ( pItem->IsImmediateItem() )
            {
                itemEndTime += 1;
            }

            float const itemStartX = eventAreaRect.GetTL().x + ( pItem->m_timeRange.m_min - m_viewRange.m_min ) * m_pixelsPerFrame;
            float const itemEndX = eventAreaRect.GetTL().x + ( itemEndTime - m_viewRange.m_min ) * m_pixelsPerFrame;
            float const itemStartY = eventAreaRect.GetTL().y + g_itemMarginY;
            float const itemEndY = eventAreaRect.GetBR().y - g_itemMarginY;

            ImVec2 const itemStart( itemStartX, itemStartY );
            ImVec2 const itemEnd( itemEndX, itemEndY );

            // Handle mouse
            //-------------------------------------------------------------------------

            ImVec2 const mousePos = ImGui::GetMousePos();
            bool const isHovered = ImRect( itemStart, itemEnd ).Contains( mousePos );
            bool const isHoveredOverLeftHandle = ( !pItem->IsImmediateItem() && isHovered ) ? ImRect( itemStart, ImVec2( itemStart.x + g_itemHandleWidth, itemEnd.y ) ).Contains( mousePos ) : false;
            bool const isHoveredOverRightHandle = ( !pItem->IsImmediateItem() && isHovered && !isHoveredOverLeftHandle ) ? ImRect( ImVec2( itemEnd.x - g_itemHandleWidth, itemStart.y ), itemEnd ).Contains( mousePos ) : false;

            if ( isHoveredOverLeftHandle || isHoveredOverRightHandle )
            {
                ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeEW );
            }

            if ( isHovered && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
            {
                if ( isHoveredOverLeftHandle )
                {
                    m_itemEditMode = ItemEditMode::ResizeLeft;
                }
                else if ( isHoveredOverRightHandle )
                {
                    m_itemEditMode = ItemEditMode::ResizeRight;
                }
                else // Regular move
                {
                    m_itemEditMode = ItemEditMode::Move;
                }

                m_pTrackForEditedItem = pTrack;
                m_pEditedItem = pItem;
                m_editedItemOriginalRange = pItem->m_timeRange;
            }

            // Draw item
            //-------------------------------------------------------------------------

            ImVec4 itemColor = pItem->GetColor().ToFloat4();
            if ( pItem == m_pEditedItem )
            {
                itemColor = (Float4) itemColor * 1.5f;
            }
            else if ( isHovered )
            {
                itemColor = (Float4) itemColor * 1.25f;
            }

            pDrawList->AddRectFilled( itemStart, itemEnd, ImColor( itemColor ), pItem->IsImmediateItem() ? 0.0f : 4.0f );

            pDrawList->AddText( itemStart + ImVec2( 5, 1 ), 0xFF000000, pItem->GetLabel() );
            pDrawList->AddText( itemStart + ImVec2( 4, 0 ), ImColor( ImGuiX::Theme::s_textColor ), pItem->GetLabel() );
        }

        ImGui::PopClipRect();
    }

    void TimelineEditor::DrawTracks( ImRect const& trackAreaRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        //-------------------------------------------------------------------------

        // TODO: Draw vertical scrollbar

        //-------------------------------------------------------------------------

        float trackStartY = trackAreaRect.GetTL().y;
        int32 const numTracks = (int32) m_tracks.size();
        for ( int32 i = 0; i < numTracks; i++ )
        {
            auto pTrack = m_tracks[i];

            float const trackEndY = trackStartY + g_trackHeight;

            // Terminate loop as soon as a track is no longer visible
            if ( trackStartY > trackAreaRect.GetBR().y )
            {
                break;
            }

            // Draw track header
            //-------------------------------------------------------------------------

            ImGui::PushID( pTrack );

            int32 const headerFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;
            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 1 ) );
            ImGui::BeginChild( "Track Header", ImVec2( g_trackHeaderWidth, g_trackHeight ), false, headerFlags );
            {
                pTrack->DrawHeader();
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            // Draw items
            //-------------------------------------------------------------------------

            ImRect const itemAreaRect( ImVec2( trackAreaRect.GetTL().x + g_trackHeaderWidth, trackStartY ), ImVec2( trackAreaRect.GetBR().x, trackEndY ) );
            DrawTrackItemArea( pTrack, itemAreaRect );

            // Draw track separator
            //-------------------------------------------------------------------------

            pDrawList->AddLine( ImVec2( trackAreaRect.GetTL().x, trackEndY ), ImVec2( trackAreaRect.GetBR().x, trackEndY ), g_trackSeparatorColor );
            trackStartY = trackEndY + 1;

            ImGui::PopID();
        }
    }

    //-------------------------------------------------------------------------

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
        // Manage item edit state
        //-------------------------------------------------------------------------

        if ( m_itemEditMode != ItemEditMode::None )
        {
            UpdateItemEditing();
        }

        //-------------------------------------------------------------------------
        // Adjust view
        //-------------------------------------------------------------------------

        // Adjust visible range based on the canvas size
        int32 const maxVisibleFrames = Math::FloorToInt( ( canvasSize.x - g_trackHeaderWidth - g_playheadHalfWidth ) / m_pixelsPerFrame );
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
        pDrawList->AddRectFilled( canvasPos, ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + g_headerHeight ), g_headerBackgroundColor, 0 );

        ImRect const timelineControlsRect( canvasPos, ImVec2( canvasPos.x + g_trackHeaderWidth, canvasPos.y + g_headerHeight ) );

        ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4) g_headerBackgroundColor );
        DrawTimelineControls( timelineControlsRect );
        ImGui::PopStyleColor();

        ImRect const timelineRect( ImVec2( canvasPos.x + g_trackHeaderWidth, canvasPos.y ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ) );
        DrawTimeline( timelineRect );

        //-------------------------------------------------------------------------
        // Tracks
        //-------------------------------------------------------------------------

        ImRect const trackAreaRect( ImVec2( canvasPos.x, canvasPos.y + g_headerHeight ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ) );
        DrawTracks( trackAreaRect );

        //-------------------------------------------------------------------------
        // Playhead
        //-------------------------------------------------------------------------

        DrawPlayhead( timelineRect );

        //-------------------------------------------------------------------------
        // Horizontal Scrollbar
        //-------------------------------------------------------------------------

        ImRect const horizontalScrollBarRect( ImVec2( canvasPos.x + g_trackHeaderWidth, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y ) );
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