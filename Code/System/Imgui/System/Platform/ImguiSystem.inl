#if _WIN32
#pragma once
#include <wtypes.h>

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    struct PlatformInputData
    {
        HWND    m_hWnd;
        UINT    m_message;
        WPARAM  m_wParam;
        LPARAM  m_lParam;
    };

    using PlatformInputResult = LRESULT;
}
#endif