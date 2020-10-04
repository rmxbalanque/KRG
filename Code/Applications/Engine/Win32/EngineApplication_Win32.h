#ifdef _WIN32
#pragma once

#include "../EngineApplication.h"
#include <windows.h>

//-------------------------------------------------------------------------

namespace KRG
{
    class EngineApplication_Win32 final : public EngineApplication
    {

    public:

        EngineApplication_Win32( HINSTANCE hInstance, char const* applicationName, int iconResourceID );
        ~EngineApplication_Win32();

        virtual bool StartDependencies() override final;
        virtual bool Initialize() override final;
        virtual int Run( S32 argc, char** argv ) override final;

        LRESULT WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

    private:

        virtual bool FatalError( KRG::String const& error ) override;

        void TryCreateWindow( HINSTANCE hInstance, char const* windowName, int iconResourceID );

        bool EnsureResourceServerIsRunning();

    private:

        WNDCLASSEX                      m_windowClass;
        HWND                            m_windowHandle = nullptr;
        RECT                            m_windowRect = { 0, 0, 640, 480 };
        MSG                             m_message;
    };
}

#endif