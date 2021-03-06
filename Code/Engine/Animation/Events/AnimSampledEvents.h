#pragma once

#include "Engine/Animation/AnimationEvent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        //class SampledEvent
        //{
        //public:

        //    SampledEvent()
        //        : m_percentageThrough( 1.0f )
        //        , m_weight( 1.0f )
        //    {}

        //    SampledEvent( Event const* pEvent, Percent const percentageThrough, bool sourceIsReversed )
        //        : m_pSourceEvent( pEvent )
        //        , m_percentageThrough( percentageThrough )
        //        , m_weight( 1.0f )
        //        , m_sourceAnimationReversed( sourceIsReversed )
        //    {}

        //    inline void UpdateWeight( float scaleMultiplier ) { m_weight *= scaleMultiplier; }

        //    inline bool IsSyncEvent() const { return m_pSourceEvent->IsSyncEvent(); }
        //    inline bool IsMarkupEvent() const { return m_pSourceEvent->IsMarkupEvent(); }

        //    inline Percent const GetPercentageThrough() const { return m_percentageThrough; }
        //    inline Percent const GetDuration() const { return m_pSourceEvent->GetDuration(); }
        //    inline Percent const GetWeight() const { return m_weight; }
        //    inline Event const* GetSourceEvent() const { return m_pSourceEvent; }
        //    inline bool const IsSourcePlayingInReverse() const { return m_sourceAnimationReversed; }

        //private:

        //    Event const*                m_pSourceEvent;             // The actual animation event
        //    Seconds               m_duration;                 // Duration for the event in seconds
        //    Percent               m_percentageThrough;        // How far through the event we are (only relevant for events with duration > 0)
        //    float                         m_weight;                   // Weight of this event - adjusted through blending operations (always starts at 1)
        //    bool                        m_sourceAnimationReversed;  // Was this event sampled from an animation playing in reverse
        //};

        ////-------------------------------------------------------------------------

        //struct SampledEventRange : public Range<uint16>
        //{
        //    static uint16 const InvalidEventIndex = 0xFFFF;
        //    SampledEventRange() : Range( InvalidEventIndex, InvalidEventIndex ) {}
        //    SampledEventRange( uint16 const startIndex ) : Range( startIndex, startIndex ) {}
        //};

        ////-------------------------------------------------------------------------

        //class KRG_ENGINE_ANIMATION_API SampledEventsBuffer
        //{
        //public:

        //    inline void UpdateEventWeights( SampledEventRange range, float scaleMultiplier );
        //    SampledEvent const* GetDominantSyncEvent() const;

        //    SampledEvent& operator[]( uint32 i ) { KRG_ASSERT( i < m_events.size() ); return m_events[i]; }
        //    SampledEvent const& operator[]( uint32 i ) const { KRG_ASSERT( i < m_events.size() ); return m_events[i]; }

        //public:

        //    TVector<SampledEvent> m_events;
        //};

        ////-------------------------------------------------------------------------

        //void SampledEventsBuffer::UpdateEventWeights( SampledEventRange range, float scaleMultiplier )
        //{
        //    for ( auto i = range.m_start; i < range.m_end; i++ )
        //    {
        //        m_events[i].UpdateWeight( scaleMultiplier );
        //    }
        //}
    }
}