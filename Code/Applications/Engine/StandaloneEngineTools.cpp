#include "StandaloneEngineTools.h"
#include "Engine/Core/Imgui/OrientationGuide.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/Entity/Debug/EntityWorldDebugger.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Input/InputSystem.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Core/Settings/SettingsRegistry.h"

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    namespace StatusBar
    {
        static float const g_height = 16.0f;
    }

    //-------------------------------------------------------------------------

    void StandaloneEngineTools::Initialize( UpdateContext const& context )
    {
        m_pWorldManager = context.GetSystem<EntityWorldManager>();
        m_pWorldDebugger = KRG::New<EntityWorldDebugger>( m_pWorldManager->GetPrimaryWorld() );
    }

    void StandaloneEngineTools::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pWorldDebugger );
        m_pWorldManager = nullptr;
    }

    //-------------------------------------------------------------------------

    void StandaloneEngineTools::Update( UpdateContext const& context, Render::ViewportManager& viewportSystem )
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
                    ImGuiX::OrientationGuide::DrawAsStandaloneWindow( *viewportSystem.GetPrimaryViewport(), ImVec2( 3.0f, 4.0f + StatusBar::g_height ) );
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

    //-------------------------------------------------------------------------
    // Drawing
    //-------------------------------------------------------------------------

    void StandaloneEngineTools::DrawPopups( UpdateContext const& context )
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

    void StandaloneEngineTools::DrawOverlayMenu( UpdateContext const& context )
    {
        if ( ImGui::BeginMainMenuBar() )
        {
            ImGui::TextColored( ImGuiX::Theme::s_itemColorLight, KRG_ICON_BUG );
            m_pWorldDebugger->DrawMenu( context );
            ImGui::EndMainMenuBar();
        }
    }

    void StandaloneEngineTools::DrawDebugWindows( UpdateContext const& context )
    {
        m_pWorldDebugger->DrawWindows( context );
    }

    void StandaloneEngineTools::DrawOverlayStatusBar( UpdateContext const& context )
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
}
#endif