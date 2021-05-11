#pragma once

#include "Tools/Animation/_Module/API.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "Engine/Animation/AnimationEvent.h"

//-------------------------------------------------------------------------
// Stores tools only information for event track data
//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    struct KRG_TOOLS_ANIMATION_API EventTrackInfo
    {
        KRG_REGISTER_TYPE;

    public:

        EXPOSE Event::AllowedTypes          m_type;
        EXPOSE bool                         m_isSyncTrack = false;
        EXPOSE TypeSystem::TypeID           m_eventTypeID;

        TypeSystem::TypeInfo const*         m_pEventTypeInfo = nullptr;
    };
}