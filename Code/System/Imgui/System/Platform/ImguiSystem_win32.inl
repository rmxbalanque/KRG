#if _WIN32
#pragma once

#include "Windows.h"
#include "../ImguiSystemBase.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ImGuiX
    {
        class KRG_SYSTEM_IMGUI_API System final : public SystemBase
        {
        public:

            LRESULT ImguiWndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

            bool Initialize();
            using SystemBase::Shutdown;

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
}
#endif