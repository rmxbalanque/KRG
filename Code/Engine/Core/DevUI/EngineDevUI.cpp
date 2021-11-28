#include "EngineDevUI.h"
#include "OrientationGuide.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/Entity/EntityWorldDebugger.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Systems/WorldSystem.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    constexpr static float const g_menuHeight = 19.0f;
    constexpr static float const g_statusBarHeight = 21.0f;

    //-------------------------------------------------------------------------

    void EngineDevUI::Initialize( UpdateContext const& context )
    {
        m_pWorldManager = context.GetSystem<EntityWorldManager>();
        for ( auto pWorld : m_pWorldManager->GetWorlds() )
        {
            if ( pWorld->IsGameWorld() )
            {
                m_pWorldDebugger = KRG::New<EntityWorldDebugger>( pWorld );
                break;
            }
        }
    }

    void EngineDevUI::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pWorldDebugger );
        m_pWorldManager = nullptr;
    }

    //-------------------------------------------------------------------------

    void EngineDevUI::FrameEndUpdate( UpdateContext const& context )
    {
        UpdateStage const updateStage = context.GetUpdateStage();
        KRG_ASSERT( updateStage == UpdateStage::FrameEnd );

        //-------------------------------------------------------------------------

        auto pInputSystem = context.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );

        // Update internal state
        //-------------------------------------------------------------------------

        float const k = 2.0f / ( context.GetFrameID() + 1 );
        m_avgTimeDelta = context.GetRawDeltaTime() * k + m_avgTimeDelta * ( 1.0f - k );

        // Get game world
        //-------------------------------------------------------------------------

        EntityWorld* pGameWorld = nullptr;
        for ( auto pWorld : m_pWorldManager->GetWorlds() )
        {
            if ( pWorld->IsGameWorld() )
            {
                pGameWorld = pWorld;
                break;
            }
        }

        if ( pGameWorld == nullptr )
        {
            return;
        }

        // Enable/disable debug overlay
        //-------------------------------------------------------------------------

        auto const pKeyboardState = pInputSystem->GetKeyboardState();
        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_Tilde ) )
        {
            m_debugOverlayEnabled = !m_debugOverlayEnabled;

            auto pWorldSystem = pGameWorld->GetWorldSystem<WorldSystem>();
            pWorldSystem->SetDevelopmentPlayerEnabled( m_debugOverlayEnabled );
        }

        //-------------------------------------------------------------------------

        Render::Viewport const* pViewport = pGameWorld->GetViewport();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus;
        if ( m_debugOverlayEnabled )
        {
            windowFlags |= ImGuiWindowFlags_MenuBar;
        }
        
        ImVec2 windowPos, windowSize;

        if ( !m_windowName.empty() )
        {
            auto pWindow = ImGui::FindWindowByName( m_windowName.c_str() );
            KRG_ASSERT( pWindow != nullptr );
            windowPos = pWindow->Pos;
            windowSize = pWindow->Size;
        }
        else
        {
            windowPos = ImVec2( 0, 0 );
            windowSize = ImVec2( pViewport->GetDimensions() );
        }

        ImGui::SetNextWindowPos( windowPos );
        ImGui::SetNextWindowSize( windowSize );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        ImGui::SetNextWindowBgAlpha( 0.0f );
        if ( ImGui::Begin( "ViewportOverlay", nullptr, windowFlags ) )
        {
            ImGui::PopStyleVar( 2 );

            // The overlay elements should always be drawn
            DrawOverlayElements( context, pViewport );

            //-------------------------------------------------------------------------

            if ( m_debugOverlayEnabled )
            {
                if ( ImGui::BeginMenuBar() )
                {
                    DrawMenu( context );

                    // Draw Performance Stats
                    //-------------------------------------------------------------------------

                    float const currentFPS = 1.0f / context.GetDeltaTime();
                    float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
                    InlineString<200> const perfStatStr( InlineString<200>::CtorSprintf(), "FPS: %3.0f Mem: %.2fMB", currentFPS, allocatedMemory );

                    ImVec2 const availableSpace = ImGui::GetContentRegionAvail();
                    ImVec2 const textSize = ImGui::CalcTextSize( perfStatStr.c_str() );

                    ImGui::SameLine( 0, availableSpace.x - textSize.x - 8.0f );
                    ImGui::Text( perfStatStr.c_str() );

                    ImGui::EndMenuBar();
                }

                DrawStatusBar( context );
            }
        }
        ImGui::End();

        // The debug windows should be always be drawn if enabled
        DrawWindows( context );

        // The pop-ups should be always be drawn if enabled
        DrawPopups( context );
    }

    //-------------------------------------------------------------------------
    // Drawing
    //-------------------------------------------------------------------------

    void EngineDevUI::DrawPopups( UpdateContext const& context )
    {
        // Get any new warnings/errors and create pop-ups for them
        //-------------------------------------------------------------------------

        auto const unhandledWarningsAndErrors = Log::GetUnhandledWarningsAndErrors();
        for ( auto const& entry : unhandledWarningsAndErrors )
        {
            auto& popupMessage = m_modalPopups.emplace_back( ModalPopupMessage() );

            UUID const ID = UUID::GenerateID();
            popupMessage.m_ID = ( entry.m_severity == Log::Severity::Warning ) ? "Warning##" : "Error##";
            popupMessage.m_ID += ID.ToString().c_str();
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

        if( !m_modalPopups.empty() )
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
                    ImGuiX::ScopedFont font( ImGuiX::Font::Huge );
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

    void EngineDevUI::DrawMenu( UpdateContext const& context )
    {
        ImGui::TextColored( ImGuiX::ConvertColor( Colors::LimeGreen ), KRG_ICON_BUG );
        m_pWorldDebugger->DrawMenu( context );
    }

    void EngineDevUI::DrawOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        m_pWorldDebugger->DrawOverlayElements( context );

        if ( m_debugOverlayEnabled )
        {
            ImVec2 const guidePosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax() - ImVec2( 0, g_statusBarHeight ) - ( ImGuiX::OrientationGuide::GetSize() / 2 );
            ImGuiX::OrientationGuide::Draw( guidePosition, *pViewport );
        }
    }

    void EngineDevUI::DrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        m_pWorldDebugger->DrawWindows( context, pWindowClass );

        //-------------------------------------------------------------------------

        if ( m_isLogWindowOpen )
        {
            ImGui::SetNextWindowBgAlpha( 0.75f );
            m_isLogWindowOpen = m_systemLogView.Draw( context );
        }

        if ( m_isDebugSettingsWindowOpen )
        {
            ImGui::SetNextWindowBgAlpha( 0.75f );
            m_isDebugSettingsWindowOpen = m_debugSettingsView.Draw( context );
        }
    }

    void EngineDevUI::DrawStatusBar( UpdateContext const& context )
    {
        ImVec2 const parentWindowPos = ImGui::GetWindowPos();
        ImVec2 const totalSpace = ImGui::GetContentRegionMax();
        ImVec2 const windowPos( parentWindowPos.x, parentWindowPos.y + totalSpace.y - g_statusBarHeight );
        ImVec2 const windowSize( totalSpace.x, g_statusBarHeight );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 2.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 4.0f, 1.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

        ImGui::SetNextWindowBgAlpha( 0.85f );
        ImGui::SetNextWindowPos( windowPos );
        if ( ImGui::BeginChild( "Stats", windowSize, false, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar ) )
        {
            InlineString<100> tempStr;

            if ( ImGui::Button( KRG_ICON_SLIDERS" Debug Settings" ) )
            {
                m_isDebugSettingsWindowOpen = true;
            }

            ImGuiX::VerticalSeparator();

            tempStr.sprintf( KRG_ICON_QUESTION_CIRCLE" %d", Log::GetNumWarnings() );
            if ( ImGui::Button( tempStr.c_str() ) )
            {
                m_isLogWindowOpen = true;
                m_systemLogView.m_showLogMessages = false;
                m_systemLogView.m_showLogWarnings = true;
                m_systemLogView.m_showLogErrors = false;
            }
            
            ImGuiX::VerticalSeparator();

            tempStr.sprintf( KRG_ICON_EXCLAMATION_CIRCLE" %d", Log::GetNumErrors() );
            if ( ImGui::Button( tempStr.c_str() ) )
            {
                m_isLogWindowOpen = true;
                m_systemLogView.m_showLogMessages = false;
                m_systemLogView.m_showLogWarnings = false;
                m_systemLogView.m_showLogErrors = true;
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 3 );
    }
}
#endif