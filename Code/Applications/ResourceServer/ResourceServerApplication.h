#pragma once

#include "ApplicationGlobalState.h"
#include "ResourceServer.h"
#include "ResourceServerUI.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Render/Renderers/RendererRegistry.h"
#include "System/DevTools/System/ImguiSystem.h"
#include "System/DevTools/Renderer/ImguiRenderer.h"
#include "System/Core/Types/String.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Math/Viewport.h"
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

        // Rendering
        Render::RenderDevice*                   m_pRenderDevice = nullptr;

        // Imgui
        ImGuiX::ImguiSystem                     m_imguiSystem;
        ImGuiX::ImguiRenderer                   m_imguiRenderer;
        Math::Viewport                          m_viewport;

        Resource::ResourceServer                m_resourceServer;
        Resource::ResourceServerUI              m_resourceServerUI;
    };
}