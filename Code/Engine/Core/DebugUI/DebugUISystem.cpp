#include "DebugUISystem.h"
#include "System/Imgui/ImguiCore.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Settings/SettingsRegistry.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Debug
    {
        DebugUISystem::Menu::MenuPath DebugUISystem::Menu::CreatePathFromString( String const& pathString )
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

        void DebugUISystem::Menu::AddCallback( DebugMenuCallback const* pCallback )
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

        void DebugUISystem::Menu::RemoveCallback( DebugMenuCallback const* pCallback )
        {
            bool const result = TryFindAndRemoveMenuCallback( pCallback );
            KRG_ASSERT( result );
        }

        DebugUISystem::Menu& DebugUISystem::Menu::FindOrAddSubMenu( MenuPath const& path )
        {
            KRG_ASSERT( !path.empty() );

            size_t const numPathLevels = path.size();
            S32 currentPathLevel = 0;
            
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

        bool DebugUISystem::Menu::TryFindMenuCallback( DebugMenuCallback const* pCallback )
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

        bool DebugUISystem::Menu::TryFindAndRemoveMenuCallback( DebugMenuCallback const* pCallback )
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

        void DebugUISystem::Menu::RemoveEmptyChildMenus()
        {
            for ( auto iter = m_childMenus.begin(); iter != m_childMenus.end(); )
            {
                ( *iter ).RemoveEmptyChildMenus();

                if ( (*iter).IsEmpty() )
                {
                   iter = m_childMenus.erase( iter );
                }
                else
                {
                    ++iter;
                }
            }
        }

        void DebugUISystem::Menu::DrawMenu( UpdateContext const& context )
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

        DebugUISystem::~DebugUISystem()
        {
            KRG_ASSERT( m_viewControllers.empty() );
            KRG_ASSERT( m_mainMenu.IsEmpty() );
        }

        //-------------------------------------------------------------------------

        void DebugUISystem::Initialize( SettingsRegistry const& settingsRegistry )
        {
            m_pSettingsRegistry = &settingsRegistry;
            RegisterDebugSettings();
        }

        void DebugUISystem::Shutdown()
        {
            UnregisterDebugSettings();
            m_pSettingsRegistry = nullptr;
        }

        //-------------------------------------------------------------------------

        void DebugUISystem::RegisterDebugView( DebugView* pDebugView )
        {
            KRG_ASSERT( pDebugView != nullptr );
            m_viewControllers.push_back( pDebugView );
            RegisterMenuCallbacks( pDebugView->GetMenuCallbacks() );
        }

        void DebugUISystem::UnregisterDebugView( DebugView* pDebugView )
        {
            KRG_ASSERT( pDebugView != nullptr );
            auto iter = VectorFind( m_viewControllers, pDebugView );
            KRG_ASSERT( iter != m_viewControllers.end() );
            UnregisterMenuCallbacks( (*iter)->GetMenuCallbacks() );
            m_viewControllers.erase( iter );
        }

        //-------------------------------------------------------------------------

        void DebugUISystem::RegisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks )
        {
            for ( auto& callback : callbacks )
            {
                m_mainMenu.AddCallback( &callback );
            }
        }

        void DebugUISystem::UnregisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks )
        {
            for ( auto& callback : callbacks )
            {
                m_mainMenu.RemoveCallback( &callback );
            }
        }

        //-------------------------------------------------------------------------

        void DebugUISystem::RegisterDebugSettings()
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

        void DebugUISystem::UnregisterDebugSettings()
        {
            UnregisterMenuCallbacks( m_debugSettingMenuCallbacks );
            m_debugSettingMenuCallbacks.clear();
        }

        //-------------------------------------------------------------------------

        void DebugUISystem::Update( UpdateContext const& context )
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

                    F32 const k = 2.0f / ( context.GetFrameID() + 1 );
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
                    }

                    // The debug windows should be always be drawn if enabled
                    DrawDebugWindows( context );
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

        //-------------------------------------------------------------------------

        void DebugUISystem::DrawOverlayMenu( UpdateContext const& context )
        {
            if ( ImGui::BeginMainMenuBar() )
            {
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

        void DebugUISystem::DrawDebugWindows( UpdateContext const& context )
        {
            for ( auto pView : m_viewControllers )
            {
                pView->DrawWindows( context );
            }
        }

        void DebugUISystem::DrawOverlayStatusBar( UpdateContext const& context )
        {
            static const F32 barHeight = 14;
            bool showAlways = true;
            U32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos( ImVec2( 0, io.DisplaySize.y - barHeight ) );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 1.0f ) );

            ImGui::SetNextWindowBgAlpha( 0.75f );
            ImGui::SetNextWindowSize( ImVec2( io.DisplaySize.x, barHeight ) );
            if ( ImGui::Begin( "Stats", &showAlways, flags ) )
            {
                float const CurrentFPS = 1.0f / context.GetDeltaTime();
                float const AvgFPS = 1.0f / m_avgTimeDelta;
                float const RequestedMemory = Memory::GetTotalRequestedMemory() / 1024.0f / 1024.0f;
                float const AllocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;

                ImGui::Text( "Current FPS: %4.0f, Avg FPS: %4.0f", CurrentFPS, AvgFPS );
                ImGui::SameLine();
                ImGui::TextColored( ( Colors::Cyan ).ToFloat4(), "|" );
                ImGui::SameLine();
                ImGui::Text( "Memory: %.2fMB Requested (%.2fMB Allocated)", RequestedMemory, AllocatedMemory );

                ImGui::End();
            }
            ImGui::PopStyleVar();
        }

        //-------------------------------------------------------------------------

        void DebugUISystem::DrawSettingUI( Setting* pSettingBase )
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
                    S32 value = *pSetting;

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
                    F32 value = *pSetting;

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
}
#endif