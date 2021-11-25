#include "ResourceServerApplication.h"
#include "Resources/Resource.h"
#include "System/Render/RenderSettings.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Time/Timers.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Render/Imgui/Platform/ImguiPlatform_win32.h"
#include <tchar.h>

#if LIVEPP_ENABLED
#include "LPP_API.h"
#endif

//-------------------------------------------------------------------------

namespace KRG
{
    constexpr static int32 const g_shellIconCallbackMessageID = WM_USER + 1;

    //-------------------------------------------------------------------------

    ResourceServerApplication::ResourceServerApplication( HINSTANCE pInstance )
        : Win32Application( pInstance, "Kruger Resource Server", IDI_RESOURCESERVER )
        , m_resourceServerUI( &m_resourceServer )
    {}

    LRESULT ResourceServerApplication::WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
    {
        // SysTray
        //-------------------------------------------------------------------------

        if ( message == RegisterWindowMessage( "TaskbarCreated" ) )
        {
            if ( !Shell_NotifyIcon( NIM_ADD, &m_systemTrayIconData ) )
            {
                FatalError( "Failed to recreate system tray icon after explorer crash!" );
                RequestExit();
                return -1;
            }
        }

        // ImGui specific message processing
        //-------------------------------------------------------------------------

        auto const imguiResult = ImGuiX::Platform::WindowsMessageHandler( hWnd, message, wParam, lParam );
        if ( imguiResult != 0 )
        {
            return imguiResult;
        }

        // General
        //-------------------------------------------------------------------------

        switch ( message )
        {
            case WM_SIZE:
            {
                Int2 const newDimensions( LOWORD( lParam ), HIWORD( lParam ) );
                if ( newDimensions.m_x > 0 && newDimensions.m_y > 0 )
                {
                    m_pRenderDevice->ResizePrimaryWindowRenderTarget( newDimensions );
                    m_viewport.Resize( Int2::Zero, newDimensions );

                    // Hack to fix client area offset bug
                    RECT rect;
                    GetWindowRect( hWnd, &rect );
                    MoveWindow( hWnd, rect.left + 1, rect.top, rect.right - rect.left, rect.bottom - rect.top, FALSE );
                }
            }
            break;

            //-------------------------------------------------------------------------

            case g_shellIconCallbackMessageID:
            {
                switch ( LOWORD( lParam ) )
                {
                    case WM_LBUTTONDBLCLK:
                    {
                        ShowApplicationWindow();
                    }
                    break;

                    case WM_RBUTTONDOWN:
                    {
                        if ( !ShowSystemTrayMenu() )
                        {
                            return -1;
                        }
                    }
                    break;
                }
            }
            break;

            //-------------------------------------------------------------------------

            case WM_COMMAND:
            {
                switch ( LOWORD( wParam ) )
                {
                    case ID_REQUEST_EXIT:
                    {
                        RequestExit();
                    }
                    break;
                }
            }
            break;

            //-------------------------------------------------------------------------

            case WM_CLOSE:
            {
                HideApplicationWindow();
                return -1;
            }
            break;

            case WM_QUIT:
            {
                RequestExit();
            }
            break;

            case WM_DESTROY:
            {
                PostQuitMessage( 0 );
            }
            break;
        }

        return 0;
    }

    //-------------------------------------------------------------------------

    void ResourceServerApplication::ShowApplicationWindow()
    {
        ShowWindow( m_windowHandle, SW_SHOW );
        SetForegroundWindow( m_windowHandle );
        m_applicationWindowHidden = false;
    }

    void ResourceServerApplication::HideApplicationWindow()
    {
        ShowWindow( m_windowHandle, SW_HIDE );
        m_applicationWindowHidden = true;
    }

    bool ResourceServerApplication::CreateSystemTrayIcon( int32 iconID )
    {
        m_systemTrayIconData.cbSize = sizeof( NOTIFYICONDATA );
        m_systemTrayIconData.hWnd = m_windowHandle;
        m_systemTrayIconData.uID = IDI_TRAY_IDLE;
        m_systemTrayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        m_systemTrayIconData.hIcon = LoadIcon( m_pInstance, (LPCTSTR) MAKEINTRESOURCE( iconID ) );
        m_systemTrayIconData.uCallbackMessage = g_shellIconCallbackMessageID;
        strcpy( m_systemTrayIconData.szTip, "Kruger Resource Server" );

        if ( !Shell_NotifyIcon( NIM_ADD, &m_systemTrayIconData ) )
        {
            return false;
        }

        m_currentIconID = iconID;
        return true;
    }

    bool ResourceServerApplication::ShowSystemTrayMenu()
    {
        HMENU hMenu, hSubMenu;

        // Get mouse cursor position x and y as lParam has the Message itself
        POINT lpClickPoint;
        GetCursorPos( &lpClickPoint );

        // Load menu resource
        hMenu = LoadMenu( m_pInstance, MAKEINTRESOURCE( IDR_SYSTRAY_MENU ) );
        if ( !hMenu )
        {
            return false;
        }

        hSubMenu = GetSubMenu( hMenu, 0 );
        if ( !hSubMenu )
        {
            DestroyMenu( hMenu );
            return false;
        }

        // Display menu
        SetForegroundWindow( m_windowHandle );
        TrackPopupMenu( hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, m_windowHandle, NULL );
        SendMessage( m_windowHandle, WM_NULL, 0, 0 );

        // Kill off objects we're done with
        DestroyMenu( hMenu );

        return true;
    }

