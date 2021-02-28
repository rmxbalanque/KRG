#if _WIN32
#pragma once

#include "Windows.h"
#include "../ImguiSystemBase.h"

//-------------------------------------------------------------------------
// Windows ImGui platform integration
//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    class KRG_SYSTEM_IMGUI_API ImguiSystem final : public ImguiSystemBase
    {
    public:

        LRESULT ImguiWndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

        bool Initialize();
        using ImguiSystemBase::Shutdown;

    private:

        virtual void UpdateDisplayInformation() override final;
        virtual void UpdateKeyStates() override final;
        virtual void UpdateMousePosition() override final;
        virtual bool UpdateMouseCursor() override final;
        virtual void InitializePlatform() override final;

    private:

        HWND m_mainwindowHandle = nullptr;
    };
}
#endif