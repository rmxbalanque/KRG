#if _WIN32
#include "ImguiPlatform_Win32.h"
#include "imgui.h"
#include "System/Core/Platform/PlatformHelpers_Win32.h"
#include <tchar.h>

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::Platform
{
    //-------------------------------------------------------------------------
    // This code is taken from the imgui docking branch examples
    //-------------------------------------------------------------------------

    struct ImGuiViewportDataWin32
    {
        HWND    Hwnd;
        bool    HwndOwned;
        DWORD   DwStyle;
        DWORD   DwExStyle;

        ImGuiViewportDataWin32() { Hwnd = NULL; HwndOwned = false;  DwStyle = DwExStyle = 0; }
        ~ImGuiViewportDataWin32() { KRG_ASSERT( Hwnd == NULL ); }
    };

    struct ImGuiBackendDataWin32
    {
        HWND                        hWnd;
        HWND                        MouseHwnd;
        bool                        MouseTracked = false;
        WNDCLASSEX                  m_imguiWindowClass;
        ImGuiMouseCursor            LastMouseCursor;
        bool                        WantUpdateHasGamepad = false;
        bool                        WantUpdateMonitors = false;
    };

    static BOOL CALLBACK ImGui_ImplWin32_UpdateMonitors_EnumFunc( HMONITOR monitor, HDC, LPRECT, LPARAM )
    {
        MONITORINFO info = { 0 };
        info.cbSize = sizeof( MONITORINFO );
        if ( !::GetMonitorInfo( monitor, &info ) )
        {
            return TRUE;
        }

        ImGuiPlatformMonitor imgui_monitor;
        imgui_monitor.MainPos = ImVec2( (float) info.rcMonitor.left, (float) info.rcMonitor.top );
        imgui_monitor.MainSize = ImVec2( (float) ( info.rcMonitor.right - info.rcMonitor.left ), (float) ( info.rcMonitor.bottom - info.rcMonitor.top ) );
        imgui_monitor.WorkPos = ImVec2( (float) info.rcWork.left, (float) info.rcWork.top );
        imgui_monitor.WorkSize = ImVec2( (float) ( info.rcWork.right - info.rcWork.left ), (float) ( info.rcWork.bottom - info.rcWork.top ) );
        imgui_monitor.DpiScale = 1.0f;

        ImGuiPlatformIO& io = ImGui::GetPlatformIO();
        if ( info.dwFlags & MONITORINFOF_PRIMARY )
        {
            io.Monitors.push_front( imgui_monitor );
        }
        else
        {
            io.Monitors.push_back( imgui_monitor );
        }
        return TRUE;
    }

    static void ImGui_ImplWin32_GetWin32StyleFromViewportFlags( ImGuiViewportFlags flags, DWORD* out_style, DWORD* out_ex_style )
    {
        if ( flags & ImGuiViewportFlags_NoDecoration )
            *out_style = WS_POPUP;
        else
            *out_style = WS_OVERLAPPEDWINDOW;

        if ( flags & ImGuiViewportFlags_NoTaskBarIcon )
            *out_ex_style = WS_EX_TOOLWINDOW;
        else
            *out_ex_style = WS_EX_APPWINDOW;

        if ( flags & ImGuiViewportFlags_TopMost )
            *out_ex_style |= WS_EX_TOPMOST;
    }

    static void ImGui_ImplWin32_CreateWindow( ImGuiViewport* viewport )
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendDataWin32* pBackendData = reinterpret_cast<ImGuiBackendDataWin32*>( io.BackendPlatformUserData );

        //-------------------------------------------------------------------------

        ImGuiViewportDataWin32* data = KRG::New<ImGuiViewportDataWin32>();
        viewport->PlatformUserData = data;

        // Select style and parent window
        ImGui_ImplWin32_GetWin32StyleFromViewportFlags( viewport->Flags, &data->DwStyle, &data->DwExStyle );
        HWND parentWindow = NULL;
        if ( viewport->ParentViewportId != 0 )
        {
            if ( ImGuiViewport* parent_viewport = ImGui::FindViewportByID( viewport->ParentViewportId ) )
            {
                parentWindow = (HWND) parent_viewport->PlatformHandle;
            }
        }

        // Create window
        RECT rect = { (LONG) viewport->Pos.x, (LONG) viewport->Pos.y, (LONG) ( viewport->Pos.x + viewport->Size.x ), (LONG) ( viewport->Pos.y + viewport->Size.y ) };
        ::AdjustWindowRectEx( &rect, data->DwStyle, FALSE, data->DwExStyle );
        
        data->Hwnd = ::CreateWindowEx
        (
            data->DwExStyle, 
            pBackendData->m_imguiWindowClass.lpszClassName,
            pBackendData->m_imguiWindowClass.lpszClassName,
            data->DwStyle,
            rect.left, 
            rect.top, 
            rect.right - rect.left,
            rect.bottom - rect.top, 
            parentWindow, 
            NULL, 
            ::GetModuleHandle( NULL ), 
            NULL 
        );

        //auto const errorMessage = KRG::Platform::Win32::GetLastErrorMessage();
        KRG_ASSERT( data->Hwnd != 0 );

        //-------------------------------------------------------------------------

        data->HwndOwned = true;
        viewport->PlatformRequestResize = false;
        viewport->PlatformHandle = viewport->PlatformHandleRaw = data->Hwnd;
    }

    static void ImGui_ImplWin32_DestroyWindow( ImGuiViewport* viewport )
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendDataWin32* pBackendData = reinterpret_cast<ImGuiBackendDataWin32*>( io.BackendPlatformUserData );

        //-------------------------------------------------------------------------

        if ( ImGuiViewportDataWin32* pData = (ImGuiViewportDataWin32*) viewport->PlatformUserData )
        {
            if ( ::GetCapture() == pData->Hwnd )
            {
                // Transfer capture so if we started dragging from a window that later disappears, we'll still receive the MOUSEUP event.
                ::ReleaseCapture();
                ::SetCapture( pBackendData->hWnd );
            }

            if ( pData->Hwnd && pData->HwndOwned )
            {
                ::DestroyWindow( pData->Hwnd );
            }

            pData->Hwnd = NULL;
            KRG::Delete( pData );
        }
        viewport->PlatformUserData = viewport->PlatformHandle = NULL;
    }

    static void ImGui_ImplWin32_ShowWindow( ImGuiViewport* viewport )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        if ( viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing )
            ::ShowWindow( data->Hwnd, SW_SHOWNA );
        else
            ::ShowWindow( data->Hwnd, SW_SHOW );
    }

    static void ImGui_ImplWin32_UpdateWindow( ImGuiViewport* viewport )
    {
        // (Optional) Update Win32 style if it changed _after_ creation.
        // Generally they won't change unless configuration flags are changed, but advanced uses (such as manually rewriting viewport flags) make this useful.
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        DWORD new_style;
        DWORD new_ex_style;
        ImGui_ImplWin32_GetWin32StyleFromViewportFlags( viewport->Flags, &new_style, &new_ex_style );

        // Only reapply the flags that have been changed from our point of view (as other flags are being modified by Windows)
        if ( data->DwStyle != new_style || data->DwExStyle != new_ex_style )
        {
            data->DwStyle = new_style;
            data->DwExStyle = new_ex_style;
            ::SetWindowLong( data->Hwnd, GWL_STYLE, data->DwStyle );
            ::SetWindowLong( data->Hwnd, GWL_EXSTYLE, data->DwExStyle );
            RECT rect = { (LONG) viewport->Pos.x, (LONG) viewport->Pos.y, (LONG) ( viewport->Pos.x + viewport->Size.x ), (LONG) ( viewport->Pos.y + viewport->Size.y ) };
            ::AdjustWindowRectEx( &rect, data->DwStyle, FALSE, data->DwExStyle ); // Client to Screen
            ::SetWindowPos( data->Hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );
            ::ShowWindow( data->Hwnd, SW_SHOWNA ); // This is necessary when we alter the style
            viewport->PlatformRequestMove = viewport->PlatformRequestResize = true;
        }
    }

    static ImVec2 ImGui_ImplWin32_GetWindowPos( ImGuiViewport* viewport )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        POINT pos = { 0, 0 };
        ::ClientToScreen( data->Hwnd, &pos );
        return ImVec2( (float) pos.x, (float) pos.y );
    }

    static void ImGui_ImplWin32_SetWindowPos( ImGuiViewport* viewport, ImVec2 pos )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        RECT rect = { (LONG) pos.x, (LONG) pos.y, (LONG) pos.x, (LONG) pos.y };
        ::AdjustWindowRectEx( &rect, data->DwStyle, FALSE, data->DwExStyle );
        ::SetWindowPos( data->Hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
    }

    static ImVec2 ImGui_ImplWin32_GetWindowSize( ImGuiViewport* viewport )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        RECT rect;
        ::GetClientRect( data->Hwnd, &rect );
        return ImVec2( float( rect.right - rect.left ), float( rect.bottom - rect.top ) );
    }

    static void ImGui_ImplWin32_SetWindowSize( ImGuiViewport* viewport, ImVec2 size )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        RECT rect = { 0, 0, (LONG) size.x, (LONG) size.y };
        ::AdjustWindowRectEx( &rect, data->DwStyle, FALSE, data->DwExStyle ); // Client to Screen
        ::SetWindowPos( data->Hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
    }

    static void ImGui_ImplWin32_SetWindowFocus( ImGuiViewport* viewport )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        ::BringWindowToTop( data->Hwnd );
        ::SetForegroundWindow( data->Hwnd );
        ::SetFocus( data->Hwnd );
    }

    static bool ImGui_ImplWin32_GetWindowFocus( ImGuiViewport* viewport )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        return ::GetForegroundWindow() == data->Hwnd;
    }

    static bool ImGui_ImplWin32_GetWindowMinimized( ImGuiViewport* viewport )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        return ::IsIconic( data->Hwnd ) != 0;
    }

    static void ImGui_ImplWin32_SetWindowTitle( ImGuiViewport* viewport, const char* title )
    {
        // ::SetWindowTextA() doesn't properly handle UTF-8 so we explicitely convert our string.
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        int n = ::MultiByteToWideChar( CP_UTF8, 0, title, -1, NULL, 0 );
        ImVector<wchar_t> title_w;
        title_w.resize( n );
        ::MultiByteToWideChar( CP_UTF8, 0, title, -1, title_w.Data, n );
        ::SetWindowTextW( data->Hwnd, title_w.Data );
    }

    static void ImGui_ImplWin32_SetWindowAlpha( ImGuiViewport* viewport, float alpha )
    {
        ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*) viewport->PlatformUserData;
        KRG_ASSERT( data->Hwnd != 0 );
        KRG_ASSERT( alpha >= 0.0f && alpha <= 1.0f );
        if ( alpha < 1.0f )
        {
            DWORD style = ::GetWindowLongW( data->Hwnd, GWL_EXSTYLE ) | WS_EX_LAYERED;
            ::SetWindowLongW( data->Hwnd, GWL_EXSTYLE, style );
            ::SetLayeredWindowAttributes( data->Hwnd, 0, (BYTE) ( 255 * alpha ), LWA_ALPHA );
        }
        else
        {
            DWORD style = ::GetWindowLongW( data->Hwnd, GWL_EXSTYLE ) & ~WS_EX_LAYERED;
            ::SetWindowLongW( data->Hwnd, GWL_EXSTYLE, style );
        }
    }

    static bool ImGui_ImplWin32_UpdateMouseCursor()
    {
        ImGuiIO& io = ImGui::GetIO();

        if ( io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange )
        {
            return false;
        }

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if ( imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor )
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            ::SetCursor( NULL );
        }
        else
        {
            // Show OS mouse cursor
            LPTSTR win32_cursor = IDC_ARROW;
            switch ( imgui_cursor )
            {
                case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
                case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
                case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
                case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
                case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
                case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
                case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
                case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
                case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
            }
            ::SetCursor( ::LoadCursor( NULL, win32_cursor ) );
        }

        return true;
    }

    //-------------------------------------------------------------------------

    // Returns 0 when the message isnt handled, used to embed into another wnd proc
    static LRESULT ImGui_ImplWin32_WndProcHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendDataWin32* pBackendData = reinterpret_cast<ImGuiBackendDataWin32*>( io.BackendPlatformUserData );

        switch ( msg )
        {
            case WM_MOUSEMOVE:
            {
                // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
                pBackendData->MouseHwnd = hwnd;
                if ( !pBackendData->MouseTracked )
                {
                    TRACKMOUSEEVENT tme = { sizeof( tme ), TME_LEAVE, hwnd, 0 };
                    ::TrackMouseEvent( &tme );
                    pBackendData->MouseTracked = true;
                }
            }
            break;

            case WM_MOUSELEAVE:
            {
                if ( pBackendData->MouseHwnd == hwnd )
                {
                    pBackendData->MouseHwnd = NULL;
                }
                pBackendData->MouseTracked = false;
            }
            break;

            case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
            case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
            {
                int button = 0;
                if ( msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK ) { button = 0; }
                if ( msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK ) { button = 1; }
                if ( msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK ) { button = 2; }
                if ( msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK ) { button = ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ) ? 3 : 4; }

                if ( !ImGui::IsAnyMouseDown() && ::GetCapture() == NULL )
                {
                    ::SetCapture( hwnd );
                }
                io.MouseDown[button] = true;
            }
            break;

            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
            {
                int button = 0;
                if ( msg == WM_LBUTTONUP ) { button = 0; }
                if ( msg == WM_RBUTTONUP ) { button = 1; }
                if ( msg == WM_MBUTTONUP ) { button = 2; }
                if ( msg == WM_XBUTTONUP ) { button = ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ) ? 3 : 4; }

                io.MouseDown[button] = false;
                if ( !ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd )
                {
                    ::ReleaseCapture();
                }
            }
            break;

            case WM_MOUSEWHEEL:
            {
                io.MouseWheel += (float) GET_WHEEL_DELTA_WPARAM( wParam ) / (float) WHEEL_DELTA;
            }
            break;

            case WM_MOUSEHWHEEL:
            {
                io.MouseWheelH += (float) GET_WHEEL_DELTA_WPARAM( wParam ) / (float) WHEEL_DELTA;
            }
            break;

            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            {
                bool const isKeyDown = ( msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN );
                if ( wParam < 256 )
                {
                    io.KeysDown[wParam] = isKeyDown;
                }
                if ( wParam == VK_CONTROL )
                {
                    io.KeyCtrl = isKeyDown;
                }

                if ( wParam == VK_SHIFT )
                {
                    io.KeyShift = isKeyDown;
                }

                if ( wParam == VK_MENU )
                {
                    io.KeyAlt = isKeyDown;
                }
            }
            break;

            case WM_SETFOCUS:
            case WM_KILLFOCUS:
            {
                io.AddFocusEvent( msg == WM_SETFOCUS );
            }
            break;

            case WM_CHAR:
            {
                // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
                if ( wParam > 0 && wParam < 0x10000 )
                {
                    io.AddInputCharacterUTF16( (unsigned short) wParam );
                }
            }
            break;

            case WM_SETCURSOR:
            {
                if ( LOWORD( lParam ) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor() )
                {
                    return 1;
                }
            }
            break;

            case WM_DISPLAYCHANGE:
            {
                pBackendData->WantUpdateMonitors = true;
            }
            break;
        }

        // Imgui Viewports
        //-------------------------------------------------------------------------

        if ( ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle( (void*) hwnd ) )
        {
            switch ( msg )
            {
                case WM_CLOSE:
                {
                    viewport->PlatformRequestClose = true;
                }
                break;

                case WM_MOVE:
                {
                    viewport->PlatformRequestMove = true;
                }
                break;

                case WM_SIZE:
                {
                    viewport->PlatformRequestResize = true;
                }
                break;

                case WM_MOUSEACTIVATE:
                if ( viewport->Flags & ImGuiViewportFlags_NoFocusOnClick )
                {
                    return MA_NOACTIVATE;
                }
                break;

                case WM_NCHITTEST:
                {
                    // Let mouse pass-through the window. This will allow the back-end to set io.MouseHoveredViewport properly (which is OPTIONAL).
                    // The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
                    // If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
                    // your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
                    if ( viewport->Flags & ImGuiViewportFlags_NoInputs )
                    {
                        return HTTRANSPARENT;
                    }
                }
                break;
            }
        }

        return 0;
    }

    // Return the default wnd proc if the message isn't handled, needed for child windows
    static LRESULT ImGui_ImplWin32_ChildWndProcHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
    {
        LRESULT result = ImGui_ImplWin32_WndProcHandler( hwnd, msg, wParam, lParam );
        if ( result != 0 )
        {
            return result;
        }

        return DefWindowProc( hwnd, msg, wParam, lParam );
    }

    //-------------------------------------------------------------------------

    void UpdateMousePosition()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendDataWin32* pBackendData = reinterpret_cast<ImGuiBackendDataWin32*>( io.BackendPlatformUserData );

        //-------------------------------------------------------------------------

        ImVec2 const mouse_pos_prev = io.MousePos;
        io.MousePos = ImVec2( -FLT_MAX, -FLT_MAX );
        io.MouseHoveredViewport = 0;

        // Obtain focused and hovered window. We forward mouse input when focused or when hovered (and no other window is capturing)
        HWND focused_window = ::GetForegroundWindow();
        HWND hovered_window = pBackendData->MouseHwnd;
        HWND mouse_window = NULL;

        if ( hovered_window && ( hovered_window == pBackendData->hWnd || ::IsChild( hovered_window, pBackendData->hWnd ) || ImGui::FindViewportByPlatformHandle( (void*) hovered_window ) ) )
        {
            mouse_window = hovered_window;
        }
        else if ( focused_window && ( focused_window == pBackendData->hWnd || ::IsChild( focused_window, pBackendData->hWnd ) || ImGui::FindViewportByPlatformHandle( (void*) focused_window ) ) )
        {
            mouse_window = focused_window;
        }

        if ( mouse_window == NULL )
        {
            return;
        }

        // Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        // (When multi-viewports are enabled, all Dear ImGui positions are same as OS positions)
        if ( io.WantSetMousePos )
        {
            POINT pos = { (int) mouse_pos_prev.x, (int) mouse_pos_prev.y };
            if ( ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) == 0 )
            {
                ::ClientToScreen( mouse_window, &pos );
            }
            ::SetCursorPos( pos.x, pos.y );
        }

        // Set imgui mouse position
        POINT mouse_screen_pos;
        if ( !::GetCursorPos( &mouse_screen_pos ) )
        {
            return;
        }

        if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
        {
            // Multi - viewport mode : mouse position in OS absolute coordinates( io.MousePos is( 0, 0 ) when the mouse is on the upper - left of the primary monitor )
            // This is the position you can get with ::GetCursorPos() or WM_MOUSEMOVE + ::ClientToScreen(). In theory adding viewport->Pos to a client position would also be the same.
            io.MousePos = ImVec2( (float) mouse_screen_pos.x, (float) mouse_screen_pos.y );
        }
        else
        {
            // Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
            // This is the position you can get with ::GetCursorPos() + ::ScreenToClient() or WM_MOUSEMOVE.
            POINT mouse_client_pos = mouse_screen_pos;
            ::ScreenToClient( pBackendData->hWnd, &mouse_client_pos );
            io.MousePos = ImVec2( (float) mouse_client_pos.x, (float) mouse_client_pos.y );
        }

        // (Optional) When using multiple viewports: set io.MouseHoveredViewport to the viewport the OS mouse cursor is hovering.
        // Important: this information is not easy to provide and many high-level windowing library won't be able to provide it correctly, because
        // - This is _ignoring_ viewports with the ImGuiViewportFlags_NoInputs flag (pass-through windows).
        // - This is _regardless_ of whether another viewport is focused or being dragged from.
        // If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the back-end, imgui will ignore this field and infer the information by relying on the
        // rectangles and last focused time of every viewports it knows about. It will be unaware of foreign windows that may be sitting between or over your windows.
        if ( HWND hovered_hwnd = ::WindowFromPoint( mouse_screen_pos ) )
        {
            if ( ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle( (void*) hovered_hwnd ) )
            {
                if ( ( viewport->Flags & ImGuiViewportFlags_NoInputs ) == 0 ) // FIXME: We still get our NoInputs window with WM_NCHITTEST/HTTRANSPARENT code when decorated?
                {
                    io.MouseHoveredViewport = viewport->ID;
                }
            }
        }
    }

    void UpdateMouseCursor()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendDataWin32* pBackendData = reinterpret_cast<ImGuiBackendDataWin32*>( io.BackendPlatformUserData );

        // Update OS mouse cursor with the cursor requested by imgui
        ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
        if ( pBackendData->LastMouseCursor != mouse_cursor )
        {
            pBackendData->LastMouseCursor = mouse_cursor;
            ImGui_ImplWin32_UpdateMouseCursor();
        }
    }

    //-------------------------------------------------------------------------
    // Platform Functions
    //-------------------------------------------------------------------------

    void InitializePlatform()
    {
        ImGuiIO& io = ImGui::GetIO();

        //-------------------------------------------------------------------------

        auto pBackendData = KRG::New<ImGuiBackendDataWin32>();

        pBackendData->hWnd = GetActiveWindow();
        pBackendData->WantUpdateHasGamepad = true;
        pBackendData->WantUpdateMonitors = true;
        pBackendData->LastMouseCursor = ImGuiMouseCursor_COUNT;

        io.BackendPlatformUserData = pBackendData;
        io.BackendPlatformName = "imgui_impl_win32";

        //-------------------------------------------------------------------------

        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can set io.MouseHoveredViewport correctly (optional, not easy)
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)

        ImGuiViewport* pMainViewport = ImGui::GetMainViewport();
        pMainViewport->PlatformHandle = pMainViewport->PlatformHandleRaw = (void*) pBackendData->hWnd;

        // Set key mappings
        //-------------------------------------------------------------------------

        io.KeyMap[ImGuiKey_Tab] = VK_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
        io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
        io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
        io.KeyMap[ImGuiKey_Home] = VK_HOME;
        io.KeyMap[ImGuiKey_End] = VK_END;
        io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
        io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
        io.KeyMap[ImGuiKey_Space] = VK_SPACE;
        io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
        io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
        io.KeyMap[ImGuiKey_A] = 'A';
        io.KeyMap[ImGuiKey_C] = 'C';
        io.KeyMap[ImGuiKey_V] = 'V';
        io.KeyMap[ImGuiKey_X] = 'X';
        io.KeyMap[ImGuiKey_Y] = 'Y';
        io.KeyMap[ImGuiKey_Z] = 'Z';

        // Set up platform IO bindings
        //-------------------------------------------------------------------------

        if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
        {
            UpdateDisplayInformation();

            ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
            platform_io.Platform_CreateWindow = ImGui_ImplWin32_CreateWindow;
            platform_io.Platform_DestroyWindow = ImGui_ImplWin32_DestroyWindow;
            platform_io.Platform_ShowWindow = ImGui_ImplWin32_ShowWindow;
            platform_io.Platform_SetWindowPos = ImGui_ImplWin32_SetWindowPos;
            platform_io.Platform_GetWindowPos = ImGui_ImplWin32_GetWindowPos;
            platform_io.Platform_SetWindowSize = ImGui_ImplWin32_SetWindowSize;
            platform_io.Platform_GetWindowSize = ImGui_ImplWin32_GetWindowSize;
            platform_io.Platform_SetWindowFocus = ImGui_ImplWin32_SetWindowFocus;
            platform_io.Platform_GetWindowFocus = ImGui_ImplWin32_GetWindowFocus;
            platform_io.Platform_GetWindowMinimized = ImGui_ImplWin32_GetWindowMinimized;
            platform_io.Platform_SetWindowTitle = ImGui_ImplWin32_SetWindowTitle;
            platform_io.Platform_SetWindowAlpha = ImGui_ImplWin32_SetWindowAlpha;
            platform_io.Platform_UpdateWindow = ImGui_ImplWin32_UpdateWindow;

            pBackendData->m_imguiWindowClass.cbSize = sizeof( WNDCLASSEX );
            pBackendData->m_imguiWindowClass.style = CS_HREDRAW | CS_VREDRAW;
            pBackendData->m_imguiWindowClass.lpfnWndProc = ImGui_ImplWin32_ChildWndProcHandler;
            pBackendData->m_imguiWindowClass.cbClsExtra = 0;
            pBackendData->m_imguiWindowClass.cbWndExtra = 0;
            pBackendData->m_imguiWindowClass.hInstance = ::GetModuleHandle( NULL );
            pBackendData->m_imguiWindowClass.hIcon = NULL;
            pBackendData->m_imguiWindowClass.hCursor = NULL;
            pBackendData->m_imguiWindowClass.hbrBackground = (HBRUSH) ( COLOR_BACKGROUND + 1 );
            pBackendData->m_imguiWindowClass.lpszMenuName = NULL;
            pBackendData->m_imguiWindowClass.lpszClassName = _T( "ImGui Platform" );
            pBackendData->m_imguiWindowClass.hIconSm = NULL;
            ::RegisterClassEx( &pBackendData->m_imguiWindowClass );

            //auto const errorMessage = KRG::Platform::Win32::GetLastErrorMessage();

            ImGuiViewportDataWin32* vd = KRG::New<ImGuiViewportDataWin32>();
            vd->Hwnd = pBackendData->hWnd;
            vd->HwndOwned = false;
            pMainViewport->PlatformUserData = vd;
        }
    }

    void ShutdownPlatform()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendDataWin32* pBackendData = reinterpret_cast<ImGuiBackendDataWin32*>( io.BackendPlatformUserData );

        //-------------------------------------------------------------------------

        if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
        {
            ::UnregisterClass( pBackendData->m_imguiWindowClass.lpszClassName, pBackendData->m_imguiWindowClass.hInstance );
        }

        KRG::Delete( pBackendData );

        //-------------------------------------------------------------------------

        io.BackendPlatformName = nullptr;
        io.BackendPlatformUserData = nullptr;
    }

    void UpdateDisplayInformation()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendDataWin32* pBackendData = reinterpret_cast<ImGuiBackendDataWin32*>( io.BackendPlatformUserData );

        RECT rect = { 0, 0, 0, 0 };
        ::GetClientRect( pBackendData->hWnd, &rect );
        io.DisplaySize = ImVec2( (float) ( rect.right - rect.left ), (float) ( rect.bottom - rect.top ) );

        if ( pBackendData->WantUpdateMonitors )
        {
            ImGui::GetPlatformIO().Monitors.resize( 0 );
            ::EnumDisplayMonitors( NULL, NULL, ImGui_ImplWin32_UpdateMonitors_EnumFunc, NULL );
            pBackendData->WantUpdateMonitors = false;
        }
    }

    void UpdateMouseInformation()
    {
        UpdateMousePosition();
        UpdateMouseCursor();
    }

    LRESULT WindowsMessageHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
    {
        return ImGui_ImplWin32_WndProcHandler( hWnd, message, wParam, lParam );
    }
}
#endif