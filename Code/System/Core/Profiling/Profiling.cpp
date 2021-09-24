#include "Profiling.h"
#include "System/Core/FileSystem/FileSystemPath.h"

#if KRG_ENABLE_SUPERLUMINAL
#include "Superluminal/PerformanceAPI.h"
#endif

#if _WIN32
#include "System/Core/Platform/PlatformHelpers_Win32.h"
#include <shellapi.h>
#endif

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Profiling
    {
        void StartFrame()
        {
            #if KRG_ENABLE_SUPERLUMINAL
            PerformanceAPI::BeginEvent( "Frame" );
            #endif

            OPTICK_FRAME( "KRG Main" );
        }

        void EndFrame()
        {
            #if KRG_ENABLE_SUPERLUMINAL
            PerformanceAPI::EndEvent();
            #endif
        }

        void OpenProfiler()
        {
            #if _WIN32
            FileSystem::Path const profilerPath = FileSystem::Path( Platform::Win32::GetCurrentModulePath() ) + "..\\..\\..\\..\\External\\Optick\\Optick.exe";
            ShellExecute( 0, 0, profilerPath.c_str(), 0, 0, SW_SHOW );
            #endif
        }

        void StartCapture()
        {
            OPTICK_START_CAPTURE();
        }

        void StopCapture( FileSystem::Path const& captureSavePath )
        {
            OPTICK_STOP_CAPTURE();
            OPTICK_SAVE_CAPTURE( captureSavePath.c_str() );
        }
    }
}