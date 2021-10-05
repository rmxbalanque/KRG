#include "TimelineEditor.h"
#include "System/Imgui/ImguiX.h" 

//-------------------------------------------------------------------------

namespace KRG::Timeline
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
    static ImColor const g_selectedTrackColor = ImGuiX::ConvertColor( Color( 0xFFB6C1FF ).GetAlphaVersion( 0.2f ) );
    static float const g_itemMarginY = 2;
    static float const g_itemHandleWidth = 4;
    static float const g_immediateItemWidth = 6;

    //-------------------------------------------------------------------------

    void TimelineEditor::MouseState::Reset()
    {
        m_isHoveredOverTrackEditor = false;
        m_pHoveredTrack = nullptr;
        m_pHoveredItem = nullptr;
        m_hoveredItemMode = ItemEditMode::None;
        m_playheadTimeForMouse = -1.0f;
        m_snappedPlayheadTimeForMouse = -1.0f;
    }

    inline void TimelineEditor::ItemEditState::Reset()
    {
        m_pTrackForEditedItem = nullptr;
        m_pEditedItem = nullptr;
        m_mode = ItemEditMode::None;
        m_originalTimeRange.Reset();
    }

    char const* TimelineEditor::ContextMenuState::GetContextMenuName() const
    {
        if ( m_pItem != nullptr )
        {
            return "ItemContextMenu";
        }

        if ( m_pTrack != nullptr )
        {
            return "TrackContextMenu";
        }

        return "EditorContextMenu";
    }

    void TimelineEditor::ContextMenuState::Reset()
    {
        m_pTrack = nullptr;
        m_pItem = nullptr;
        m_isOpen = false;
        m_playheadTimeForMouse = -1.0f;
    }

    //-------------------------------------------------------------------------

    TimelineEditor::TimelineEditor( IntRange const& inTimeRange )
        : m_timeRange( inTimeRange )
        , m_viewRange( inTimeRange )
    {
        KRG_ASSERT( inTimeRange.IsSetAndValid() );
    }

    TimelineEditor::~TimelineEditor()
    {
        m_trackContainer.Reset();
    }

    //-------------------------------------------------------------------------

    bool TimelineEditor::IsValidPtr( TrackItem const* pItem )
    {
        for ( auto pTrack : m_trackContainer )
        {
            if ( pTrack->Contains( pItem ) )
            {
                return true;
            }
        }

        return false;
    }

    bool TimelineEditor::IsValidPtr( Track const* pTrack )
    {
        return eastl::find( m_trackContainer.begin(), m_trackContainer.end(), pTrack ) != m_trackContainer.end();
    }

    void TimelineEditor::DeleteItem( TrackItem* pItem )
    {
        KRG_ASSERT( IsValidPtr( pItem ) );

        for ( auto pTrack : m_trackContainer )
        {
            if ( pTrack->DeleteItem( pItem ) )
            {
                m_isDirty = true;
                break;
            }
        }

        m_itemEditState.Reset();
    }

    void TimelineEditor::DeleteTrack( Track* pTrack )
    {
        KRG_ASSERT( IsValidPtr( pTrack ) );
        m_trackContainer.m_tracks.erase_first( pTrack );
        KRG::Delete( pTrack );
        m_isDirty = true;
    }

    //-------------------------------------------------------------------------

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

    void TimelineEditor::HandleUserInput()
    {
        if ( m_mouseState.m_pHoveredItem != nullptr && m_mouseState.m_hoveredItemMode != ItemEditMode::Move )
        {
            ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeEW );
        }

        // Context Menu
        //-------------------------------------------------------------------------

        bool const isMouseRightButtonReleased = ImGui::IsMouseReleased( ImGuiMouseButton_Right );
        if ( isMouseRightButtonReleased )
        {
            if ( m_mouseState.m_isHoveredOverTrackEditor )
            {
                m_contextMenuState.m_pItem = m_mouseState.m_pHoveredItem;
                m_contextMenuState.m_pTrack = m_mouseState.m_pHoveredTrack;
                m_contextMenuState.m_playheadTimeForMouse = m_mouseState.m_playheadTimeForMouse;
                ImGui::OpenPopupEx( GImGui->CurrentWindow->GetID( m_contextMenuState.GetContextMenuName() ) );
            }
        }

        DrawContextMenu();

        // Handle Selection
        //-------------------------------------------------------------------------

        bool const isMouseClicked = ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseClicked( ImGuiMouseButton_Right );
        if ( isMouseClicked )
        {
            if ( m_mouseState.m_pHoveredItem != nullptr )
            {
                m_itemEditState.m_pEditedItem = m_mouseState.m_pHoveredItem;
                m_itemEditState.m_pTrackForEditedItem = m_mouseState.m_pHoveredTrack;
                m_itemEditState.m_originalTimeRange = m_mouseState.m_pHoveredItem->GetTimeRange();
                m_itemEditState.m_mode = m_mouseState.m_hoveredItemMode;

                SetSelection( m_mouseState.m_pHoveredItem );
            }
            else if( m_mouseState.m_pHoveredTrack != nullptr )
            {
                SetSelection( m_mouseState.m_pHoveredTrack );
            }
        }

        // Keyboard
        //-------------------------------------------------------------------------

        if ( ImGui::IsKeyReleased( ImGui::GetKeyIndex( ImGuiKey_Space ) ) )
        {
            SetPlayState( m_playState == PlayState::Playing ? PlayState::Paused : PlayState::Playing );
        }
        else if ( ImGui::IsKeyReleased( ImGui::GetKeyIndex( ImGuiKey_Enter ) ) )
        {
            for ( auto pTrack : m_selectedTracks )
            {
                pTrack->CreateItem( m_playheadTime );
                m_isDirty = true;
            }
        }
        else  if ( ImGui::IsKeyReleased( ImGui::GetKeyIndex( ImGuiKey_Delete ) ) )
        {
            TVector<TrackItem*> copiedSelectedItems = m_selectedItems;
            ClearSelection();

            for ( auto pItem : copiedSelectedItems )
            {
                DeleteItem( pItem );
            }
        }

        // Item Edition
        //-------------------------------------------------------------------------

        if ( m_itemEditState.m_mode != ItemEditMode::None )
        {
            KRG_ASSERT( m_itemEditState.m_pEditedItem != nullptr && m_itemEditState.m_pTrackForEditedItem != nullptr );

            if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
            {
                auto pEditedItem = m_itemEditState.m_pEditedItem;

                // Calculate valid range for modifications
                //-------------------------------------------------------------------------

                auto const floatViewRange = GetViewRangeAsFloatRange();
                FloatRange validEventRange = GetViewRangeAsFloatRange();
                for ( auto const pOtherItem : m_itemEditState.m_pTrackForEditedItem->m_items )
                {
                    FloatRange const otherItemTimeRange = pOtherItem->GetTimeRange();

                    if ( pOtherItem == pEditedItem )
                    {
                        continue;
                    }

                    if ( otherItemTimeRange.m_end < m_itemEditState.m_originalTimeRange.m_start && otherItemTimeRange.m_end > validEventRange.m_start )
                    {
                        validEventRange.m_start = otherItemTimeRange.m_end;
                    }

                    if ( otherItemTimeRange.m_start > m_itemEditState.m_originalTimeRange.m_end && otherItemTimeRange.m_start < validEventRange.m_end )
                    {
                        validEventRange.m_end = otherItemTimeRange.m_start;
                    }
                }

                // Prevent immediate items ending up on top of other items or outside the range
                if ( pEditedItem->IsImmediateItem() )
                {
                    validEventRange.m_end -= 1;
                }

                // Apply mouse delta to item
                //-------------------------------------------------------------------------

                if ( validEventRange.IsSetAndValid() )
                {
                    float const pixelOffset = ImGui::GetMouseDragDelta().x;
                    float const timeOffset = pixelOffset / m_pixelsPerFrame;

                    FloatRange editedItemTimeRange = pEditedItem->GetTimeRange();

                    if ( m_itemEditState.m_mode == ItemEditMode::Move )
                    {
                        // Create a new range to clamp the event start time to
                        FloatRange validEventStartRange = validEventRange;
                        if ( pEditedItem->IsDurationItem() )
                        {
                            validEventStartRange.m_end = validEventStartRange.m_end - m_itemEditState.m_originalTimeRange.GetLength();
                        }

                        float newTime = m_itemEditState.m_originalTimeRange.m_start + timeOffset;
                        if ( m_isFrameSnappingEnabled )
                        {
                            newTime = Math::Round( newTime );
                        }

                        editedItemTimeRange.m_start = validEventStartRange.GetClampedValue( newTime );
                        editedItemTimeRange.m_end = editedItemTimeRange.m_start + m_itemEditState.m_originalTimeRange.GetLength();
                    }
                    else if ( m_itemEditState.m_mode == ItemEditMode::ResizeLeft )
                    {
                        float newTime = m_itemEditState.m_originalTimeRange.m_start + timeOffset;
                        if ( m_isFrameSnappingEnabled )
                        {
                            newTime = Math::Round( newTime );
                        }

                        editedItemTimeRange.m_start = Math::Min( m_itemEditState.m_originalTimeRange.m_end - 1, newTime );
                        editedItemTimeRange.m_start = Math::Max( validEventRange.m_start, editedItemTimeRange.m_start );
                    }
                    else if ( m_itemEditState.m_mode == ItemEditMode::ResizeRight )
                    {
                        float newTime = m_itemEditState.m_originalTimeRange.m_end + timeOffset;
                        if ( m_isFrameSnappingEnabled )
                        {
                            newTime = Math::Round( newTime );
                        }

                        editedItemTimeRange.m_end = Math::Max( m_itemEditState.m_originalTimeRange.m_start + 1, newTime );
                        editedItemTimeRange.m_end = Math::Min( validEventRange.m_end, editedItemTimeRange.m_end );
                    }

                    pEditedItem->SetTimeRange( editedItemTimeRange );
                    m_isDirty = true;
                }
            }
            else if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
            {
                m_itemEditState.Reset();
            }
        }

        // Zoom
        //-------------------------------------------------------------------------

        auto const& IO = ImGui::GetIO();
        float const mouseWheelDelta = IO.MouseWheel;
        if ( IO.KeyMods && ImGuiKeyModFlags_Ctrl && mouseWheelDelta != 0 )
        {
            m_pixelsPerFrame = Math::Max( 1.0f, m_pixelsPerFrame + mouseWheelDelta );
        }

        //-------------------------------------------------------------------------

        m_mouseState.Reset();
    }

    void TimelineEditor::UpdateViewRange()
    {
        ImVec2 const canvasSize = ImGui::GetContentRegionAvail();
        float const trackAreaWidth = ( canvasSize.x - g_trackHeaderWidth - g_playheadHalfWidth );
        int32 const maxVisibleFrames = Math::Max( 0, Math::FloorToInt( ( canvasSize.x - g_trackHeaderWidth - g_playheadHalfWidth ) / m_pixelsPerFrame ) );

        // Adjust visible range based on the canvas size
        if ( m_viewRange.GetLength() != maxVisibleFrames )
        {
            m_viewRange.m_end = m_viewRange.m_start + maxVisibleFrames;
        }

        // Process any update requests
        //-------------------------------------------------------------------------

        switch ( m_viewUpdateMode )
        {
            case ViewUpdateMode::ShowFullTimeRange:
            {
                int32 const timeRangeLength = m_timeRange.GetLength();
                m_pixelsPerFrame = Math::Max( 1.0f, Math::Floor( trackAreaWidth / timeRangeLength ) );
                m_viewRange = m_timeRange;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::GoToStart:
            {
                m_viewRange.m_start = m_timeRange.m_start;
                m_viewRange.m_end = maxVisibleFrames;
                m_playheadTime = (float) m_timeRange.m_start;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::GoToEnd:
            {
                m_viewRange.m_start = Math::Max( m_timeRange.m_start, m_timeRange.m_end - maxVisibleFrames );
                m_viewRange.m_end = m_viewRange.m_start + maxVisibleFrames;
                m_playheadTime = (float) m_timeRange.m_end;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::TrackPlayhead:
            {
                if ( !m_viewRange.ContainsInclusive( (int32) m_playheadTime ) )
                {
                    // If the playhead is in the last visible range
                    if ( m_playheadTime + maxVisibleFrames >= m_timeRange.m_end )
                    {
                        m_viewRange.m_start = m_timeRange.m_end - maxVisibleFrames;
                        m_viewRange.m_end = m_timeRange.m_end;
                    }
                    else
                    {
                        m_viewRange.m_start = (int) m_playheadTime;
                        m_viewRange.m_end = m_viewRange.m_start + maxVisibleFrames;
                    }
                }
            }
            break;
        }
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::SetPlayheadPosition( float inPosition )
    {
        m_playheadTime = GetTimeRangeAsFloatRange().GetClampedValue( inPosition );

        if ( m_isFrameSnappingEnabled )
        {
            m_playheadTime = Math::Round( m_playheadTime );
        }
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::ClearSelection()
    {
        m_selectedItems.clear();
    }

    void TimelineEditor::SetSelection( TrackItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );
        m_selectedTracks.clear();
        m_selectedItems.clear();
        m_selectedItems.emplace_back( pItem );
    }

    void TimelineEditor::SetSelection( Track* pTrack )
    {
        KRG_ASSERT( pTrack != nullptr );
        m_selectedItems.clear();
        m_selectedTracks.clear();
        m_selectedTracks.emplace_back( pTrack );
    }

    void TimelineEditor::AddToSelection( TrackItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );
        m_selectedTracks.clear();
        if ( !VectorContains( m_selectedItems, pItem ) )
        {
            m_selectedItems.emplace_back( pItem );
        }
    }

    void TimelineEditor::AddToSelection( Track* pTrack )
    {
        KRG_ASSERT( pTrack != nullptr );
        m_selectedItems.clear();
        if ( !VectorContains( m_selectedTracks, pTrack ) )
        {
            m_selectedTracks.emplace_back( pTrack );
        }
    }

    void TimelineEditor::RemoveFromSelection( TrackItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );
        m_selectedItems.erase_first( pItem );
    }

    void TimelineEditor::RemoveFromSelection( Track* pTrack )
    {
        KRG_ASSERT( pTrack != nullptr );
        m_selectedTracks.erase_first( pTrack );
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::EnsureValidSelection()
    {
        for ( int32 i = (int32) m_selectedItems.size() - 1; i >= 0; i-- )
        {
            if ( !IsValidPtr( m_selectedItems[i] ) )
            {
                m_selectedItems.erase_unsorted( m_selectedItems.begin() + i );
            }
        }

        //-------------------------------------------------------------------------

        for ( int32 i = (int32) m_selectedTracks.size() - 1; i >= 0; i-- )
        {
            if ( !IsValidPtr( m_selectedTracks[i] ) )
            {
                m_selectedTracks.erase_unsorted( m_selectedTracks.begin() + i );
            }
        }
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::DrawTimelineControls( ImRect const& controlsRect )
    {
        auto const& style = ImGui::GetStyle();

        ImVec2 const controlsChildSize = controlsRect.GetSize() - ImVec2( 2 * style.ChildBorderSize, 2 * style.ChildBorderSize );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, (int) g_headerBackgroundColor );
        ImGui::BeginChild( "TimelineControls", controlsChildSize, false );

        ImVec2 const buttonSize = ImVec2( 20, controlsChildSize.y - style.ChildBorderSize );
        constexpr static float const buttonSeperation = 2;

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );

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

        if ( ImGui::Button( KRG_ICON_EYE "##ResetView", buttonSize ) )
        {
            ResetViewRange();
        }

        ImGuiX::ItemTooltip( "Reset View" );

        //-------------------------------------------------------------------------
        // Spacer
        //-------------------------------------------------------------------------

        float const buttonOffset = buttonSeperation + buttonSize.x;
        float const spacerWidth = controlsChildSize.x - ( buttonOffset * 8 );
        ImGui::SameLine( 0, spacerWidth );

        // Add tracks button
        //-------------------------------------------------------------------------

        if ( ImGuiX::ButtonColored( Colors::LimeGreen.ToFloat4(), KRG_ICON_PLUS "##AddTrack", buttonSize ) )
        {
            ImGui::OpenPopup( "AddTracksPopup" );
        }
        ImGuiX::ItemTooltip( "Add Track" );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        if ( ImGui::BeginPopup( "AddTracksPopup" ) )
        {
            if ( DrawAddTracksMenu() )
            {
                m_isDirty = true;
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        ImGui::EndChild();
        ImGui::PopStyleColor();
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

            bool const isRangeEndLine = ( ( m_viewRange.m_start + i ) == m_timeRange.m_end );
            bool const isLargeLine = ( ( i % numFramesForLargeInterval ) == 0 ) || ( i == m_viewRange.GetLength() || i == 0 ) || isRangeEndLine;
            bool const isMediumLine = ( i % NumFramesForMediumInterval ) == 0;

            //-------------------------------------------------------------------------

            if ( isLargeLine )
            {
                float lineOffsetY = g_timelineLargeLineOffset;
                ImColor lineColor = isRangeEndLine ? g_timelineRangeEndLineColor :  g_timelineLargeLineColor;

                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, endPosY ), lineColor, 1 );

                // Draw text label
                InlineString<256> label;
                label.sprintf( "%d", m_viewRange.m_start + i );
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

        float const playheadStartOffsetX = ConvertFramesToPixels( m_playheadTime - m_viewRange.m_start );
        float const playheadHeight = g_headerHeight - ( g_playHeadVerticalPadding * 2 );
        ImVec2 const playheadPosition( playheadPosX + playheadStartOffsetX, playheadPosY - g_playHeadVerticalPadding );

        ImGui::SetCursorPos( playheadPosition - ImVec2{ g_playheadHalfWidth, playheadHeight } - ImGui::GetWindowPos() );
        ImGui::SetItemAllowOverlap();
        ImGui::InvisibleButton( "##Playhead", ImVec2( g_playheadHalfWidth * 2.0f, playheadHeight ) );

        // Update playhead position
        //-------------------------------------------------------------------------

        ImVec2 const mousePos = ImGui::GetMousePos();

        // If the mouse is clicked over the header, start dragging operation
        if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
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
            FloatRange const playheadValidRange( (float) Math::Max( m_viewRange.m_start, m_timeRange.m_start ), (float) Math::Min( m_viewRange.m_end, m_timeRange.m_end ) );
            float newPlayheadTime = m_viewRange.m_start + ConvertPixelsToFrames( mousePos.x - playheadRect.Min.x );
            newPlayheadTime = playheadValidRange.GetClampedValue( newPlayheadTime );
            SetPlayheadPosition( newPlayheadTime );
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

    void TimelineEditor::DrawTracks( ImRect const& fullTrackAreaRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        ImVec2 const mousePos = ImGui::GetMousePos();
        auto const floatViewRange = GetViewRangeAsFloatRange();

        //-------------------------------------------------------------------------

        // TODO: Draw vertical scrollbar

        //-------------------------------------------------------------------------

        float trackStartY = fullTrackAreaRect.GetTL().y;
        int32 const numTracks = m_trackContainer.GetNumTracks();
        for ( int32 i = 0; i < numTracks; i++ )
        {
            auto pTrack = m_trackContainer[i];

            float const trackEndY = trackStartY + g_trackHeight;

            // Terminate loop as soon as a track is no longer visible
            if ( trackStartY > fullTrackAreaRect.GetBR().y )
            {
                break;
            }

            //-------------------------------------------------------------------------

            ImRect const trackRect( ImVec2( fullTrackAreaRect.GetTL().x, trackStartY ), ImVec2( fullTrackAreaRect.GetBR().x, trackEndY ) );
            ImRect const trackHeaderRect( ImVec2( fullTrackAreaRect.GetTL().x, trackStartY ), ImVec2( fullTrackAreaRect.GetTL().x + g_trackHeaderWidth, trackEndY ) );
            ImRect const trackAreaRect( ImVec2( fullTrackAreaRect.GetTL().x + g_trackHeaderWidth, trackStartY ), ImVec2( fullTrackAreaRect.GetBR().x, trackEndY ) );

            // Are we hovered over the track editor
            m_mouseState.m_isHoveredOverTrackEditor = fullTrackAreaRect.Contains( mousePos );

            // Are we hovered over this track?
            if ( trackRect.Contains( mousePos ) )
            {
                m_mouseState.m_pHoveredTrack = pTrack;
            }

            // Calculate playhead position for the mouse pos
            if ( fullTrackAreaRect.Contains( mousePos ) )
            {
                FloatRange const playheadValidRange( (float) Math::Max( m_viewRange.m_start, m_timeRange.m_start ), (float) Math::Min( m_viewRange.m_end, m_timeRange.m_end ) );
                m_mouseState.m_playheadTimeForMouse = m_viewRange.m_start + ConvertPixelsToFrames( mousePos.x - trackAreaRect.Min.x );
                m_mouseState.m_playheadTimeForMouse = playheadValidRange.GetClampedValue( m_mouseState.m_playheadTimeForMouse );
                m_mouseState.m_snappedPlayheadTimeForMouse = m_mouseState.m_playheadTimeForMouse;

                if ( m_isFrameSnappingEnabled )
                {
                    m_mouseState.m_snappedPlayheadTimeForMouse = Math::Round( m_mouseState.m_snappedPlayheadTimeForMouse );
                }
            }

            //-------------------------------------------------------------------------

            ImGui::PushID( pTrack );
            {
                // Draw track header
                //-------------------------------------------------------------------------

                int32 const headerFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;
                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 1 ) );
                if( ImGui::BeginChild( "Track Header", ImVec2( g_trackHeaderWidth, g_trackHeight ), false, headerFlags ) )
                {
                    pTrack->DrawHeader( trackHeaderRect );
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();

                // Track Item Area
                //-------------------------------------------------------------------------

                // Draw track highlight
                if ( IsSelected( pTrack ) )
                {
                    pDrawList->AddRectFilled( trackAreaRect.GetTL(), trackAreaRect.GetBR(), g_selectedTrackColor );
                }

                // Draw items
                //-------------------------------------------------------------------------

                ImGui::PushClipRect( trackAreaRect.GetTL(), trackAreaRect.GetBR(), false );

                for ( auto const pItem : pTrack->m_items )
                {
                    FloatRange const itemTimeRange = pItem->GetTimeRange();
                    if ( !floatViewRange.Overlaps( itemTimeRange ) )
                    {
                        continue;
                    }

                    //-------------------------------------------------------------------------

                    float itemEndTime = itemTimeRange.m_end;
                    if ( pItem->IsImmediateItem() )
                    {
                        itemEndTime += ( g_immediateItemWidth / m_pixelsPerFrame );
                    }

                    float const itemStartX = trackAreaRect.GetTL().x + ( itemTimeRange.m_start - m_viewRange.m_start ) * m_pixelsPerFrame;
                    float const itemEndX = trackAreaRect.GetTL().x + ( itemEndTime - m_viewRange.m_start ) * m_pixelsPerFrame;
                    float const itemStartY = trackAreaRect.GetTL().y + g_itemMarginY;
                    float const itemEndY = trackAreaRect.GetBR().y - g_itemMarginY;

                    ImVec2 const itemStart( itemStartX, itemStartY );
                    ImVec2 const itemEnd( itemEndX, itemEndY );

                    // Handle mouse
                    //-------------------------------------------------------------------------

                    bool const isItemHovered = ImRect( itemStart, itemEnd ).Contains( mousePos );
                    bool const isHoveredOverLeftHandle = ( !pItem->IsImmediateItem() && isItemHovered ) ? ImRect( itemStart, ImVec2( itemStart.x + g_itemHandleWidth, itemEnd.y ) ).Contains( mousePos ) : false;
                    bool const isHoveredOverRightHandle = ( !pItem->IsImmediateItem() && isItemHovered && !isHoveredOverLeftHandle ) ? ImRect( ImVec2( itemEnd.x - g_itemHandleWidth, itemStart.y ), itemEnd ).Contains( mousePos ) : false;

                    if ( isItemHovered )
                    {
                        m_mouseState.m_pHoveredItem = pItem;

                        if ( isHoveredOverLeftHandle )
                        {
                            m_mouseState.m_hoveredItemMode = ItemEditMode::ResizeLeft;
                        }
                        else if ( isHoveredOverRightHandle )
                        {
                            m_mouseState.m_hoveredItemMode = ItemEditMode::ResizeRight;
                        }
                        else
                        {
                            m_mouseState.m_hoveredItemMode = ItemEditMode::Move;
                        }
                    }

                    // Draw item
                    //-------------------------------------------------------------------------

                    ImVec4 itemColor = pItem->GetColor().ToFloat4();

                    // Set selection color first
                    if ( IsSelected( pItem ) )
                    {
                        itemColor = (Float4) itemColor * 1.45f;
                    }

                    // Apply modifiers based on current state
                    if ( pItem == m_itemEditState.m_pEditedItem )
                    {
                        itemColor = (Float4) itemColor * 1.1f;
                    }
                    else if ( isItemHovered )
                    {
                        itemColor = (Float4) itemColor * 1.15f;
                    }

                    // Draw actual item
                    pDrawList->AddRectFilled( itemStart, itemEnd, ImColor( itemColor ), pItem->IsImmediateItem() ? 0.0f : 4.0f );

                    InlineString<100> const itemLabel = pItem->GetLabel();
                    pDrawList->AddText( itemStart + ImVec2( 5, 1 ), 0xFF000000, itemLabel.c_str() );
                    pDrawList->AddText( itemStart + ImVec2( 4, 0 ), ImColor( ImGuiX::Theme::s_textColor ), itemLabel.c_str() );
                }

                ImGui::PopClipRect();

                // Draw track separator
                //-------------------------------------------------------------------------

                pDrawList->AddLine( ImVec2( fullTrackAreaRect.GetTL().x, trackEndY ), ImVec2( fullTrackAreaRect.GetBR().x, trackEndY ), g_trackSeparatorColor );
                trackStartY = trackEndY + 1;
            }
            ImGui::PopID();
        }
    }

    bool TimelineEditor::DrawAddTracksMenu()
    {
        ImGui::Text( "Please override this function" );
        return false;
    }

    void TimelineEditor::DrawContextMenu()
    {
        m_contextMenuState.m_isOpen = false;

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );

        // Items
        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupContextItem( "ItemContextMenu" ) )
        {
            if ( IsValidPtr( m_contextMenuState.m_pItem ) )
            {
                bool const shouldDeleteItem = ImGui::MenuItem( "Delete Item" );

                //-------------------------------------------------------------------------

                if ( m_contextMenuState.m_pItem->HasContextMenu() )
                {
                    ImGui::Separator();
                    m_contextMenuState.m_pItem->DrawContextMenu();
                }

                //-------------------------------------------------------------------------

                if ( shouldDeleteItem )
                {
                    ClearSelection();
                    DeleteItem( m_contextMenuState.m_pItem );
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
            m_contextMenuState.m_isOpen = true;
        }

        // Tracks
        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupContextItem( "TrackContextMenu" ) )
        {
            if ( IsValidPtr( m_contextMenuState.m_pTrack ) )
            {
                if ( ImGui::MenuItem( "Add Item" ) )
                {
                    m_contextMenuState.m_pTrack->CreateItem( m_contextMenuState.m_playheadTimeForMouse < 0.0f ? m_playheadTime : m_contextMenuState.m_playheadTimeForMouse );
                    m_isDirty = true;
                }

                bool const shouldDeleteTrack = ImGui::MenuItem( "Delete Track" );

                //-------------------------------------------------------------------------

                if ( m_contextMenuState.m_pTrack->HasContextMenu() )
                {
                    ImGui::Separator();
                    m_contextMenuState.m_pTrack->DrawContextMenu( m_trackContainer.m_tracks, m_contextMenuState.m_playheadTimeForMouse < 0.0f ? m_playheadTime : m_contextMenuState.m_playheadTimeForMouse );
                }

                //-------------------------------------------------------------------------

                if ( shouldDeleteTrack )
                {
                    ClearSelection();
                    DeleteTrack( m_contextMenuState.m_pTrack );
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
            m_contextMenuState.m_isOpen = true;
        }

        // General
        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupContextItem( "EditorContextMenu" ) )
        {
            if ( ImGui::BeginMenu( "Add Track" ) )
            {
                if ( DrawAddTracksMenu() )
                {
                    m_isDirty = true;
                }
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
            m_contextMenuState.m_isOpen = true;
        }

        //-------------------------------------------------------------------------

        ImGui::PopStyleVar();
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::Draw()
    {
        ImVec2 const canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 const canvasSize = ImGui::GetContentRegionAvail();

        bool const requiresHorizontalScrollBar = ( m_viewRange.m_start > m_timeRange.m_start || m_viewRange.m_end < m_timeRange.m_end );
        float const horizontalScrollBarHeight = requiresHorizontalScrollBar ? g_horizontalScrollbarHeight : 0.0f;

        //-------------------------------------------------------------------------
        // Manage play state and input
        //-------------------------------------------------------------------------

        if ( IsPlaying() )
        {
            if ( !m_isLoopingEnabled && m_playheadTime >= m_timeRange.m_end )
            {
                SetPlayState( PlayState::Paused );
            }
        }

        // Remove any invalid ptrs from the current selection
        EnsureValidSelection();

        // Handle the user input based on the current keyboard state and the mouse state from the last frame
        HandleUserInput();

        // Update the view range, to ensure that we track the playhead, etc...
        UpdateViewRange();

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleColor( ImGuiCol_FrameBg, 0 );
        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 2 ) );
        ImGui::BeginChildFrame( 99, canvasSize );

        //-------------------------------------------------------------------------
        // Header
        //-------------------------------------------------------------------------

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        pDrawList->AddRectFilled( canvasPos, ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + g_headerHeight ), g_headerBackgroundColor, 0 );

        ImRect const timelineControlsRect( canvasPos, ImVec2( canvasPos.x + g_trackHeaderWidth, canvasPos.y + g_headerHeight - 1 ) );

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
        float scrollbarPosition = m_viewRange.m_start * m_pixelsPerFrame;

        ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
        ImGuiID const horizontalScrollBarID = pWindow->GetID( "#TimelineScrollbarY" );
        if( ImGui::ScrollbarEx( horizontalScrollBarRect, horizontalScrollBarID, ImGuiAxis_X, &scrollbarPosition, currentViewSize, totalContentSizeNeeded, 0 ) )
        {
            int32 const viewRangeOriginalLength = m_viewRange.GetLength();
            m_viewRange.m_start = Math::FloorToInt( scrollbarPosition / m_pixelsPerFrame );
            m_viewRange.m_end = m_viewRange.m_start + viewRangeOriginalLength;
        }

        //-------------------------------------------------------------------------

        ImGui::EndChildFrame();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopID();
    }
}