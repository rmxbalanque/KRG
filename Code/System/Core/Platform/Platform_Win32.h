#pragma once
#ifdef _WIN32

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// Platform Specific Helpers/Functions
//-------------------------------------------------------------------------

namespace KRG::Platform::Win32
{
    // File system
    //-------------------------------------------------------------------------

    KRG_SYSTEM_CORE_API String GetShortPath( String const& origPath );

    // Processes
    //-------------------------------------------------------------------------

    KRG_SYSTEM_CORE_API uint32 GetProcessID( char const* processName );
    KRG_SYSTEM_CORE_API String GetProcessPath( uint32 processID );
    KRG_SYSTEM_CORE_API String GetCurrentModulePath();
    KRG_SYSTEM_CORE_API String GetLastErrorMessage();

    // Try to start a window process and returns the process ID
    KRG_SYSTEM_CORE_API uint32 StartProcess( char const* exePath, char const* cmdLine = nullptr );

    // Kill a running process
    KRG_SYSTEM_CORE_API bool KillProcess( uint32 processID );

    inline bool IsProcessRunning( char const* processName, uint32* pProcessID ) { return GetProcessID( processName ) != 0; }
}

#endif
