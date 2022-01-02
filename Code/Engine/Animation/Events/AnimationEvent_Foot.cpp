#include "AnimationEvent_Foot.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    char const* g_phaseNames[4] =
    {
        "Left Foot Down",
        "Right Foot Passing",
        "Right Foot Down",
        "Left Foot Passing",
    };

    //-------------------------------------------------------------------------

    StringID FootEvent::GetSyncEventID() const
    {
        return StringID( g_phaseNames[(int32) m_phase] );
    }

    #if KRG_DEVELOPMENT_TOOLS
    InlineString FootEvent::GetDisplayText() const
    {
        return g_phaseNames[(int32) m_phase];
    }
    #endif
}