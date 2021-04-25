#pragma once

#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Range.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------
// Timeline Widget
//-------------------------------------------------------------------------

namespace KRG
{
    class Item
    {
    public:

        inline float GetLength() const { return m_endTime - m_startTime; }

        float               m_startTime;
        float               m_endTime;
    };

    //-------------------------------------------------------------------------

    class Track
    {
    public:

        TVector<Item*>      m_items;
    };

    //-------------------------------------------------------------------------

    class TimelineEditor
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

    public:

        TimelineEditor( TRange<int32> const& inTimeRange );

        virtual ~TimelineEditor() = default;

        inline bool IsPlaying() const { return m_playState == PlayState::Playing; }
        inline bool IsPaused() const { return m_playState == PlayState::Paused; }

        inline bool IsLoopingEnabled() const { return m_isLoopingEnabled; }
        inline bool IsFrameSnappingEnabled() const { return m_isFrameSnappingEnabled; }

        inline Percentage GetPlayheadPositionAsPercentage() const { return Percentage( m_playheadTime / m_timeRange.m_max ); }

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

        void DrawTimelineControls( ImRect const& controlsRect );
        void DrawTimeline( ImRect const& timelineRect );
        void DrawPlayhead( ImRect const& timelineRect );

        inline float ConvertPixelsToFrames( float inPixels ) const { return inPixels / m_pixelsPerFrame; }
        inline float ConvertFramesToPixels( float inTime ) const { return inTime * m_pixelsPerFrame; }
        inline void SetViewToStart() { m_viewUpdateMode = ViewUpdateMode::GoToStart; }
        inline void SetViewToEnd() { m_viewUpdateMode = ViewUpdateMode::GoToEnd; }

        void SetPlayState( PlayState newPlayState );

    protected:

        TVector<Track*>     m_tracks;

        // The total editable time range
        TRange<int32>       m_timeRange = TRange<int32>( 0, 0 );

        // The current visible time range
        TRange<int32>       m_viewRange = TRange<int32>( 0, 0 );

        float               m_pixelsPerFrame = 10.0f;
        float               m_playheadTime = 0.0f;

        PlayState           m_playState = PlayState::Paused;
        ViewUpdateMode      m_viewUpdateMode = ViewUpdateMode::None;
        bool                m_isLoopingEnabled = false;
        bool                m_isFrameSnappingEnabled = true;
        bool                m_isDraggingPlayhead = false;
    };
}