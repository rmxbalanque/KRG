#pragma once

#include "Tools/Core/_Module/API.h"
#include "TimelineData.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------
// Timeline Widget
//-------------------------------------------------------------------------
// Basic timeline/sequencer style widget

namespace KRG
{
    class KRG_TOOLS_CORE_API TimelineEditor
    {
    protected:

        enum class ViewUpdateMode : uint8
        {
            None,
            ShowFullTimeRange,
            GoToStart,
            GoToEnd,
            TrackPlayhead,
        };

        enum class PlayState : uint8
        {
            Playing,
            Paused
        };

        enum class ItemEditMode : uint8
        {
            None,
            Move,
            ResizeLeft,
            ResizeRight
        };

        struct MouseState
        {
            inline void Reset()
            {
                m_isHoveredOverTrackEditor = false;
                m_pHoveredTrack = nullptr;
                m_pHoveredItem = nullptr;
                m_hoveredItemMode = ItemEditMode::None;
                m_playheadTimeForMouse = -1.0f;
                m_snappedPlayheadTimeForMouse = -1.0f;
            }

            TimelineTrack*          m_pHoveredTrack = nullptr;
            TimelineItem*           m_pHoveredItem = nullptr;
            float                   m_playheadTimeForMouse = -1.0f;
            float                   m_snappedPlayheadTimeForMouse = -1.0f;
            ItemEditMode            m_hoveredItemMode = ItemEditMode::None;
            bool                    m_isHoveredOverTrackEditor = false;
        };

        struct ItemEditState
        {
            inline void Reset()
            {
                m_pTrackForEditedItem = nullptr;
                m_pEditedItem = nullptr;
                m_mode = ItemEditMode::None;
                m_originalTimeRange.Reset();
            }

            ItemEditMode            m_mode = ItemEditMode::None;
            TimelineTrack const*    m_pTrackForEditedItem = nullptr;
            TimelineItem*           m_pEditedItem = nullptr;
            FloatRange              m_originalTimeRange;
        };

        // The state for the given open context menu
        struct ContextMenuState
        {
            inline char const* GetContextMenuName() const
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

            inline void Reset()
            {
                m_pTrack = nullptr;
                m_pItem = nullptr;
                m_isOpen = false;
                m_playheadTimeForMouse = -1.0f;
            }

            TimelineTrack*          m_pTrack = nullptr;
            TimelineItem*           m_pItem = nullptr;
            float                   m_playheadTimeForMouse = -1.0f;
            bool                    m_isOpen = false;
        };

    public:

        TimelineEditor( IntRange const& inTimeRange );
        virtual ~TimelineEditor();

        inline bool IsPlaying() const { return m_playState == PlayState::Playing; }
        inline bool IsPaused() const { return m_playState == PlayState::Paused; }

        inline bool IsLoopingEnabled() const { return m_isLoopingEnabled; }
        inline bool IsFrameSnappingEnabled() const { return m_isFrameSnappingEnabled; }

        inline Percentage GetPlayheadPositionAsPercentage() const { return Percentage( m_playheadTime / m_timeRange.m_end ); }

        inline TVector<TimelineItem*> const& GetSelectedItems() const { return m_selectedItems; }
        void ClearSelection();

    protected:

        // This is protected, since most of the time the client will want to provide a custom update function 
        // that handles the side-effects of the draw e.g. play head position update, track changes, etc...
        void Draw();

        inline FloatRange GetTimeRangeAsFloatRange() const{ return FloatRange( float( m_timeRange.m_start ), float( m_timeRange.m_end ) ); }
        inline FloatRange GetViewRangeAsFloatRange() const{ return FloatRange( float( m_viewRange.m_start ), float( m_viewRange.m_end ) ); }

        inline void SetTimeRange( IntRange const& inRange ) { KRG_ASSERT( inRange.IsSetAndValid() ); m_timeRange = inRange; }
        inline void SetViewRange( IntRange const& inRange ) { KRG_ASSERT( inRange.IsSetAndValid() ); m_viewRange = inRange; }

        // Called whenever the play state is switched
        virtual void OnPlayStateChanged() {}

        // Set the playhead position
        inline void SetPlayheadPosition( float inPosition ) { m_playheadTime = GetTimeRangeAsFloatRange().GetClampedValue( inPosition ); }

