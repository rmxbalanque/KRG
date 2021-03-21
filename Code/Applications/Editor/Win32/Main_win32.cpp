#ifdef _WIN32
#include "EditorApplication_Win32.h"
#include <tchar.h>
#include <windows.h>

#if LIVEPP_ENABLED
#include "LPP_API.h"
#endif

//-------------------------------------------------------------------------

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    //-------------------------------------------------------------------------
    // Live++ Support
    //-------------------------------------------------------------------------

    #if LIVEPP_ENABLED
    HMODULE livePP = lpp::lppLoadAndRegister( L"../../External/LivePP", "Quickstart" );
    lpp::lppEnableAllCallingModulesSync( livePP );
    #endif

    //-------------------------------------------------------------------------

    KRG::EditorApplication editorApplication( hInstance );
    return editorApplication.Run( __argc, __argv );
}

#endif