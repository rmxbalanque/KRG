#include "AnimGraphEvents.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    SampledEvent::SampledEvent( Event const* pEvent, Percentage percentageThrough, NodeIndex sourceNodeIdx )
        : m_eventData( pEvent )
        , m_percentageThrough( percentageThrough.GetClamped( false ) )
        , m_sourceNodeIdx( sourceNodeIdx )
    {
        KRG_ASSERT( pEvent != nullptr && sourceNodeIdx != InvalidIndex );
    }

    SampledEvent::SampledEvent( Flags stateEventType, StringID ID, Percentage percentageThrough, NodeIndex sourceNodeIdx )
        : m_eventData( ID )
        , m_percentageThrough( percentageThrough.GetClamped( false ) )
        , m_flags( stateEventType )
        , m_sourceNodeIdx( sourceNodeIdx )
    {
        KRG_ASSERT( stateEventType >= Flags::StateEntry && sourceNodeIdx != InvalidIndex );
    }
}