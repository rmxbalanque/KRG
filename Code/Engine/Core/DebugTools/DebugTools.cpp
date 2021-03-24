#include "DebugTools.h"
#include "System/DevTools/ImguiX.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Core/Math/Viewport.h"
#include "System/DevTools/CommonWidgets/CommonWidgets.h"

#if KRG_DEVELOPMENT_TOOLS

//-------------------------------------------------------------------------
// UI System
//-------------------------------------------------------------------------

namespace KRG::Debug
{
    namespace StatusBar
    {
        static float const g_height = 16.0f;
    }

    //-------------------------------------------------------------------------

    DebugTools::Menu::MenuPath DebugTools::Menu::CreatePathFromString( String const& pathString )
    {
        KRG_ASSERT( !pathString.empty() );
        size_t const categoryNameLength = pathString.length();

        // Validation
        KRG_ASSERT( categoryNameLength < 256 );
        if ( categoryNameLength > 0 )
        {
            for ( auto i = 0; i < categoryNameLength - 1; i++ )
            {
                KRG_ASSERT( isalnum( pathString[i] ) || pathString[i] == ' ' || pathString[i] == '/' );
            }
        }

        //-------------------------------------------------------------------------

        MenuPath path;
        StringUtils::Split( pathString, path, "/" );
        return path;
    }

    void DebugTools::Menu::AddCallback( DebugMenuCallback const* pCallback )
    {
        KRG_ASSERT( !TryFindMenuCallback( pCallback ) );

        auto const menuPath = CreatePathFromString( pCallback->GetCategory() );
        auto& subMenu = FindOrAddSubMenu( menuPath );

        // Add new callback and sort callback list
        //-------------------------------------------------------------------------

        subMenu.m_callbacks.emplace_back( pCallback );

        auto callbackSortPredicate = [] ( DebugMenuCallback const* const& pA, DebugMenuCallback const* const& pB )
        {
            return pA->GetName() < pB->GetName();
        };

        eastl::sort( subMenu.m_callbacks.begin(), subMenu.m_callbacks.end(), callbackSortPredicate );
    }

    void DebugTools::Menu::RemoveCallback( DebugMenuCallback const* pCallback )
    {
        bool const result = TryFindAndRemoveMenuCallback( pCallback );
        KRG_ASSERT( result );
    }

    DebugTools::Menu& DebugTools::Menu::FindOrAddSubMenu( MenuPath const& path )
    {
        KRG_ASSERT( !path.empty() );

        size_t const numPathLevels = path.size();
        int32 currentPathLevel = 0;

        Menu* pCurrentMenu = this;
        while ( true )
        {
            // Check all child menus for a menu that matches the current path level name
            bool childMenuFound = false;
            for ( auto& childMenu : pCurrentMenu->m_childMenus )
            {
                // If the child menu matches the title, continue the search from the child menu
                if ( childMenu.m_title == path[currentPathLevel] )
                {
                    pCurrentMenu = &childMenu;
                    currentPathLevel++;

                    if ( currentPathLevel == path.size() )
                    {
                        return childMenu;
                    }
                    else
                    {
                        childMenuFound = true;
                        break;
                    }
                }
            }

            //-------------------------------------------------------------------------

            // If we didn't find a matching menu add a new one and redo the search
            if ( !childMenuFound )
            {
                // Add new child menu and sort the child menu list
                pCurrentMenu->m_childMenus.emplace_back( Menu( path[currentPathLevel] ) );

                auto menuSortPredicate = [] ( Menu const& menuA, Menu const& menuB )
                {
                    return menuA.m_title < menuB.m_title;
                };

                eastl::sort( pCurrentMenu->m_childMenus.begin(), pCurrentMenu->m_childMenus.end(), menuSortPredicate );

                // Do not change current menu or current path level, we will redo the child menu 
                // search in the next loop update and find the new menu, this is simpler than trying
                // to sort, and redo the search here
            }
        }

        //-------------------------------------------------------------------------

        KRG_UNREACHABLE_CODE();
        return *this;
    }

