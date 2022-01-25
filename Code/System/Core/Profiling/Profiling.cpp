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

namespace KRG::Profiling
{
    void StartFrame()
    {
        #if KRG_ENABLE_SUPERLUMINAL
        PerformanceAPI::BeginEvent( "Frame" );
        #endif

        #if KRG_DEVELOPMENT_TOOLS
        OPTICK_FRAME( "KRG Main" );
        #endif
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
        #if KRG_DEVELOPMENT_TOOLS
        OPTICK_START_CAPTURE();
        #endif
    }

    void StopCapture( FileSystem::Path const& captureSavePath )
    {
        #if KRG_DEVELOPMENT_TOOLS
        OPTICK_STOP_CAPTURE();
        OPTICK_SAVE_CAPTURE( captureSavePath.c_str() );
        #endif
    }
}