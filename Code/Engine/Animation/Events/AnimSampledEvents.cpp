#include "AnimSampledEvents.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        //SampledEvent const* SampledEventsBuffer::GetDominantSyncEvent() const
        //{
        //    SampledEvent const* pDominantEvent = nullptr;

        //    auto const numEvents = m_events.size();
        //    for ( auto i = 0u; i < numEvents; i++ )
        //    {
        //        if ( m_events[i].IsSyncEvent() )
        //        {
        //            if ( pDominantEvent == nullptr )
        //            {
        //                pDominantEvent = &m_events[i];
        //            }
        //            else // compare weight of event
        //            {
        //                if ( pDominantEvent->GetWeight() < m_events[i].GetWeight() )
        //                {
        //                    pDominantEvent = &m_events[i];
        //                }
        //            }
        //        }
        //    }

        //    return pDominantEvent;
        //}
    }
}