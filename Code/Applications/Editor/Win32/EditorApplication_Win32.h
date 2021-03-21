#ifdef _WIN32
#pragma once

#include <windows.h>
#include "Win32/Application_Win32.h"
#include "../Editor.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EditorApplication : public Win32Application
    {

    public:

        EditorApplication( HINSTANCE hInstance );

    private:

        virtual bool ReadSettings( int32 argc, char** argv ) override;
        virtual bool Initialize();
        virtual bool Shutdown();

        virtual LRESULT WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

        bool EnsureResourceServerIsRunning();
        virtual bool ApplicationLoop() override;

    private:

        Editor                          m_editor;
    };
}

#endif