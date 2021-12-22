#include "AnimationSyncTrack.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    SyncTrack const SyncTrack::s_defaultTrack;

    //-------------------------------------------------------------------------

    SyncTrack::SyncTrack()
    {
        m_syncEvents.emplace_back( Event() );
    }

    SyncTrack::SyncTrack( TInlineVector<EventMarker, 10> const& inMarkers, int32 eventOffset )
    {
        // If there are no markers, then just default-construct
        if ( inMarkers.empty() )
        {
            m_syncEvents.emplace_back( Event() );
            return;
        }

        // Reflect sync events
        //-------------------------------------------------------------------------

        m_syncEvents.reserve( inMarkers.size() );

        for ( auto const& syncMarker : inMarkers )
        {
            // Ensure that the supplied events are sorted
            if ( m_syncEvents.size() > 0  )
            {
                KRG_ASSERT( syncMarker.m_startTime > m_syncEvents.back().m_startTime );
            }

            m_syncEvents.emplace_back( Event( syncMarker.m_ID, syncMarker.m_startTime ) );
        }

        int32 const numSyncEvents = (int32) m_syncEvents.size();
        KRG_ASSERT( numSyncEvents != 0 );

        // Calculate the sync periods
        //-------------------------------------------------------------------------

        for ( int32 i = 0; i < numSyncEvents - 1; i++ )
        {
            m_syncEvents[i].m_duration = m_syncEvents[i + 1].m_startTime - m_syncEvents[i].m_startTime;
        }

        // Ensure that last event covers the entire remaining duration, this might wrap around to the start of the first event
        m_syncEvents.back().m_duration = 1.0f - ( m_syncEvents.back().m_startTime - m_syncEvents[0].m_startTime );

        // Set event offset
        //-------------------------------------------------------------------------

        m_startEventOffset = ClampIndexToTrack( eventOffset );
    }

    //-------------------------------------------------------------------------

    SyncTrack::SyncTrack( SyncTrack const& track0, SyncTrack const& track1, float const blendWeight )
    {
        int32 const numEventsTrack0 = (int32) track0.m_syncEvents.size();
        int32 const numEventsTrack1 = (int32) track1.m_syncEvents.size();

        KRG_ASSERT( blendWeight >= 0.0f && blendWeight <= 1.0f );
        KRG_ASSERT( numEventsTrack0 > 0 && numEventsTrack1 > 0 );

        int32 const minEvents = Math::Min( numEventsTrack0, numEventsTrack1 );
        int32 const maxEvents = Math::Max( numEventsTrack0, numEventsTrack1 );

        // Calculate the number of events in the blended track (the LCM of the two tracks)
        int32 const LCM = Math::LowestCommonMultiple( minEvents, maxEvents );
        float durationScaleTrack0 = float( numEventsTrack0 ) / LCM;
        float durationScaleTrack1 = float( numEventsTrack1 ) / LCM;

        // Create new sync events
        Percentage blendedStartPercent = 0.0f;
        Percentage blendedDuration = 0.0f;

        m_syncEvents.reserve( LCM );
        for ( auto i = 0; i < LCM; i++ )
        {
            // Take into account any event offsets
            auto const& event0 = track0.GetEvent( i );
            auto const& event1 = track1.GetEvent( i );
            float const event0Duration = (float) event0.m_duration * durationScaleTrack0;
            float const event1Duration = (float) event1.m_duration * durationScaleTrack1;

            blendedDuration = Math::Lerp( event0Duration, event1Duration, blendWeight );
            m_syncEvents.emplace_back( Event( blendWeight > 0.5f ? event1.m_ID : event0.m_ID, blendedStartPercent, blendedDuration ) );
            blendedStartPercent += blendedDuration;
        }

        // Normalize blended sync track
        float const normalizedScalingFactor = 1.0f / blendedStartPercent; // BlendedStartPercent represents the final blended track duration i.e. the start of the next event
        for ( auto i = 0; i < LCM; i++ )
        {
            m_syncEvents[i].m_startTime = (float) m_syncEvents[i].m_startTime * normalizedScalingFactor;
            m_syncEvents[i].m_duration = (float) m_syncEvents[i].m_duration * normalizedScalingFactor;
        }

        // Make sure the last event reaches the end of the track
        KRG_ASSERT( m_syncEvents.back().m_startTime <= 1.0f );
        m_syncEvents.back().m_duration = 1.0f - m_syncEvents.back().m_startTime;
    }

    SyncTrackTime SyncTrack::GetTime( Percentage const percentage, bool withOffset ) const
    {
        int32 const numSyncEvents = (int32) m_syncEvents.size();

        int32 loopCount = 0;
        Percentage percentageThrough = 0.0f;
        percentage.GetLoopCountAndNormalizedTime( loopCount, percentageThrough );

        KRG_ASSERT( numSyncEvents > 0 );
        SyncTrackTime time;

        // If the playback percent is before the start of the first event
        if ( percentageThrough < m_syncEvents[0].m_startTime )
        {
            // We are dealing with a looping sequence so the event is the last one
            time.m_eventIdx = (uint32) numSyncEvents - 1;
            KRG_ASSERT( m_syncEvents[time.m_eventIdx].m_duration > Math::Epsilon );

            float const EventDelta = m_syncEvents[0].m_startTime - percentageThrough;
            time.m_percentageThrough = ( (float) m_syncEvents.back().m_duration - EventDelta ) / m_syncEvents.back().m_duration;
        }
        else // Search the sync track for the event and percent
        {
            for ( auto Idx = 0; Idx < numSyncEvents; Idx++ )
            {
                // Find the first event whose end time (i.e. start time of next event) is greater than the playback percent
                if ( ( m_syncEvents[Idx].m_startTime + m_syncEvents[Idx].m_duration ) >= percentageThrough )
                {
                    KRG_ASSERT( m_syncEvents[Idx].m_duration > Math::Epsilon );

                    time.m_eventIdx = Idx;
                    time.m_percentageThrough = ( percentageThrough - m_syncEvents[Idx].m_startTime ) / m_syncEvents[Idx].m_duration;

                    // Handle looping sequences
                    while ( time.m_percentageThrough > 1.0f )
                    {
                        time.m_percentageThrough -= 1.0f;
                    }
                    break;
                }
            }
        }

        // Make sure we have found a valid event and percent through
        KRG_ASSERT( time.m_eventIdx != InvalidIndex );
        KRG_ASSERT( time.m_percentageThrough >= 0.0f && time.m_percentageThrough <= 1.0f );

        // Take into account loop count
        time.m_eventIdx += loopCount * numSyncEvents;

        // Adjust sync track position to take account the offset
        int32 const offset = withOffset ? m_startEventOffset : 0;
        time.m_eventIdx = ClampIndexToTrack( time.m_eventIdx - offset );
        return time;
    }

    Percentage SyncTrack::GetPercentageThrough( SyncTrackTime const& time, bool withOffset ) const
    {
        KRG_ASSERT( time.m_percentageThrough >= 0.0f && time.m_percentageThrough <= 1.0f );

        // Adjust event index based on event offset and clamp to the track
        int32 offset = withOffset ? m_startEventOffset : 0;
        int32 const eventIdx = ClampIndexToTrack( time.m_eventIdx + offset );
        Percentage percentageThroughSyncTrack = m_syncEvents[eventIdx].m_startTime + ( m_syncEvents[eventIdx].m_duration * time.m_percentageThrough );

        // Handle looping sequences
        while ( percentageThroughSyncTrack > 1.0f )
        {
            percentageThroughSyncTrack -= 1.0f;
        }

        KRG_ASSERT( percentageThroughSyncTrack >= 0.0f && percentageThroughSyncTrack <= 1.0f );
        return percentageThroughSyncTrack;
    }

    Percentage SyncTrack::GetEventDuration( int32 const eventIdx, bool withOffset ) const
    {
        KRG_ASSERT( eventIdx >= 0 && eventIdx < m_syncEvents.size() );
        Percentage const startTime = GetPercentageThrough( SyncTrackTime( eventIdx, 0.0f ), withOffset );
        Percentage const endTime = GetPercentageThrough( SyncTrackTime( eventIdx, 1.0f ), withOffset );

        Percentage duration = 0.0f;
        if ( endTime < startTime )
        {
            duration = 1.0f - startTime;
            duration += endTime;
        }
        else
        {
            duration = endTime - startTime;
        }

        KRG_ASSERT( duration >= 0.0f && duration <= 1.0f );
        return duration;
    }

    Percentage SyncTrack::CalculatePercentageCovered( SyncTrackTime const& startTime, SyncTrackTime const& endTime ) const
    {
        float syncTimeDistance = 0;

        // Handle the same event case
        if ( startTime.m_eventIdx == endTime.m_eventIdx && startTime.m_percentageThrough < endTime.m_percentageThrough )
        {
            syncTimeDistance = endTime.m_percentageThrough - startTime.m_percentageThrough;
        }
        else // Calculate the multi-event/looped distance
        {
            // calculate the distance to the end of the start event
            syncTimeDistance = ( 1.0f - startTime.m_percentageThrough );

            int32 eventIdx = startTime.m_eventIdx + 1;
            while ( eventIdx != endTime.m_eventIdx )
            {
                // If we go past the number of event, then loop back around
                if ( eventIdx == m_syncEvents.size() )
                {
                    eventIdx = 0;
                }
                else // Add this event's distance and continue
                {
                    syncTimeDistance += 1;
                    eventIdx++;
                }
            }

            syncTimeDistance += endTime.m_percentageThrough;
        }

        //-------------------------------------------------------------------------

        Percentage const percentageCovered( syncTimeDistance / m_syncEvents.size() );
        return percentageCovered;
    }
}