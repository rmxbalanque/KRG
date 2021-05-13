#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Events
{
    class KRG_ENGINE_ANIMATION_API FootstepEvent final : public Event
    {
        KRG_REGISTER_TYPE( FootstepEvent );

    public:

        enum Phase
        {
            KRG_REGISTER_ENUM

            LeftFootDown = 0,
            LeftFootPassing,
            RightFootDown,
            RightFootPassing
        };

    public:

        virtual StringID GetSyncEventID() const override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventName() const override { return "Footstep"; }
        virtual InlineString<100> GetDisplayText() const override;
        virtual EventType GetEventType() const override { return EventType::Duration; }
        #endif

    private:

        EXPOSE Phase         m_phase = Phase::LeftFootDown;
    };
}