    bool ResourceServerApplication::ReadSettings( int32 argc, char** argv )
    {
        // Read configuration settings from ini
        //-------------------------------------------------------------------------

        m_settingsRegistry.RegisterSettings( &m_settings );

        FileSystem::Path const iniPath = FileSystem::GetCurrentProcessPath().Append( "KRG.ini" );
        if ( !m_settingsRegistry.LoadFromFile( iniPath ) )
        {
            return FatalError( "Failed to read required settings from INI file" );
        }

        return true;
    }

    void ResourceServerApplication::DestroySystemTrayIcon()
    {
        Shell_NotifyIcon( NIM_DELETE, &m_systemTrayIconData );
    }

    void ResourceServerApplication::RefreshSystemTrayIcon( int32 iconID )
    {
        if ( iconID != m_currentIconID )
        {
            DestroySystemTrayIcon();
            CreateSystemTrayIcon( iconID );
        }
    }

    //-------------------------------------------------------------------------

    bool ResourceServerApplication::Initialize()
    {
        m_pRenderDevice = KRG::New<Render::RenderDevice>();
        if ( !m_pRenderDevice->Initialize( Render::Settings() ) )
        {
            KRG::Delete( m_pRenderDevice );
            return FatalError( "Failed to create render device!" );
        }

        Int2 const windowDimensions( ( m_windowRect.right - m_windowRect.left ), ( m_windowRect.bottom - m_windowRect.top ) );
        m_pRenderDevice->ResizePrimaryWindowRenderTarget( windowDimensions );
        m_viewport = Render::Viewport( Int2( 0, 0 ), windowDimensions, Math::ViewVolume( Float2( windowDimensions ), FloatRange( 0.1f, 100.0f ) ) );

        //-------------------------------------------------------------------------

        if ( !m_resourceServer.Initialize( m_settings ) )
        {
            return FatalError( "Resource server failed to initialize!" );
        }

        //-------------------------------------------------------------------------

        m_imguiSystem.Initialize( m_applicationNameNoWhitespace + ".imgui.ini", m_pRenderDevice );
        m_imguiRenderer.Initialize( m_pRenderDevice );

        //-------------------------------------------------------------------------

        CreateSystemTrayIcon( IDI_TRAY_IDLE );
        HideApplicationWindow();
        return true;
    }

    bool ResourceServerApplication::Shutdown()
    {
        if ( IsInitialized() )
        {
            m_imguiRenderer.Shutdown();
            m_imguiSystem.Shutdown();
        }

        //-------------------------------------------------------------------------

        m_resourceServer.Shutdown();

        if ( m_pRenderDevice != nullptr )
        {
            m_pRenderDevice->Shutdown();
            KRG::Delete( m_pRenderDevice );
        }

        //-------------------------------------------------------------------------

        DestroySystemTrayIcon();

        return true;
    }

    //-------------------------------------------------------------------------

    bool ResourceServerApplication::ApplicationLoop()
    {
        Milliseconds deltaTime = 0;
        {
            ScopedSystemTimer frameTimer( deltaTime );

            // Update resource server
            //-------------------------------------------------------------------------

            m_resourceServer.Update();

            if ( m_resourceServer.IsBusy() )
            {
                RefreshSystemTrayIcon( IDI_TRAY_BUSY );
            }
            else // Wait
            {
                RefreshSystemTrayIcon( IDI_TRAY_IDLE );
                Threading::Sleep( 1 );
            }

            // Draw UI
            //-------------------------------------------------------------------------

            if ( !m_applicationWindowHidden )
            {
                m_imguiSystem.StartFrame( m_updateContext.GetDeltaTime() );
              
                m_resourceServerUI.Draw();

                m_imguiSystem.EndFrame();
                m_imguiRenderer.RenderViewport( m_pRenderDevice->GetPrimaryWindowRenderTarget(), m_viewport );
                m_pRenderDevice->PresentFrame();
            }
        }

        m_updateContext.UpdateDeltaTime( deltaTime );
        EngineClock::Update( deltaTime );

        return true;
    }
}

//-------------------------------------------------------------------------

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    HANDLE pSingletonMutex = CreateMutex( NULL, TRUE, "Kruger Resource Server" );
    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        MessageBox( GetActiveWindow(), "Only allowed a single instance of the Kruger Resource Server", "Fatal Error Occurred!", MB_OK | MB_ICONERROR );
        return -1;
    }

    //-------------------------------------------------------------------------
    // Live++ Support
    //-------------------------------------------------------------------------

    #if LIVEPP_ENABLED
    /*HMODULE livePP = lpp::lppLoadAndRegister( L"../../External/LivePP", "Quickstart" );
    lpp::lppEnableAllCallingModulesSync( livePP );*/
    #endif

    //-------------------------------------------------------------------------
    
    KRG::ApplicationGlobalState globalState;
    KRG::ResourceServerApplication engineApplication( hInstance );
    int const result = engineApplication.Run( __argc, __argv );

    //-------------------------------------------------------------------------

    ReleaseMutex( pSingletonMutex );
    CloseHandle( pSingletonMutex );
    return result;
}