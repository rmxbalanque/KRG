#include "DebugUISystem.h"
#include "System/Imgui/ImguiX.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Math/Viewport.h"

#if KRG_DEVELOPMENT_TOOLS
//-------------------------------------------------------------------------
// UI System
//-------------------------------------------------------------------------

namespace KRG::Debug
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
        UnregisterMenuCallbacks( ( *iter )->GetMenuCallbacks() );
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

    void DebugUISystem::Update( UpdateContext const& context, TInlineVector<Math::Viewport, 2> activeViewports )
    {
        if ( activeViewports.empty() )
        {
            return;
        }

        //-------------------------------------------------------------------------

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
                    DrawOrientationGuide( context, activeViewports.front() );
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
    namespace StatusBar
    {
        static float const g_height = 16.0f;
    }

    namespace OrientationGuide
    {
        static float const g_windowMarginX = 3.0f;
        static float const g_windowMarginY = 4.0f;
        static float const g_windowPadding = 4.0f;
        static float const g_windowRounding = 2.0f;
        static float const g_windowDimension = 55.0f;
        static float const g_axisHeadRadius = 3.0f;
        static float const g_axisHalfLength = ( g_windowDimension / 2 ) - g_axisHeadRadius - 4.0f;
        static float const g_worldRenderDistanceZ = 5.0f;
        static float const g_axisThickness = 2.0f;
    }

    //-------------------------------------------------------------------------

    void DebugUISystem::DrawPopups( UpdateContext const& context )
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

    void DebugUISystem::DrawOverlayMenu( UpdateContext const& context )
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

    void DebugUISystem::DrawDebugWindows( UpdateContext const& context )
    {
        for ( auto pView : m_viewControllers )
        {
            pView->DrawWindows( context );
        }
    }

    void DebugUISystem::DrawOverlayStatusBar( UpdateContext const& context )
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

    void DebugUISystem::DrawOrientationGuide( UpdateContext const& context, Math::Viewport const& viewport )
    {
        ImGuiIO& io = ImGui::GetIO();
        static bool alwaysOpen = true;

        Float2 const windowDimensions( OrientationGuide::g_windowDimension, OrientationGuide::g_windowDimension );
        Float2 const windowPosition = ImVec2( OrientationGuide::g_windowMarginX, io.DisplaySize.y - windowDimensions.m_y - StatusBar::g_height - OrientationGuide::g_windowMarginY );
        Float2 const windowCenter = Float2( windowPosition.m_x + windowDimensions.m_x / 2, windowPosition.m_y + windowDimensions.m_y / 2 );

        ImGui::SetNextWindowSize( windowDimensions );
        ImGui::SetNextWindowPos( windowPosition );
        ImGui::SetNextWindowBgAlpha( 0.0f );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, ImVec2( 0.0f, 0.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( OrientationGuide::g_windowPadding, OrientationGuide::g_windowPadding ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, OrientationGuide::g_windowRounding );
        uint32 const flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
        if ( ImGui::Begin( "DebugUI_OrientationGuide", &alwaysOpen, flags ) )
        {
            // Project world space axis positions to screen space
            //-------------------------------------------------------------------------

            Vector const& originWS = viewport.GetViewOrigin() + viewport.GetViewForwardDirection() * OrientationGuide::g_worldRenderDistanceZ;
            Vector const& worldAxisX = ( Vector::UnitX );
            Vector const& worldAxisY = ( Vector::UnitY );
            Vector const& worldAxisZ = ( Vector::UnitZ );

            Vector const& worldAxisForwardPointX = ( originWS + worldAxisX );
            Vector const& worldAxisForwardPointY = ( originWS + worldAxisY );
            Vector const& worldAxisForwardPointZ = ( originWS + worldAxisZ );
            Vector const& worldAxisBackwardPointX = ( originWS - worldAxisX );
            Vector const& worldAxisBackwardPointY = ( originWS - worldAxisY );
            Vector const& worldAxisBackwardPointZ = ( originWS - worldAxisZ );

            Vector const axisStartPointX = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointX ) );
            Vector const axisStartPointY = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointY ) );
            Vector const axisStartPointZ = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointZ ) );
            Vector const axisEndPointX = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointX ) );
            Vector const axisEndPointY = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointY ) );
            Vector const axisEndPointZ = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointZ ) );

            // Calculate screen space axis lengths
            //-------------------------------------------------------------------------

            float const axisLengthX = axisStartPointX.GetDistance2( axisEndPointX );
            float const axisLengthY = axisStartPointY.GetDistance2( axisEndPointY );
            float const axisLengthZ = axisStartPointZ.GetDistance2( axisEndPointZ );
            float const maxAxisLength = Math::Max( axisLengthX, Math::Max( axisLengthY, axisLengthZ ) );

            static float const axisHalfLengthSS = OrientationGuide::g_axisHalfLength;
            float const axisScaleX = ( axisLengthX / maxAxisLength ) * axisHalfLengthSS;
            float const axisScaleY = ( axisLengthY / maxAxisLength ) * axisHalfLengthSS;
            float const axisScaleZ = ( axisLengthZ / maxAxisLength ) * axisHalfLengthSS;

            // Calculate screen space axis directions
            Vector const origin = viewport.WorldSpaceToScreenSpace( originWS );
            Vector const axisDirX = ( axisEndPointX - origin ).GetNormalized2();
            Vector const axisDirY = ( axisEndPointY - origin ).GetNormalized2();
            Vector const axisDirZ = ( axisEndPointZ - origin ).GetNormalized2();

            // Sort SS axis and draw them in the correct order
            //-------------------------------------------------------------------------

            struct AxisDrawRequest { Axis m_axis; bool m_isInForwardDirection; float m_distance; };
            TInlineVector<AxisDrawRequest, 3> drawRequests;

            Plane const NearPlane = viewport.GetViewVolume().GetViewPlane( Math::ViewVolume::PlaneID::Near );

            drawRequests.push_back( { Axis::X, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointX ) } );
            drawRequests.push_back( { Axis::Y, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointY ) } );
            drawRequests.push_back( { Axis::Z, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointZ ) } );
            drawRequests.push_back( { Axis::X, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointX ) } );
            drawRequests.push_back( { Axis::Y, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointY ) } );
            drawRequests.push_back( { Axis::Z, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointZ ) } );

            eastl::sort( drawRequests.begin(), drawRequests.end(), [] ( AxisDrawRequest const& lhs, AxisDrawRequest const& rhs ) { return lhs.m_distance > rhs.m_distance; } );

            //-------------------------------------------------------------------------

            auto pDrawList = ImGui::GetWindowDrawList();
            for ( auto const& request : drawRequests )
            {
                // X
                if ( request.m_axis == Axis::X && request.m_isInForwardDirection )
                {
                    pDrawList->AddLine( windowCenter, windowCenter + axisDirX * ( axisScaleX - OrientationGuide::g_axisHeadRadius + 1.0f ), 0xBB0000FF, OrientationGuide::g_axisThickness );
                    pDrawList->AddCircleFilled( windowCenter + axisDirX * axisScaleX, OrientationGuide::g_axisHeadRadius, 0xBB0000FF );
                }
                else if ( request.m_axis == Axis::X && !request.m_isInForwardDirection )
                {
                    pDrawList->AddCircleFilled( windowCenter - axisDirX * axisScaleX, OrientationGuide::g_axisHeadRadius, 0x660000FF );
                }
                //Y
                else if ( request.m_axis == Axis::Y && request.m_isInForwardDirection )
                {
                    pDrawList->AddLine( windowCenter, windowCenter + axisDirY * ( axisScaleY - OrientationGuide::g_axisHeadRadius + 1.0f ), 0xBB00FF00, OrientationGuide::g_axisThickness );
                    pDrawList->AddCircleFilled( windowCenter + axisDirY * axisScaleY, OrientationGuide::g_axisHeadRadius, 0xBB00FF00 );
                }
                else if ( request.m_axis == Axis::Y && !request.m_isInForwardDirection )
                {
                    pDrawList->AddCircleFilled( windowCenter - axisDirY * axisScaleY, OrientationGuide::g_axisHeadRadius, 0x6600FF00 );
                }
                // Z
                else if ( request.m_axis == Axis::Z && request.m_isInForwardDirection )
                {
                    pDrawList->AddLine( windowCenter, windowCenter + axisDirZ * ( axisScaleZ - OrientationGuide::g_axisHeadRadius + 1.0f ), 0xBBFF0000, OrientationGuide::g_axisThickness );
                    pDrawList->AddCircleFilled( windowCenter + axisDirZ * axisScaleZ, OrientationGuide::g_axisHeadRadius, 0xBBFF0000 );
                }
                else if ( request.m_axis == Axis::Z && !request.m_isInForwardDirection )
                {
                    pDrawList->AddCircleFilled( windowCenter - axisDirZ * axisScaleZ, OrientationGuide::g_axisHeadRadius, 0x66FF0000 );
                }
            }

            ImGui::End();
        }
        ImGui::PopStyleVar( 3 );
    }
}
#endif