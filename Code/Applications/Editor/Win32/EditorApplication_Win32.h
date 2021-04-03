#pragma once

#ifdef _WIN32
#include "Win32/Application_Win32.h"
#include "Applications/Editor/Editor.h"
#include "Engine.h"
#include <windows.h>

//-------------------------------------------------------------------------

namespace KRG
{
    class EditorHost : public Engine
    {
        friend class EditorApplication;

    public:

        using Engine::Engine;

        // Deletes active editor
        virtual ~EditorHost()
        {
            KRG_ASSERT( m_pDevelopmentTools != &m_debugTools );
            KRG::Delete( m_pDevelopmentTools );
        }

        // Takes ownership of the active editor
        inline void SetActiveEditor( Editor* pActiveEditor )
        {
            KRG_ASSERT( pActiveEditor != nullptr );
            m_pDevelopmentTools = pActiveEditor;
        }
    };

    //-------------------------------------------------------------------------

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

        EditorHost                      m_editorHost;
        String                          m_editorModeID;
    };
}
#endif