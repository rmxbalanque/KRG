#pragma once

#include "_Module/API.h"
#include "System/Core/Time/Time.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------
// Animation Event
//-------------------------------------------------------------------------
// Base class for all animation events

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API Event
    {
        KRG_REGISTER_TYPE;
        KRG_SERIALIZE_MEMBERS( m_startTime, m_duration );

        friend struct EventManipulator;

    public:

        enum class AllowedTypes
        {
            KRG_REGISTER_ENUM

            Immediate,
            Duration,
            Both
        };

    public:

        Event() = default;
        virtual ~Event() = default;

        virtual TypeSystem::TypeInfo const* GetTypeInfo() const { return s_pTypeInfo; }

        inline Seconds GetStartTime() const { return m_startTime; }
        inline Seconds GetDuration() const { return m_duration; }
        inline bool IsImmediateEvent() const { return m_duration == 0; }
        inline bool IsDurationEvent() const { return m_duration > 0; }

        // Optional: Allow the track to return a specific sync event ID
        virtual StringID GetSyncEventID() const { return StringID(); }

        // Event properties
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        virtual InlineString<100> GetDisplayText() const = 0;
        virtual char const* GetEventTypeName() const = 0;
        virtual bool AllowMultipleTracks() const { return false; }
        virtual int32 GetMaxEventsAllowedPerTrack() const { return -1; }
        virtual AllowedTypes GetAllowedTypes() const { return AllowedTypes::Duration; }
        #endif

    protected:

        EXPOSE Seconds         m_startTime = 0.0f;
        EXPOSE Seconds         m_duration = 0.0f;
    };
}