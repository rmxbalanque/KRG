#include "Profiling.h"
#include "Superluminal/PerformanceAPI.h"
#include "System/Core/FileSystem/FileSystemPath.h"

#if _WIN32
#include "System/Core/Platform/Platform/Platform_Win32.h"
#include <shellapi.h>
#endif

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Profiling
    {
        void StartFrame()
        {
            PerformanceAPI::BeginEvent( "Frame" );
            OPTICK_FRAME( "KRG Main" );
        }

        void EndFrame()
        {
            PerformanceAPI::EndEvent();
        }

        void OpenProfiler()
        {
            #if _WIN32
            FileSystemPath const profilerPath = FileSystemPath( Platform::Win32::GetCurrentModulePath() ) + "..\\..\\..\\..\\External\\Optick\\Optick.exe";
            ShellExecute( 0, 0, profilerPath.c_str(), 0, 0, SW_SHOW );
            #endif
        }

        void StartCapture()
        {
            OPTICK_START_CAPTURE();
        }

        void StopCapture( FileSystemPath const& captureSavePath )
        {
            OPTICK_STOP_CAPTURE();
            OPTICK_SAVE_CAPTURE( captureSavePath.c_str() );
        }
    }
}