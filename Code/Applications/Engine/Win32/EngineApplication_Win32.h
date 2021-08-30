#ifdef _WIN32
#pragma once

#include <windows.h>
#include "Win32/Application_Win32.h"
#include "Applications/EngineShared/Engine.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EngineApplication : public Win32Application
    {

    public:

        EngineApplication( HINSTANCE hInstance );

    private:

        virtual bool ReadSettings( int32 argc, char** argv ) override;
        virtual bool Initialize();
        virtual bool Shutdown();

        virtual LRESULT WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

        virtual bool ApplicationLoop() override;

    private:

        Engine                          m_engine;
    };
}

#endif