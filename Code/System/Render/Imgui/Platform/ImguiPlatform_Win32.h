#if _WIN32
#pragma once

#include "../../_Module/API.h"
#include <windows.h>

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::Platform
{
    void InitializePlatform();
    void ShutdownPlatform();

    // Called each frame
    void UpdateDisplayInformation();
    void UpdateMouseInformation();

    // Returns 0 when the message isnt handled, used to embed into another wnd proc
    KRG_SYSTEM_RENDER_API LRESULT WindowsMessageHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
}
#endif