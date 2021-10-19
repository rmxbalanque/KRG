#pragma once

#include "ApplicationGlobalState.h"
#include "ResourceServer.h"
#include "ResourceServerUI.h"
#include "Engine/Render/Renderers/ImguiRenderer.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Render/RenderDevice.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Render/Imgui/ImguiSystem.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/IntegralTypes.h"
#include "System/Core/Update/UpdateContext.h"
#include "Win32/Application_Win32.h"
#include <shellapi.h>

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceServerApplication : public Win32Application
    {
        class InternalUpdateContext : public UpdateContext
        {
            friend ResourceServerApplication;
        };

    public:

        ResourceServerApplication( HINSTANCE hInstance );

    private:

        virtual bool ReadSettings( int32 argc, char** argv ) override;
        virtual bool Initialize() override;
        virtual bool Shutdown() override;
        virtual bool ApplicationLoop() override;
        virtual LRESULT WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) override;

        // System Tray
        //-------------------------------------------------------------------------

        void ShowApplicationWindow();
        void HideApplicationWindow();

        bool CreateSystemTrayIcon( int32 iconID );
        void DestroySystemTrayIcon();
        void RefreshSystemTrayIcon( int32 iconID );
        bool ShowSystemTrayMenu();

    private:

        NOTIFYICONDATA                          m_systemTrayIconData;
        int32                                   m_currentIconID = 0;
        bool                                    m_applicationWindowHidden = false;

        //-------------------------------------------------------------------------

        SettingsRegistry                        m_settingsRegistry;
        InternalUpdateContext                   m_updateContext;

        ImGuiX::ImguiSystem                     m_imguiSystem;

        // Rendering
        Render::RenderDevice*                   m_pRenderDevice = nullptr;
        Render::ViewportManager                 m_viewportManager;
        Render::ImguiRenderer                   m_imguiRenderer;

        // Resource
        Resource::Settings                      m_settings;
        Resource::ResourceServer                m_resourceServer;
        Resource::ResourceServerUI              m_resourceServerUI;
    };
}