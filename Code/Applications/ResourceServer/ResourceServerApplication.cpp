#include "ResourceServerApplication.h"
#include "Resources/Resource.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Time/Timers.h"

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
        if ( !IsInitialized() )
        {
            return DefWindowProc( hWnd, message, wParam, lParam );
        }

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

        //-------------------------------------------------------------------------

        switch ( message )
        {
            case WM_SIZE:
            {
                KRG::uint32 width = LOWORD( lParam );
                KRG::uint32 height = HIWORD( lParam );
                if ( width > 0 && height > 0 )
                {
                    Int2 const newWindowDimensions( width, height );
                    m_pRenderDevice->ResizeRenderTargets( newWindowDimensions );
                    m_viewport = Math::Viewport( Int2( 0, 0 ), newWindowDimensions );
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
                return 0;
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

        // ImGui specific message processing
        //-------------------------------------------------------------------------

        auto const imguiResult = m_imguiSystem.ProcessInput( { hWnd, message, wParam, lParam } );
        if ( imguiResult != 0 )
        {
            return imguiResult;
        }

        // Default
        //-------------------------------------------------------------------------

        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    //-------------------------------------------------------------------------

    void ResourceServerApplication::ShowApplicationWindow()
    {
        ShowWindow( m_pWindow, SW_SHOW );
        SetForegroundWindow( m_pWindow );
        m_applicationWindowHidden = false;
    }

    void ResourceServerApplication::HideApplicationWindow()
    {
        ShowWindow( m_pWindow, SW_HIDE );
        m_applicationWindowHidden = true;
    }

    bool ResourceServerApplication::CreateSystemTrayIcon( int32 iconID )
    {
        m_systemTrayIconData.cbSize = sizeof( NOTIFYICONDATA );
        m_systemTrayIconData.hWnd = m_pWindow;
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
        SetForegroundWindow( m_pWindow );
        TrackPopupMenu( hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, m_pWindow, NULL );
        SendMessage( m_pWindow, WM_NULL, 0, 0 );

        // Kill off objects we're done with
        DestroyMenu( hMenu );

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
        if ( !m_pRenderDevice->Initialize() )
        {
            KRG::Delete( m_pRenderDevice );
            return FatalError( "Failed to create render device!" );
        }

        Int2 const windowDimensions( ( m_windowRect.right - m_windowRect.left ), ( m_windowRect.bottom - m_windowRect.top ) );
        m_pRenderDevice->ResizeRenderTargets( windowDimensions );
        m_viewport = Math::Viewport( Int2( 0, 0 ), windowDimensions );

        //-------------------------------------------------------------------------

        if ( !m_resourceServer.Initialize() )
        {
            return FatalError( "Resource server failed to initialize!" );
        }

        //-------------------------------------------------------------------------

        m_imguiSystem.Initialize( m_applicationNameNoWhitespace + ".imgui.ini" );
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
                m_imguiRenderer.Render( m_viewport );
                m_pRenderDevice->PresentFrame();
            }
        }

        m_updateContext.UpdateDeltaTime( deltaTime );
        EngineClock::Update( deltaTime );

        return true;
    }
}