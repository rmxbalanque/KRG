#include "AnimationEvent_Footstep.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Events
{
    char const* g_phaseNames[4] =
    {
        "Left Foot Down",
        "Left Foot Passing",
        "Right Foot Down",
        "Right Foot Passing"
    };

    //-------------------------------------------------------------------------

    StringID FootstepEvent::GetSyncEventID() const
    {
        return StringID( g_phaseNames[(int32) m_phase] );
    }

    #if KRG_DEVELOPMENT_TOOLS
    InlineString<100> FootstepEvent::GetDisplayText() const
    {
        return g_phaseNames[(int32) m_phase];
    }
    #endif
}