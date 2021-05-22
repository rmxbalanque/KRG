#pragma once

#include "_Module/API.h"
#include "System/Core/Time/Time.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Math/NumericRange.h"

//-------------------------------------------------------------------------
// Animation Event
//-------------------------------------------------------------------------
// Base class for all animation events

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API Event : public IRegisteredType
    {
        KRG_REGISTER_TYPE( Event );

        friend struct EventManipulator;

    public:

        enum class EventType
        {
            KRG_REGISTER_ENUM

            Immediate,
            Duration,
            Both
        };

    public:

        Event() = default;
        virtual ~Event() = default;

        inline Seconds GetStartTime() const { return m_startTime; }
        inline Seconds GetDuration() const { return m_duration; }
        inline bool IsImmediateEvent() const { return m_duration == 0; }
        inline bool IsDurationEvent() const { return m_duration > 0; }

        // Get the time range for this event (in seconds)
        KRG_FORCE_INLINE FloatRange GetTimeRange() const { return FloatRange( m_startTime, m_startTime + m_duration ); }

        // Optional: Allow the track to return a specific sync event ID
        virtual StringID GetSyncEventID() const { return StringID(); }

        // Event properties
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        virtual InlineString<100> GetDisplayText() const = 0;
        virtual char const* GetEventName() const = 0;
        virtual bool AllowMultipleTracks() const { return false; }
        virtual int32 GetMaxEventsAllowedPerTrack() const { return -1; }
        virtual EventType GetEventType() const { return EventType::Duration; }
        #endif

    protected:

        EXPOSE Seconds         m_startTime = 0.0f;
        EXPOSE Seconds         m_duration = 0.0f;
    };
}