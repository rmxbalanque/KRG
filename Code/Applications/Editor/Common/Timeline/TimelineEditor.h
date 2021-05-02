#pragma once

#include "TimelineTrack.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------
// Timeline Widget
//-------------------------------------------------------------------------
// Basic timeline/sequencer style widget

namespace KRG::Timeline
{
    class Editor
    {
    protected:

        enum class ViewUpdateMode : uint8
        {
            None,
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
                m_pHoveredTrack = nullptr;
                m_pHoveredItem = nullptr;
                m_hoveredItemMode = ItemEditMode::None;
                m_playheadTimeForMouse = -1.0f;
                m_snappedPlayheadTimeForMouse = -1.0f;
            }

            Track*                  m_pHoveredTrack = nullptr;
            Item*                   m_pHoveredItem = nullptr;
            float                   m_playheadTimeForMouse = -1.0f;
            float                   m_snappedPlayheadTimeForMouse = -1.0f;
            ItemEditMode            m_hoveredItemMode = ItemEditMode::None;
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
            Track const*            m_pTrackForEditedItem = nullptr;
            Item*                   m_pEditedItem = nullptr;
            TRange<float>           m_originalTimeRange;
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

            Track*                  m_pTrack = nullptr;
            Item*                   m_pItem = nullptr;
            float                   m_playheadTimeForMouse = -1.0f;
            bool                    m_isOpen = false;
        };

    public:

        Editor( TRange<int32> const& inTimeRange );
        virtual ~Editor();

        inline bool IsPlaying() const { return m_playState == PlayState::Playing; }
        inline bool IsPaused() const { return m_playState == PlayState::Paused; }

        inline bool IsLoopingEnabled() const { return m_isLoopingEnabled; }
        inline bool IsFrameSnappingEnabled() const { return m_isFrameSnappingEnabled; }

        inline Percentage GetPlayheadPositionAsPercentage() const { return Percentage( m_playheadTime / m_timeRange.m_max ); }

        inline TVector<Item*> const& GetSelectedItems() const { return m_selectedItems; }
        void ClearSelection();

    protected:

        // This is protected, since most of the time the client will want to provide a custom update function 
        // that handles the side-effects of the draw e.g. play head position update, track changes, etc...
        void Draw();

        inline TRange<float> GetTimeRangeAsFloatRange() const{ return TRange<float>( float( m_timeRange.m_min ), float( m_timeRange.m_max ) ); }
        inline TRange<float> GetViewRangeAsFloatRange() const{ return TRange<float>( float( m_viewRange.m_min ), float( m_viewRange.m_max ) ); }

        inline void SetTimeRange( TRange<int32> const& inRange ) { KRG_ASSERT( inRange.IsSetAndValid() ); m_timeRange = inRange; }
        inline void SetViewRange( TRange<int32> const& inRange ) { KRG_ASSERT( inRange.IsSetAndValid() ); m_viewRange = inRange; }

        // Called whenever the play state is switched
        virtual void OnPlayStateChanged() {}

        // Set the playhead position
        inline void SetPlayheadPosition( float inPosition ) { m_playheadTime = GetTimeRangeAsFloatRange().GetClampedValue( inPosition ); }

        // Set the playhead position from a percentage over the time range
        inline void SetPlayheadPositionAsPercentage( Percentage inPercentage ) { m_playheadTime = inPercentage.GetClamped( m_isLoopingEnabled ).ToFloat() * m_timeRange.m_max; }

    private:

        inline float ConvertPixelsToFrames( float inPixels ) const { return inPixels / m_pixelsPerFrame; }
        inline float ConvertFramesToPixels( float inTime ) const { return inTime * m_pixelsPerFrame; }
        inline void SetViewToStart() { m_viewUpdateMode = ViewUpdateMode::GoToStart; }
        inline void SetViewToEnd() { m_viewUpdateMode = ViewUpdateMode::GoToEnd; }

        //-------------------------------------------------------------------------

        // Is this a valid item ptr (i.e. is it present in any known track)
        bool IsValidPtr( Item const* pItem );

        // Is this a valid track ptr (i.e. is it in the list of tracks)
        bool IsValidPtr( Track const* pTrack );

        // Delete specified track
        void DeleteTrack( Track* pTrack );

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

        // Draw the add track menu
        virtual void DrawAddTracksMenu();

        // Draw the various context menus
        void DrawContextMenu();

        //-------------------------------------------------------------------------

        // Called each frame to process any mouse/keyboard input
        void HandleUserInput();

        // Selection
        //-------------------------------------------------------------------------

        inline bool IsSelected( Item const* pItem ) const { return VectorContains( m_selectedItems, const_cast<Item*>( pItem ) ); }
        inline bool IsSelected( Track const* pTrack ) const { return VectorContains( m_selectedTracks, const_cast<Track*>( pTrack ) ); }

        // Set the selection to a single item
        void SetSelection( Item* pItem );
        void SetSelection( Track* pTrack );

        // Add item to the current selection
        void AddToSelection( Item* pItem );
        void AddToSelection( Track* pTrack );

        // Remove an item from the current selection
        void RemoveFromSelection( Item* pItem );
        void RemoveFromSelection( Track* pTrack );

        void EnsureValidSelection();

    protected:

        TVector<Track*>         m_tracks;

        // The total editable time range
        TRange<int32>           m_timeRange = TRange<int32>( 0, 0 );

        // The current visible time range
        TRange<int32>           m_viewRange = TRange<int32>( 0, 0 );

        float                   m_pixelsPerFrame = 10.0f;
        float                   m_playheadTime = 0.0f;

        PlayState               m_playState = PlayState::Paused;
        ViewUpdateMode          m_viewUpdateMode = ViewUpdateMode::None;
        bool                    m_isLoopingEnabled = false;
        bool                    m_isFrameSnappingEnabled = true;
        bool                    m_isDraggingPlayhead = false;

        MouseState              m_mouseState;
        ItemEditState           m_itemEditState;
        ContextMenuState        m_contextMenuState;

        TVector<Item*>          m_selectedItems;
        TVector<Track*>         m_selectedTracks;
    };
}