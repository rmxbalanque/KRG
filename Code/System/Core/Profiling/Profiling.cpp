#include "Profiling.h"
#include "Superluminal/PerformanceAPI.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Profiling
    {
        void Initialize()
        {
            MicroProfileOnThreadCreate( "Main" );
            MicroProfileSetEnableAllGroups( true );
            MicroProfileSetForceMetaCounters( true );
        }

        void Shutdown()
        {
            MicroProfileShutdown();
        }

        void StartFrame()
        {
            PerformanceAPI::BeginEvent( "Frame" );
        }

        void EndFrame()
        {
            PerformanceAPI::EndEvent();
            MicroProfileFlip( 0 );
        }
    }
}