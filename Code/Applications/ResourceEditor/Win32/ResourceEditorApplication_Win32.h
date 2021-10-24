#pragma once

#include "Applications/ResourceEditor/ResourceEditor.h"
#include "Engine.h"
#include "Win32/Application_Win32.h"

#include "_AutoGenerated/ToolsTypeRegistration.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceEditorEngine final : public Engine
    {
        friend class ResourceEditorApplication;

    public:

        using Engine::Engine;

        virtual void RegisterTypes() override
        {
            AutoGenerated::Tools::RegisterTypes( *m_pTypeRegistry );
        }

        virtual void UnregisterTypes() override
        {
            AutoGenerated::Tools::UnregisterTypes( *m_pTypeRegistry );
        }

        #if KRG_DEVELOPMENT_TOOLS
        virtual void CreateDevelopmentUI() { m_pDevelopmentUI = KRG::New<ResourceEditor>(); }
        #endif
    };

    //-------------------------------------------------------------------------

    class ResourceEditorApplication final : public Win32Application
    {

    public:

        ResourceEditorApplication( HINSTANCE hInstance );

    private:

        virtual bool ReadSettings( int32 argc, char** argv ) override;
        virtual bool Initialize() override;
        virtual bool Shutdown() override;

        virtual LRESULT WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

        virtual bool ApplicationLoop() override;

    private:

        ResourceEditorEngine                    m_editorEngine;
    };
}