    bool DebugTools::Menu::TryFindMenuCallback( DebugMenuCallback const* pCallback )
    {
        auto iter = VectorFind( m_callbacks, pCallback );
        if ( iter != m_callbacks.end() )
        {
            return true;
        }
        else
        {
            for ( auto& childMenu : m_childMenus )
            {
                if ( childMenu.TryFindMenuCallback( pCallback ) )
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool DebugTools::Menu::TryFindAndRemoveMenuCallback( DebugMenuCallback const* pCallback )
    {
        auto iter = VectorFind( m_callbacks, pCallback );
        if ( iter != m_callbacks.end() )
        {
            m_callbacks.erase( iter );
            return true;
        }
        else
        {
            for ( auto& childMenu : m_childMenus )
            {
                if ( childMenu.TryFindAndRemoveMenuCallback( pCallback ) )
                {
                    RemoveEmptyChildMenus();
                    return true;
                }
            }
        }

        return false;
    }

    void DebugTools::Menu::RemoveEmptyChildMenus()
    {
        for ( auto iter = m_childMenus.begin(); iter != m_childMenus.end(); )
        {
            ( *iter ).RemoveEmptyChildMenus();

            if ( ( *iter ).IsEmpty() )
            {
                iter = m_childMenus.erase( iter );
            }
            else
            {
                ++iter;
            }
        }
    }

    void DebugTools::Menu::DrawMenu( UpdateContext const& context )
    {
        for ( auto& childMenu : m_childMenus )
        {
            KRG_ASSERT( !childMenu.IsEmpty() );

            if ( ImGui::BeginMenu( childMenu.m_title.c_str() ) )
            {
                childMenu.DrawMenu( context );
                ImGui::EndMenu();
            }
        }

        //-------------------------------------------------------------------------

        for ( auto& callback : m_callbacks )
        {
            callback->Execute( context );
        }
    }

    //-------------------------------------------------------------------------

    DebugTools::~DebugTools()
    {
        KRG_ASSERT( m_viewControllers.empty() );
        KRG_ASSERT( m_mainMenu.IsEmpty() );
    }

    //-------------------------------------------------------------------------

    void DebugTools::Initialize( SettingsRegistry const& settingsRegistry )
    {
        m_pSettingsRegistry = &settingsRegistry;
        RegisterDebugSettings();
    }

    void DebugTools::Shutdown()
    {
        UnregisterDebugSettings();
        m_pSettingsRegistry = nullptr;
    }

    //-------------------------------------------------------------------------

    void DebugTools::RegisterDebugView( DebugView* pDebugView )
    {
        KRG_ASSERT( pDebugView != nullptr );
        m_viewControllers.push_back( pDebugView );
        RegisterMenuCallbacks( pDebugView->GetMenuCallbacks() );
    }

    void DebugTools::UnregisterDebugView( DebugView* pDebugView )
    {
        KRG_ASSERT( pDebugView != nullptr );
        auto iter = VectorFind( m_viewControllers, pDebugView );
        KRG_ASSERT( iter != m_viewControllers.end() );
        UnregisterMenuCallbacks( ( *iter )->GetMenuCallbacks() );
        m_viewControllers.erase( iter );
    }

    //-------------------------------------------------------------------------

    void DebugTools::RegisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks )
    {
        for ( auto& callback : callbacks )
        {
            m_mainMenu.AddCallback( &callback );
        }
    }

    void DebugTools::UnregisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks )
    {
        for ( auto& callback : callbacks )
        {
            m_mainMenu.RemoveCallback( &callback );
        }
    }

    //-------------------------------------------------------------------------

    void DebugTools::RegisterDebugSettings()
    {
        // Create menu callbacks for all settings
        //-------------------------------------------------------------------------

        auto const& debugSettings = m_pSettingsRegistry->GetAllDebugSettings();
        m_debugSettingMenuCallbacks.reserve( debugSettings.size() );

        for ( auto& debugSetting : debugSettings )
        {
            auto pSetting = debugSetting.second;
            auto drawFunction = [this, pSetting] ( UpdateContext const& context )
            {
                DrawSettingUI( pSetting );
            };

            m_debugSettingMenuCallbacks.emplace_back( DebugMenuCallback( pSetting->GetName(), pSetting->GetCategory(), drawFunction ) );
        }

        // Register all callbacks
        //-------------------------------------------------------------------------

        RegisterMenuCallbacks( m_debugSettingMenuCallbacks );
    }

    void DebugTools::UnregisterDebugSettings()
    {
        UnregisterMenuCallbacks( m_debugSettingMenuCallbacks );
        m_debugSettingMenuCallbacks.clear();
    }

    //-------------------------------------------------------------------------

    void DebugTools::Update( UpdateContext const& context, Render::ViewportManager& viewportSystem )
    {
        UpdateStage const updateStage = context.GetUpdateStage();

        switch ( updateStage )
        {
            case UpdateStage::FrameStart:
            {
                auto pInputSystem = context.GetSystem<Input::InputSystem>();
                KRG_ASSERT( pInputSystem != nullptr );

                // Update internal state
                //-------------------------------------------------------------------------

                float const k = 2.0f / ( context.GetFrameID() + 1 );
                m_avgTimeDelta = context.GetRawDeltaTime() * k + m_avgTimeDelta * ( 1.0f - k );

                // Enable/disable debug overlay
                //-------------------------------------------------------------------------

                auto const pKeyboardState = pInputSystem->GetKeyboardState();
                if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_Tilde ) )
                {
                    m_debugOverlayEnabled = !m_debugOverlayEnabled;
                }
            }
            break;

            //-------------------------------------------------------------------------

            case UpdateStage::FrameEnd:
            {
                if ( m_debugOverlayEnabled )
                {
                    DrawOverlayMenu( context );
                    DrawOverlayStatusBar( context );
                    ImGuiX::DrawOrientationGuide( viewportSystem.GetActiveViewports().front(), ImVec2( 3.0f, 4.0f + StatusBar::g_height ) );
                }

                // The debug windows should be always be drawn if enabled
                DrawDebugWindows( context );

                // The pop-ups should be always be drawn if enabled
                DrawPopups( context );
            }
            break;

            //-------------------------------------------------------------------------

            default:
            {
                KRG_HALT(); // Not allowed to run at this stage
            }
            break;
        }
    }
}