        // Set the playhead position from a percentage over the time range
        inline void SetPlayheadPositionAsPercentage( Percentage inPercentage ) { m_playheadTime = inPercentage.GetClamped( m_isLoopingEnabled ).ToFloat() * m_timeRange.m_end; }

        // Has any modifications been made to the tracks/events?
        virtual bool IsDirty() const { return m_isDirty; }

        // Request that we save the state
        virtual void RequestSave() {}

    private:

        inline float ConvertPixelsToFrames( float inPixels ) const { return inPixels / m_pixelsPerFrame; }
        inline float ConvertFramesToPixels( float inTime ) const { return inTime * m_pixelsPerFrame; }
        inline void SetViewToStart() { m_viewUpdateMode = ViewUpdateMode::GoToStart; }
        inline void SetViewToEnd() { m_viewUpdateMode = ViewUpdateMode::GoToEnd; }

        //-------------------------------------------------------------------------

        // Is this a valid item ptr (i.e. is it present in any known track)
        bool IsValidPtr( TimelineItem const* pItem );

        // Is this a valid track ptr (i.e. is it in the list of tracks)
        bool IsValidPtr( TimelineTrack const* pTrack );

        // Delete specified track
        void DeleteTrack( TimelineTrack* pTrack );

        //-------------------------------------------------------------------------

        void SetPlayState( PlayState newPlayState );

        //-------------------------------------------------------------------------

        // Provided rect overs only the area within with the controls can be drawn
        void DrawTimelineControls( ImRect const& controlsRect );

        // Provided rect covers only the area for the timeline display, excludes track header region
        void DrawTimeline( ImRect const& timelineRect );

        // Provided rect covers only the area for the timeline display, excludes track header region
        void DrawPlayhead( ImRect const& playheadRect );

        // Provided rect defines the area available to draw multiple tracks (incl. headers and items)
        void DrawTracks( ImRect const& trackAreaRect );

        // Draw the add track menu - returns true if a track was added
        virtual bool DrawAddTracksMenu();

        // Draw the various context menus
        void DrawContextMenu();

        //-------------------------------------------------------------------------

        // Called each frame to process any mouse/keyboard input
        void HandleUserInput();

        // Called each frame to update the view range
        void UpdateViewRange();

        // Reset the view to the full time range
        void ResetViewRange() { m_viewUpdateMode = ViewUpdateMode::ShowFullTimeRange; }

        // Selection
        //-------------------------------------------------------------------------

        inline bool IsSelected( TimelineItem const* pItem ) const { return VectorContains( m_selectedItems, const_cast<TimelineItem*>( pItem ) ); }
        inline bool IsSelected( TimelineTrack const* pTrack ) const { return VectorContains( m_selectedTracks, const_cast<TimelineTrack*>( pTrack ) ); }

        // Set the selection to a single item
        void SetSelection( TimelineItem* pItem );
        void SetSelection( TimelineTrack* pTrack );

        // Add item to the current selection
        void AddToSelection( TimelineItem* pItem );
        void AddToSelection( TimelineTrack* pTrack );

        // Remove an item from the current selection
        void RemoveFromSelection( TimelineItem* pItem );
        void RemoveFromSelection( TimelineTrack* pTrack );

        void EnsureValidSelection();

    protected:

        TimelineData                m_data;

        // The total editable time range
        IntRange                    m_timeRange = IntRange( 0, 0 );

        // The current visible time range
        IntRange                    m_viewRange = IntRange( 0, 0 );

        float                       m_pixelsPerFrame = 10.0f;
        float                       m_playheadTime = 0.0f;

        PlayState                   m_playState = PlayState::Paused;
        ViewUpdateMode              m_viewUpdateMode = ViewUpdateMode::ShowFullTimeRange;
        bool                        m_isLoopingEnabled = false;
        bool                        m_isFrameSnappingEnabled = true;
        bool                        m_isDraggingPlayhead = false;

        MouseState                  m_mouseState;
        ItemEditState               m_itemEditState;
        ContextMenuState            m_contextMenuState;

        TVector<TimelineItem*>      m_selectedItems;
        TVector<TimelineTrack*>     m_selectedTracks;

        bool                        m_isDirty = false;
    };
}