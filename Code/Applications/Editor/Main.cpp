#include "EditorApplication.h"
#include "Applications/Shared/ApplicationGlobalState.h"

#if LIVEPP_ENABLED
#include "LPP_API.h"
#endif

//-------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    KRG::ApplicationGlobalState State;

    //-------------------------------------------------------------------------
    // Live++ Support
    //-------------------------------------------------------------------------

    #if LIVEPP_ENABLED
    HMODULE livePP = lpp::lppLoadAndRegister( L"../../External/LivePP", "Quickstart" );
    lpp::lppEnableAllCallingModulesSync( livePP );
    #endif

    //-------------------------------------------------------------------------
    // Resource Server
    //-------------------------------------------------------------------------

    KRG::Editor::EditorApplication app( argc, argv );
    return app.Run();
}
