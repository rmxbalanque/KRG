#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API IDEvent final : public Event
    {
        KRG_REGISTER_TYPE( IDEvent );

    public:

        inline StringID const& GetID() const { return m_ID; }
        virtual StringID GetSyncEventID() const override { return m_ID; }

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventName() const override { return "ID"; }
        virtual bool AllowMultipleTracks() const override { return true; }
        virtual InlineString<100> GetDisplayText() const override { return m_ID.ToString(); }
        virtual EventType GetEventType() const override { return EventType::Both; }
        #endif

    private:

        KRG_EXPOSE StringID         m_ID;
    };
}