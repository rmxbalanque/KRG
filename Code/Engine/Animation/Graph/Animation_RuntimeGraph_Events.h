#pragma once

#include "Engine/Animation/AnimationEvent.h"
#include "Animation_RuntimeGraph_Common.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    //-------------------------------------------------------------------------
    // A sampled event from the graph
    //-------------------------------------------------------------------------

    struct KRG_ENGINE_ANIMATION_API SampledEvent
    {
        friend class SampledEventsBuffer;

        union EventData
        {
            EventData() : m_pEvent( nullptr ) {}
            EventData( Event const* pEvent ) : m_pEvent( pEvent ) {}
            EventData( StringID ID ) : m_stateEventID( ID ) {}

            Event const*                        m_pEvent = nullptr;
            StringID                            m_stateEventID;
        };

    public:

        enum class Flags
        {
            Ignored,                // Should we ignore this event?
            FromInactiveBranch,     // Is this event from an inactive branch in the graph/blend
            SampledInReverse,       // Was this event sampled from an animation playing in reverse

            // State events
            StateEntry,             // Is this a state transition in event
            StateExecute,           // Is this a "fully in state" event
            StateExit,              // Is this a state transition out event
            StateTimed,             // Timed event coming from a state
        };

    public:

        SampledEvent() = default;
        SampledEvent( NodeIndex sourceNodeIdx, Event const* pEvent, Percentage percentageThrough );
        SampledEvent( NodeIndex sourceNodeIdx, Flags stateEventType, StringID ID, Percentage percentageThrough );

        inline NodeIndex GetSourceNodeIndex() const { return m_sourceNodeIdx; }
        inline bool IsStateEvent() const { return m_flags.AreAnyFlagsSet( Flags::StateEntry, Flags::StateExecute, Flags::StateExit, Flags::StateTimed ); }
        inline bool IsFromActiveBranch() const { return !m_flags.IsFlagSet( Flags::FromInactiveBranch ); }
        inline StringID GetID() const { KRG_ASSERT( IsStateEvent() ); return m_eventData.m_stateEventID; }
        inline float GetWeight() const { return m_weight; }
        inline void SetWeight( float weight ) { KRG_ASSERT( weight >= 0.0f && weight <= 1.0f ); m_weight = weight; }
        inline Percentage GetPercentageThrough() const { return m_percentageThrough; }

        //-------------------------------------------------------------------------

        inline Event const* GetEvent() const { KRG_ASSERT( !IsStateEvent() ); return m_eventData.m_pEvent; }

        template<typename T>
        inline T const* GetEvent() const
        {
            KRG_ASSERT( !IsStateEvent() );
            KRG_ASSERT( m_eventData.m_pEvent->GetTypeInfo()->IsDerivedFrom( T::s_pTypeInfo->m_ID ) );
            return static_cast<T const*>( m_eventData.m_pEvent );
        }

        // Flags
        //-------------------------------------------------------------------------

        inline TBitFlags<Flags>& GetFlags() { return m_flags; }
        inline TBitFlags<Flags> const& GetFlags() const { return m_flags; }
        inline void SetFlag( Flags flag, bool value ) { m_flags.SetFlag( flag, value ); }

    private:

        EventData                           m_eventData;                    // The event data
        float                               m_weight = 1.0f;                // The weight of the event when sampled
        Percentage                          m_percentageThrough = 1.0f;     // The percentage through the event we were when sampling
        TBitFlags<Flags>                    m_flags;                        // Misc flags
        NodeIndex                           m_sourceNodeIdx = InvalidIndex; // The index of the node that this event was sampled from
    };

    //-------------------------------------------------------------------------
    // Sample Event Range
    //-------------------------------------------------------------------------
    // A range of indices into the sampled events buffer: [startIndex, endIndex);
    // The end index is not part of each range, it is the start index for the next range or the end of the sampled events buffer

    struct SampledEventRange
    {
        SampledEventRange() = default;
        KRG_FORCE_INLINE SampledEventRange( int16 index ) : m_startIdx( index ), m_endIdx( index ) {}
        KRG_FORCE_INLINE SampledEventRange( int16 startIndex, int16 endIndex ) : m_startIdx( startIndex ), m_endIdx( endIndex ) {}

        KRG_FORCE_INLINE bool IsValid() const { return m_startIdx != InvalidIndex && m_endIdx >= m_startIdx; }
        KRG_FORCE_INLINE int32 GetLength() const { return m_endIdx - m_startIdx; }
        KRG_FORCE_INLINE void Reset() { m_startIdx = m_endIdx = InvalidIndex; }

    public:

        int16                               m_startIdx = InvalidIndex;
        int16                               m_endIdx = InvalidIndex;
    };

    //-------------------------------------------------------------------------
    // Sample Event Buffer
    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API SampledEventsBuffer
    {
    public:

        inline void Reset() { m_events.clear(); }

        inline TVector<SampledEvent> const& GetEvents() const { return m_events; }

        inline int16 GetNumEvents() const { return (int16) m_events.size(); }

        // Is the supplied range valid for the current state of the buffer?
        inline bool IsValidRange( SampledEventRange range ) const
        {
            if ( m_events.empty() )
            {
                return range.m_startIdx == 0 && range.m_endIdx == 0;
            }
            else
            {
                return range.m_startIdx >= 0 && range.m_endIdx <= m_events.size();
            }
        }

        // Update all event weights for the supplied range
        inline void UpdateWeights( SampledEventRange range, float weightMultiplier )
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx <= m_events.size() );
            for ( int16 i = range.m_startIdx; i < range.m_endIdx; i++ )
            {
                m_events[i].m_weight *= weightMultiplier;
            }
        }

        // Set a flag on all events for the supplied range
        inline void SetFlag( SampledEventRange range, SampledEvent::Flags flag )
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx < m_events.size() );
            for ( int16 i = range.m_startIdx; i < range.m_endIdx; i++ )
            {
                m_events[i].m_flags.SetFlag( flag );
            }
        }

        //-------------------------------------------------------------------------

        inline SampledEvent& EmplaceAnimEvent( NodeIndex sourceNodeIdx, Event const* pEvent, Percentage percentageThrough )
        {
            return m_events.emplace_back( SampledEvent( sourceNodeIdx, pEvent, percentageThrough ) );
        }

        inline SampledEvent& EmplaceStateEvent( NodeIndex sourceNodeIdx, SampledEvent::Flags stateEventType, StringID ID )
        {
            KRG_ASSERT( stateEventType >= SampledEvent::Flags::StateEntry );
            return m_events.emplace_back( SampledEvent( sourceNodeIdx, stateEventType, ID, 1.0f ) );
        }

        // Helpers
        //-------------------------------------------------------------------------
        // These are not the cheapest, so use sparingly. These functions also take the ignored flag into account.

        inline bool ContainsStateEvent( SampledEventRange const& range, StringID ID, bool onlyFromActiveBranch = false ) const
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx < m_events.size() );

            for ( int32 i = range.m_startIdx; i < range.m_endIdx; i++ )
            {
                auto& event = m_events[i];

                if ( event.m_flags.IsFlagSet( SampledEvent::Flags::Ignored ) )
                {
                    continue;
                }

                if ( onlyFromActiveBranch && event.m_flags.IsFlagSet( SampledEvent::Flags::FromInactiveBranch ) )
                {
                    continue;
                }

                if ( event.IsStateEvent() && event.GetID() == ID )
                {
                    return true;
                }
            }

            return false;
        }

        inline bool ContainsSpecificStateEvent( SampledEventRange const& range, StringID ID, SampledEvent::Flags flag, bool onlyFromActiveBranch = false ) const
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx < m_events.size() );

            for ( int32 i = range.m_startIdx; i < range.m_endIdx; i++ )
            {
                auto& event = m_events[i];

                if ( event.m_flags.IsFlagSet( SampledEvent::Flags::Ignored ) )
                {
                    continue;
                }

                if ( onlyFromActiveBranch && event.m_flags.IsFlagSet( SampledEvent::Flags::FromInactiveBranch ) )
                {
                    continue;
                }

                if ( event.IsStateEvent() && event.m_flags.IsFlagSet( flag ) && !event.m_flags.IsFlagSet( SampledEvent::Flags::FromInactiveBranch ) && event.GetID() == ID )
                {
                    return true;
                }
            }

            return false;
        }

        inline bool ContainsStateEvent( StringID ID, bool onlyFromActiveBranch = false ) const
        {
            return ContainsStateEvent( SampledEventRange( 0, (int16) m_events.size() ), ID, onlyFromActiveBranch );
        }

        inline bool ContainsSpecificStateEvent( StringID ID, SampledEvent::Flags flag, bool onlyFromActiveBranch = false ) const
        {
            return ContainsSpecificStateEvent( SampledEventRange( 0, (int16) m_events.size() ), ID, flag, onlyFromActiveBranch );
        }

        // Operators
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE TVector<SampledEvent>::iterator begin() { return m_events.begin(); }
        KRG_FORCE_INLINE TVector<SampledEvent>::iterator end() { return m_events.end(); }
        KRG_FORCE_INLINE TVector<SampledEvent>::const_iterator begin() const { return m_events.begin(); }
        KRG_FORCE_INLINE TVector<SampledEvent>::const_iterator end() const{ return m_events.end(); }
        KRG_FORCE_INLINE SampledEvent& operator[]( uint32 i ) { KRG_ASSERT( i < m_events.size() ); return m_events[i]; }
        KRG_FORCE_INLINE SampledEvent const& operator[]( uint32 i ) const { KRG_ASSERT( i < m_events.size() ); return m_events[i]; }

    public:

        TVector<SampledEvent>               m_events;
    };

    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------

    [[nodiscard]] SampledEventRange CombineAndUpdateEvents( SampledEventsBuffer& sampledEventsBuffer, SampledEventRange const& eventRange0, SampledEventRange const& eventRange1, float const blendWeight );
}