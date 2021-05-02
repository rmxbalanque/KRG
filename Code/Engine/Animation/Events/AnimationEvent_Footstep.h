#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Events
{
    class KRG_ENGINE_ANIMATION_API FootstepEvent final : public Event
    {
        KRG_REGISTER_TYPE;
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( Event ), m_ID );

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

        virtual TypeSystem::TypeInfo const* GetTypeInfo() const override { return FootstepEvent::s_pTypeInfo; }
        virtual StringID GetSyncEventID() const override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventTypeName() const override { return "Footstep"; }
        virtual InlineString<100> GetDisplayText() const override;
        virtual AllowedTypes GetAllowedTypes() const override { return AllowedTypes::Duration; }
        #endif

    private:

        EXPOSE Phase         m_phase = Phase::LeftFootDown;
    };
}