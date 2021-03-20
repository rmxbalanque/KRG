#include "ResourceServerApplication.h"
#include <tchar.h>

#if LIVEPP_ENABLED
#include "LPP_API.h"
#endif

//-------------------------------------------------------------------------

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    HANDLE pSingletonMutex = CreateMutex( NULL, TRUE, "Kruger Resource Server" );
    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        MessageBox( GetActiveWindow(), "Only allowed a single instance of the Kruger Resource Server", "Fatal Error Occurred!", MB_OK | MB_ICONERROR );
        return -1;
    }

    //-------------------------------------------------------------------------
    // Live++ Support
    //-------------------------------------------------------------------------

    #if LIVEPP_ENABLED
    /*HMODULE livePP = lpp::lppLoadAndRegister( L"../../External/LivePP", "Quickstart" );
    lpp::lppEnableAllCallingModulesSync( livePP );*/
    #endif

    //-------------------------------------------------------------------------

    KRG::ResourceServerApplication engineApplication( hInstance );
    int const result = engineApplication.Run( __argc, __argv );

    //-------------------------------------------------------------------------

    ReleaseMutex( pSingletonMutex );
    CloseHandle( pSingletonMutex );
    return result;
}
