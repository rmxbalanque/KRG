#pragma once

#include "Engine/Animation/AnimationEvent.h"
#include "AnimGraphCommon.h"

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
        SampledEvent( Event const* pEvent, Percentage percentageThrough, NodeIndex sourceNodeIdx );
        SampledEvent( Flags stateEventType, StringID ID, Percentage percentageThrough, NodeIndex sourceNodeIdx );

        inline bool IsStateEvent() const { return m_flags.AreAnyFlagsSet( Flags::StateEntry, Flags::StateExecute, Flags::StateExit, Flags::StateTimed ); }
        inline Event const* GetEvent() const { KRG_ASSERT( !IsStateEvent() ); return m_eventData.m_pEvent; }
        inline StringID GetID() const { KRG_ASSERT( IsStateEvent() ); return m_eventData.m_stateEventID; }
        inline float GetWeight() const { return m_weight; }
        inline void SetWeight( float weight ) { KRG_ASSERT( weight >= 0.0f && weight <= 1.0f ); m_weight = weight; }
        inline Percentage GetPercentageThrough() const { return m_percentageThrough; }
        inline TBitFlags<Flags>& GetFlags() { return m_flags; }
        inline TBitFlags<Flags> const& GetFlags() const { return m_flags; }

        template<typename T>
        inline T const* GetEvent() const
        {
            KRG_ASSERT( !IsStateEvent() );
            KRG_ASSERT( m_pEvent->GetTypeInfo()->IsDerivedFrom( T::s_pTypeInfo.m_ID ) );
            return static_cast<T const*>( m_pEvent );
        }

    private:

        EventData                           m_eventData;                    // The event data
        float                               m_weight = 1.0f;                // The weight of the event when sampled
        Percentage                          m_percentageThrough = 1.0f;     // The percentage through the event we were when sampling
        TBitFlags<Flags>                    m_flags;                        // Misc flags
        NodeIndex                           m_sourceNodeIdx = InvalidIndex; // The index of the node that this event was sampled from
    };

    //-------------------------------------------------------------------------
    // Sample Event Buffer
    //-------------------------------------------------------------------------

    struct SampledEventRange
    {
        SampledEventRange() = default;
        KRG_FORCE_INLINE SampledEventRange( int16 index ) : m_startIdx( index ), m_endIdx( index ) {}
        KRG_FORCE_INLINE SampledEventRange( int16 startIndex, int16 endIndex ) : m_startIdx( startIndex ), m_endIdx( endIndex ) {}
        KRG_FORCE_INLINE bool IsValid() { return m_startIdx != InvalidIndex; }
        KRG_FORCE_INLINE void Reset() { m_startIdx = m_endIdx = InvalidIndex; }

    public:

        int16                               m_startIdx = InvalidIndex;
        int16                               m_endIdx = InvalidIndex;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API SampledEventsBuffer
    {
    public:

        inline void Reset() { m_events.clear(); }

        inline TVector<SampledEvent> const& GetEvents() const { return m_events; }

        inline int16 GetNumEvents() const { return (int16) m_events.size(); }

        inline void UpdateWeights( SampledEventRange range, float weightMultiplier )
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx < m_events.size() );
            for ( int16 i = range.m_startIdx; i < range.m_endIdx; i++ )
            {
                m_events[i].m_weight *= weightMultiplier;
            }
        }

        inline void SetFlag( SampledEventRange range, SampledEvent::Flags flag )
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx < m_events.size() );
            for ( int16 i = range.m_startIdx; i < range.m_endIdx; i++ )
            {
                m_events[i].m_flags.SetFlag( flag );
            }
        }

        //-------------------------------------------------------------------------

        inline SampledEvent& EmplaceAnimEvent( Event const* pEvent, Percentage percentageThrough, NodeIndex sourceNodeIdx )
        {
            return m_events.emplace_back( SampledEvent( pEvent, percentageThrough, sourceNodeIdx ) );
        }

        inline SampledEvent& EmplaceStateEvent( SampledEvent::Flags stateEventType, StringID ID, Percentage percentageThrough, NodeIndex sourceNodeIdx )
        {
            return m_events.emplace_back( SampledEvent( stateEventType, ID, percentageThrough, sourceNodeIdx ) );
        }

        // Helpers
        //-------------------------------------------------------------------------
        // These are not the cheapest, so use sparingly

        inline bool ContainStateEvent( SampledEventRange const& range, StringID ID, bool onlyFromActiveBranch = false ) const
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx < m_events.size() );

            if ( onlyFromActiveBranch )
            {
                for ( int32 i = range.m_startIdx; i < range.m_endIdx; i++ )
                {
                    auto& event = m_events[i];
                    if ( event.IsStateEvent() && !event.m_flags.IsFlagSet( SampledEvent::Flags::FromInactiveBranch ) && event.GetID() == ID )
                    {
                        return true;
                    }
                }
            }
            else // Dont check the active branch flag
            {
                for ( int32 i = range.m_startIdx; i < range.m_endIdx; i++ )
                {
                    auto& event = m_events[i];
                    if ( event.IsStateEvent() && event.GetID() == ID )
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        inline bool ContainSpecificStateEvent( SampledEventRange const& range, StringID ID, SampledEvent::Flags flag, bool onlyFromActiveBranch = false ) const
        {
            KRG_ASSERT( range.m_startIdx >= 0 && range.m_endIdx < m_events.size() );

            if ( onlyFromActiveBranch )
            {
                for ( int32 i = range.m_startIdx; i < range.m_endIdx; i++ )
                {
                    auto& event = m_events[i];
                    if ( event.IsStateEvent() && event.m_flags.IsFlagSet( flag ) && !event.m_flags.IsFlagSet( SampledEvent::Flags::FromInactiveBranch ) && event.GetID() == ID )
                    {
                        return true;
                    }
                }
            }
            else // Dont check the active branch flag
            {
                for ( int32 i = range.m_startIdx; i < range.m_endIdx; i++ )
                {
                    auto& event = m_events[i];
                    if ( event.IsStateEvent() && event.m_flags.IsFlagSet( flag ) && event.GetID() == ID )
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        inline bool ContainStateEvent( StringID ID, bool onlyFromActiveBranch = false ) const
        {
            return ContainStateEvent( SampledEventRange( 0, (int16) m_events.size() ), ID, onlyFromActiveBranch );
        }

        inline bool ContainSpecificStateEvent( StringID ID, SampledEvent::Flags flag, bool onlyFromActiveBranch = false ) const
        {
            return ContainSpecificStateEvent( SampledEventRange( 0, (int16) m_events.size() ), ID, flag, onlyFromActiveBranch );
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
}