//-------------------------------------------------------------------------
// Drawing
//-------------------------------------------------------------------------

namespace KRG::Debug
{
    void DebugTools::DrawPopups( UpdateContext const& context )
    {
        // Get any new warnings/errors and create pop-ups for them
        //-------------------------------------------------------------------------

        auto const unhandledWarningsAndErrors = Log::GetUnhandledWarningsAndErrors();
        for ( auto const& entry : unhandledWarningsAndErrors )
        {
            auto& popupMessage = m_modalPopups.emplace_back( ModalPopupMessage() );

            UUID const ID = UUID::GenerateID();
            popupMessage.m_ID = ( entry.m_severity == Log::Severity::Warning ) ? "Warning##" : "Error##";
            popupMessage.m_ID += ID.ToString();
            popupMessage.m_channel = entry.m_channel;
            popupMessage.m_severity = entry.m_severity;
            popupMessage.m_message = entry.m_message;
        }

        // Remove closed warnings/errors
        //-------------------------------------------------------------------------

        for ( auto i = 0; i < m_modalPopups.size(); i++ )
        {
            if ( !m_modalPopups[i].m_isOpen )
            {
                m_modalPopups.erase( m_modalPopups.begin() + i );
                i--;
            }
        }

        // Draw pop-ups
        //-------------------------------------------------------------------------

        if( HasModalPopupOpen() )
        {
            // Always draw latest first
            auto& message = m_modalPopups.back();

            ImVec4 const titleBarColor = ( message.m_severity == Log::Severity::Warning ) ? ImGuiX::ConvertColor( Colors::Yellow ) : ImGuiX::ConvertColor( Colors::Red );
            ImVec4 const titleBarTextColor = ( message.m_severity == Log::Severity::Warning ) ? ImGuiX::ConvertColor( Colors::Black ) : ImGuiX::ConvertColor( Colors::Black );

            ImGui::OpenPopup( message.m_ID.c_str() );
            ImGui::PushStyleColor( ImGuiCol_Text, titleBarTextColor );
            ImGui::PushStyleColor( ImGuiCol_TitleBgActive, titleBarColor );
            if ( ImGui::BeginPopupModal( message.m_ID.c_str(), &message.m_isOpen, ImGuiWindowFlags_NoSavedSettings ) )
            {
                ImGui::PopStyleColor( 2 );

                ImGui::BeginGroup();
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, titleBarColor );
                    ImGuiX::ScopedFont font( ImGuiX::Font::ExtraLarge );
                    ImGui::Text( KRG_ICON_EXCLAMATION_TRIANGLE );
                    ImGui::PopStyleColor( 1 );
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::Text( "Channel: %s", message.m_channel.c_str() );
                    ImGui::Text( "Message: %s", message.m_message.c_str() );
                }
                ImGui::EndGroup();

                //-------------------------------------------------------------------------

                ImGui::NewLine();
                ImGui::NewLine();
                ImGui::SameLine( ( ImGui::GetWindowWidth() - 100 ) / 2 );
                if ( ImGui::Button( "Ok", ImVec2( 100, 0 ) ) )
                {
                    message.m_isOpen = false;
                }

                //-------------------------------------------------------------------------

                ImGui::EndPopup();
            }
        }
    }

    void DebugTools::DrawOverlayMenu( UpdateContext const& context )
    {
        if ( ImGui::BeginMainMenuBar() )
        {
            ImGui::TextColored( ImGuiX::Theme::s_accentColorDark, KRG_ICON_BUG );

            if ( m_mainMenu.IsEmpty() )
            {
                ImGui::Text( "No Menu Options" );
            }
            else
            {
                m_mainMenu.DrawMenu( context );
            }

            ImGui::EndMainMenuBar();
        }
    }

    void DebugTools::DrawDebugWindows( UpdateContext const& context )
    {
        for ( auto pView : m_viewControllers )
        {
            pView->DrawWindows( context );
        }
    }

    void DebugTools::DrawOverlayStatusBar( UpdateContext const& context )
    {
        bool showAlways = true;
        uint32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos( ImVec2( 0, io.DisplaySize.y - StatusBar::g_height ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 1.0f ) );

        ImGui::SetNextWindowBgAlpha( 0.75f );
        ImGui::SetNextWindowSize( ImVec2( io.DisplaySize.x, StatusBar::g_height ) );
        if ( ImGui::Begin( "Stats", &showAlways, flags ) )
        {
            float const currentFPS = 1.0f / context.GetDeltaTime();
            float const avgFPS = 1.0f / m_avgTimeDelta;
            float const requestedMemory = Memory::GetTotalRequestedMemory() / 1024.0f / 1024.0f;
            float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
            Seconds const currentEngineTime = EngineClock::GetTimeInSeconds();

            ImGui::Text( "Current FPS: %4.0f, Avg FPS: %4.0f", currentFPS, avgFPS );
            ImGui::SameLine();
            ImGui::TextColored( ( Colors::Cyan ).ToFloat4(), "|" );
            ImGui::SameLine();
            ImGui::Text( "Engine Time: %.3f", currentEngineTime.ToFloat() );
            ImGui::SameLine();
            ImGui::TextColored( ( Colors::Cyan ).ToFloat4(), "|" );
            ImGui::SameLine();
            ImGui::Text( "Memory: %.2fMB Requested (%.2fMB Allocated)", requestedMemory, allocatedMemory );

            ImGui::End();
        }
        ImGui::PopStyleVar();
    }

    //-------------------------------------------------------------------------

    void DebugTools::DrawSettingUI( Setting* pSettingBase )
    {
        KRG_ASSERT( pSettingBase != nullptr );

        //-------------------------------------------------------------------------

        switch ( pSettingBase->GetType() )
        {
            case Setting::Type::Bool:
            {
                auto pSetting = static_cast<DebugSettingBool*>( pSettingBase );
                bool value = *pSetting;
                if ( ImGui::Checkbox( pSettingBase->GetName(), &value ) )
                {
                    *pSetting = value;
                }
            }
            break;

            case Setting::Type::Int:
            {
                auto pSetting = static_cast<DebugSettingInt*>( pSettingBase );
                int32 value = *pSetting;

                if ( pSetting->HasLimits() )
                {
                    if ( ImGui::SliderInt( pSettingBase->GetName(), &value, pSetting->GetMin(), pSetting->GetMax() ) )
                    {
                        *pSetting = value;
                    }
                }
                else
                {
                    if ( ImGui::InputInt( pSettingBase->GetName(), &value ) )
                    {
                        *pSetting = value;
                    }
                }
            }
            break;

            case Setting::Type::Float:
            {
                auto pSetting = static_cast<DebugSettingFloat*>( pSettingBase );
                float value = *pSetting;

                if ( pSetting->HasLimits() )
                {
                    if ( ImGui::SliderFloat( pSettingBase->GetName(), &value, pSetting->GetMin(), pSetting->GetMax() ) )
                    {
                        *pSetting = value;
                    }
                }
                else
                {
                    if ( ImGui::InputFloat( pSettingBase->GetName(), &value, 0.1f, 1.0f ) )
                    {
                        *pSetting = value;
                    }
                }
            }
            break;
        }
    }
}
#endif