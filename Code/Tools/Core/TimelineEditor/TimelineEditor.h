#pragma once

#include "Tools/Core/_Module/API.h"
#include "TimelineData.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"
#include "System/Imgui/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------
// Timeline Widget
//-------------------------------------------------------------------------
// Basic timeline/sequencer style widget

namespace KRG::Timeline
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
            void Reset();

            Track*                  m_pHoveredTrack = nullptr;
            TrackItem*              m_pHoveredItem = nullptr;
            float                   m_playheadTimeForMouse = -1.0f;
            float                   m_snappedPlayheadTimeForMouse = -1.0f;
            ItemEditMode            m_hoveredItemMode = ItemEditMode::None;
            bool                    m_isHoveredOverTrackEditor = false;
        };

        struct ItemEditState
        {
            void Reset();

            ItemEditMode            m_mode = ItemEditMode::None;
            Track const*            m_pTrackForEditedItem = nullptr;
            TrackItem*              m_pEditedItem = nullptr;
            FloatRange              m_originalTimeRange;
        };

        // The state for the given open context menu
        struct ContextMenuState
        {
            char const* GetContextMenuName() const;
            void Reset();

            Track*                  m_pTrack = nullptr;
            TrackItem*              m_pItem = nullptr;
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

        inline TVector<TrackItem*> const& GetSelectedItems() const { return m_selectedItems; }
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
        void SetPlayheadPosition( float inPosition );

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
        bool IsValidPtr( TrackItem const* pItem );

        // Is this a valid track ptr (i.e. is it in the list of tracks)
        bool IsValidPtr( Track const* pTrack );

        // Delete specified item
        void DeleteItem( TrackItem* pItem );

        // Delete specified track
        void DeleteTrack( Track* pTrack );

        //-------------------------------------------------------------------------

        // Set the current state of the timeline editor
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

        inline bool IsSelected( TrackItem const* pItem ) const { return VectorContains( m_selectedItems, const_cast<TrackItem*>( pItem ) ); }
        inline bool IsSelected( Track const* pTrack ) const { return VectorContains( m_selectedTracks, const_cast<Track*>( pTrack ) ); }

        // Set the selection to a single item
        void SetSelection( TrackItem* pItem );
        void SetSelection( Track* pTrack );

        // Add item to the current selection
        void AddToSelection( TrackItem* pItem );
        void AddToSelection( Track* pTrack );

        // Remove an item from the current selection
        void RemoveFromSelection( TrackItem* pItem );
        void RemoveFromSelection( Track* pTrack );

        // Sanitize the current selection and remove any invalid items
        void EnsureValidSelection();

    protected:

        TrackContainer              m_trackContainer;

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

        TVector<TrackItem*>         m_selectedItems;
        TVector<Track*>             m_selectedTracks;

        bool                        m_isDirty = false;
    };
}