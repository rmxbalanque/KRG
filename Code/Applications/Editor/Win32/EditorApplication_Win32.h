#pragma once

#ifdef _WIN32
#include "Win32/Application_Win32.h"
#include "Applications/Editor/Editor.h"
#include "Engine.h"
#include <windows.h>
#include "_AutoGenerated/ToolsTypeRegistration.h"

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
            KRG::Delete( m_pActiveEditor );
        }

        // Takes ownership of the active editor
        inline void SetActiveEditor( Editor* pActiveEditor )
        {
            KRG_ASSERT( pActiveEditor != nullptr );
            m_pDevelopmentTools = pActiveEditor;
            m_pActiveEditor = pActiveEditor;
        }

        virtual void RegisterTypes() override
        {
            AutoGenerated::Tools::RegisterTypes( *m_pTypeRegistry );
        }

        virtual void UnregisterTypes() override
        {
            AutoGenerated::Tools::UnregisterTypes( *m_pTypeRegistry );
        }

        // User flags
        inline void SetUserFlags( uint64 flags ) { m_pActiveEditor->SetUserFlags( flags ); }
        inline uint64 GetUserFlags() const { return m_pActiveEditor->GetUserFlags(); }

    private:

        Editor*     m_pActiveEditor = nullptr;
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
    };
}
#endif