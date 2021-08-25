#include "AnimationEvent_Foot.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    char const* g_phaseNames[4] =
    {
        "Left Foot Down",
        "Left Foot Passing",
        "Right Foot Down",
        "Right Foot Passing"
    };

    //-------------------------------------------------------------------------

    StringID FootEvent::GetSyncEventID() const
    {
        return StringID( g_phaseNames[(int32) m_phase] );
    }

    #if KRG_DEVELOPMENT_TOOLS
    InlineString<100> FootEvent::GetDisplayText() const
    {
        return g_phaseNames[(int32) m_phase];
    }
    #endif
}