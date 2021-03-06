#include "UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void UpdateContext::SetTimeScale( float scale )
    {
        KRG_ASSERT( scale > 0 && scale < 100 );
        m_timeScale = scale;
    }

    void UpdateContext::UpdateDeltaTime( Milliseconds deltaTime )
    {
        KRG_ASSERT( deltaTime >= 0 );

        // Update internal time
        m_timeScale = 1.0f;
        m_deltaTime = deltaTime.ToSeconds();
        m_frameID++;
    }
}