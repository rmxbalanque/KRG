#include "StandaloneEngineTools.h"
#include "Engine/Core/Imgui/OrientationGuide.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/Entity/Debug/EntityWorldDebugger.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    constexpr static float const g_menuHeight = 19.0f;
    constexpr static float const g_statusBarHeight = 19.0f;

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

    void StandaloneEngineTools::FrameEndUpdate( UpdateContext const& context )
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

        // Enable/disable debug overlay
        //-------------------------------------------------------------------------

        auto const pKeyboardState = pInputSystem->GetKeyboardState();
        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_Tilde ) )
        {
            m_debugOverlayEnabled = !m_debugOverlayEnabled;
        }

        //-------------------------------------------------------------------------

        Render::Viewport const* pViewport = m_pWorldManager->GetPrimaryWorld()->GetViewport();

        ImGui::SetNextWindowPos( ImVec2( 0, g_menuHeight ) );
        ImGui::SetNextWindowSize( pViewport->GetDimensions() - Float2( 0, g_statusBarHeight + g_menuHeight ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::SetNextWindowBgAlpha( 0.0f );
        if ( ImGui::Begin( "ViewportOverlay", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus ) )
        {
            // The overlay elements should always be drawn
            DrawOverlayElements( context, pViewport );
        }
        ImGui::End();
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        if ( m_debugOverlayEnabled )
        {
            DrawMenu( context );
            DrawStatusBar( context );
        }

        // The debug windows should be always be drawn if enabled
        DrawWindows( context );

        // The pop-ups should be always be drawn if enabled
        DrawPopups( context );
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

    void StandaloneEngineTools::DrawMenu( UpdateContext const& context )
    {
        if ( ImGui::BeginMainMenuBar() )
        {
            // Draw Debug Menu
            //-------------------------------------------------------------------------

            ImGui::TextColored( ImGuiX::ConvertColor( Colors::LimeGreen ), KRG_ICON_BUG );
            m_pWorldDebugger->DrawMenu( context );

            // Draw Performance Stats
            //-------------------------------------------------------------------------

            float const currentFPS = 1.0f / context.GetDeltaTime();
            float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
            InlineString<200> perfStatStr;
            perfStatStr.sprintf( "FPS: %3.0f Mem: %.2fMB", currentFPS, allocatedMemory );

            ImGuiViewport const* viewport = ImGui::GetMainViewport();
            ImVec2 const textSize = ImGui::CalcTextSize( perfStatStr.c_str() );
            ImGui::SameLine( viewport->WorkSize.x - textSize.x - 8.0f );

            ImGui::Text( perfStatStr.c_str() );
            ImGui::EndMainMenuBar();
        }
    }

    void StandaloneEngineTools::DrawOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        m_pWorldDebugger->DrawOverlayElements( context );

        if ( m_debugOverlayEnabled )
        {
            ImVec2 const guidePosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax() - ( ImGuiX::OrientationGuide::GetSize() / 2 );
            ImGuiX::OrientationGuide::Draw( guidePosition, *pViewport );
        }
    }

    void StandaloneEngineTools::DrawWindows( UpdateContext const& context )
    {
        m_pWorldDebugger->DrawWindows( context );

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

    void StandaloneEngineTools::DrawStatusBar( UpdateContext const& context )
    {
        bool showAlways = true;
        uint32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos( ImVec2( 0, io.DisplaySize.y - g_statusBarHeight ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 2.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 4.0f, 1.0f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

        ImGui::SetNextWindowBgAlpha( 0.85f );
        ImGui::SetNextWindowSize( ImVec2( io.DisplaySize.x, g_statusBarHeight ) );
        if ( ImGui::Begin( "Stats", &showAlways, flags ) )
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
        ImGui::End();
        ImGui::PopStyleVar( 3 );
    }
}
#endif