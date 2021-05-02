#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Events
{
    class KRG_ENGINE_ANIMATION_API IDEvent final : public Event
    {
        KRG_REGISTER_TYPE;
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( Event ), m_ID );

    public:

        virtual TypeSystem::TypeInfo const* GetTypeInfo() const override { return IDEvent::s_pTypeInfo; }
        virtual StringID GetSyncEventID() const override { return m_ID; }

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventTypeName() const override { return "ID"; }
        virtual InlineString<100> GetDisplayText() const override { return m_ID.ToString(); }
        virtual AllowedTypes GetAllowedTypes() const override { return AllowedTypes::Both; }
        #endif

    private:

        EXPOSE StringID         m_ID;
    };
}