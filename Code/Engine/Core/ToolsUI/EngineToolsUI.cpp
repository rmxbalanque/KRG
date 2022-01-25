#include "EngineToolsUI.h"
#include "OrientationGuide.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/Entity/EntityWorldDebugger.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Systems/WorldSystem_PlayerManager.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    constexpr static float const g_menuHeight = 19.0f;
    constexpr static float const g_minTimeScaleValue = 0.1f;
    constexpr static float const g_maxTimeScaleValue = 3.5f;

    //-------------------------------------------------------------------------

    void EngineToolsUI::Initialize( UpdateContext const& context )
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

    void EngineToolsUI::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pWorldDebugger );
        m_pWorldManager = nullptr;
    }

    //-------------------------------------------------------------------------

    void EngineToolsUI::EndFrame( UpdateContext const& context )
    {
        UpdateStage const updateStage = context.GetUpdateStage();
        KRG_ASSERT( updateStage == UpdateStage::FrameEnd );

        // Update internal state
        //-------------------------------------------------------------------------

        float const k = 2.0f / ( context.GetFrameID() + 1 );
        m_averageDeltaTime = context.GetDeltaTime() * k + m_averageDeltaTime * ( 1.0f - k );

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

        // Process user input
        //-------------------------------------------------------------------------

        HandleUserInput( context, pGameWorld );

        // Draw overlay window
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
                DrawMenu( context, pGameWorld );
            }
        }
        ImGui::End();

        // The debug windows should be always be drawn if enabled
        DrawWindows( context, pGameWorld );

        // Always show the log if any errors/warnings occurred
        auto const unhandledWarningsAndErrors = Log::GetUnhandledWarningsAndErrors();
        if ( !unhandledWarningsAndErrors.empty() )
        {
            m_isLogWindowOpen = true;
        }
    }

    //-------------------------------------------------------------------------
    // Drawing
    //-------------------------------------------------------------------------

    void EngineToolsUI::DrawMenu( UpdateContext const& context, EntityWorld* pGameWorld )
    {
        if ( ImGui::BeginMenuBar() )
        {
            ImVec2 const totalAvailableSpace = ImGui::GetContentRegionAvail();

            float const currentFPS = 1.0f / context.GetDeltaTime();
            float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;

            TInlineString<10> const warningsStr( TInlineString<10>::CtorSprintf(), KRG_ICON_QUESTION_CIRCLE" %d", Log::GetNumWarnings() );
            TInlineString<10> const errorsStr( TInlineString<10>::CtorSprintf(), KRG_ICON_EXCLAMATION_CIRCLE" %d", Log::GetNumErrors() );
            TInlineString<40> const perfStatsStr( TInlineString<40>::CtorSprintf(), "FPS: %3.0f Mem: %.2fMB", currentFPS, allocatedMemory );

            ImVec2 const warningsTextSize = ImGui::CalcTextSize( warningsStr.c_str() );
            ImVec2 const errorTextSize = ImGui::CalcTextSize( errorsStr.c_str() );
            ImVec2 const perfStatsTextSize = ImGui::CalcTextSize( perfStatsStr.c_str() );

            float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
            float const framePadding = ImGui::GetStyle().FramePadding.x;
            float const perfStatsOffset = totalAvailableSpace.x - perfStatsTextSize.x - ( itemSpacing * 2 );
            float const warningsAndErrorsOffset = perfStatsOffset - warningsTextSize.x - errorTextSize.x - ( itemSpacing * 3 ) - ( framePadding * 4 );
            float const debugCameraOffset = warningsAndErrorsOffset - 30;

            //-------------------------------------------------------------------------

            ImGui::PushStyleColor( ImGuiCol_Text, Colors::LimeGreen.ToFloat4() );
            bool const drawDebugMenu = ImGui::BeginMenu( KRG_ICON_BUG );
            ImGui::PopStyleColor();
            if ( drawDebugMenu )
            {
                if ( ImGui::MenuItem( KRG_ICON_SLIDERS_H" Show Debug Settings", nullptr, &m_isDebugSettingsWindowOpen ) )
                {
                    m_isDebugSettingsWindowOpen = true;
                }

                if ( ImGui::MenuItem( KRG_ICON_CLOCK" Show Time Controls", nullptr, &m_isTimeControlWindowOpen ) )
                {
                    m_isTimeControlWindowOpen = true;
                }

                ImGui::EndMenu();
            }

            // Draw world debuggers
            //-------------------------------------------------------------------------

            m_pWorldDebugger->DrawMenu( context );

            // Camera controls
            //-------------------------------------------------------------------------

            ImGui::SameLine( debugCameraOffset, 0 );

            auto pPlayerManager = pGameWorld->GetWorldSystem<PlayerManager>();
            bool const debugCamEnabled = pPlayerManager->GetDebugMode() == PlayerManager::DebugMode::OnlyDebugCamera;
            if ( debugCamEnabled )
            {
                if ( ImGuiX::FlatButton( KRG_ICON_VIDEO"##DisableDebugCam" ) )
                {
                    pPlayerManager->SetDebugMode( PlayerManager::DebugMode::PlayerWithDebugCamera );
                }
            }
            else
            {
                if ( ImGuiX::FlatButton( KRG_ICON_VIDEO_SLASH"##EnableDebugCam") )
                {
                    pPlayerManager->SetDebugMode( PlayerManager::DebugMode::OnlyDebugCamera );
                }
            }

            // Log
            //-------------------------------------------------------------------------

            ImGuiX::VerticalSeparator();

            if ( ImGuiX::FlatButton( warningsStr.c_str() ) )
            {
                m_isLogWindowOpen = true;
                m_systemLogView.m_showLogMessages = false;
                m_systemLogView.m_showLogWarnings = true;
                m_systemLogView.m_showLogErrors = false;
            }

            ImGuiX::VerticalSeparator();

            if ( ImGuiX::FlatButton( errorsStr.c_str() ) )
            {
                m_isLogWindowOpen = true;
                m_systemLogView.m_showLogMessages = false;
                m_systemLogView.m_showLogWarnings = false;
                m_systemLogView.m_showLogErrors = true;
            }

            ImGuiX::VerticalSeparator();

            // Draw Performance Stats
            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, 8 );
            ImGui::Text( perfStatsStr.c_str() );

            ImGui::EndMenuBar();
        }
    }

    void EngineToolsUI::DrawOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        m_pWorldDebugger->DrawOverlayElements( context );

        if ( m_debugOverlayEnabled )
        {
            ImVec2 const guidePosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax() - ( ImGuiX::OrientationGuide::GetSize() / 2 );
            ImGuiX::OrientationGuide::Draw( guidePosition, *pViewport );
        }
    }

    void EngineToolsUI::DrawWindows( UpdateContext const& context, EntityWorld* pGameWorld, ImGuiWindowClass* pWindowClass )
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
            m_isDebugSettingsWindowOpen = SystemDebugView::DrawDebugSettingsView( context );
        }

        //-------------------------------------------------------------------------

        if ( m_isTimeControlWindowOpen )
        {
            ImGui::SetNextWindowSizeConstraints( ImVec2( 100, 54 ), ImVec2( FLT_MAX, 54 ) );
            ImGui::SetNextWindowBgAlpha( 0.75f );
            if ( ImGui::Begin( "Time Controls", &m_isTimeControlWindowOpen, ImGuiWindowFlags_NoScrollbar ) )
            {
                ImVec2 const buttonSize( 24, 0 );

                // Play/Pause
                if ( pGameWorld->IsPaused() )
                {
                    if ( ImGui::Button( KRG_ICON_PLAY"##ResumeWorld", buttonSize ) )
                    {
                        ToggleWorldPause( pGameWorld );
                    }
                    ImGuiX::ItemTooltip( "Resume" );
                }
                else
                {
                    if ( ImGui::Button( KRG_ICON_PAUSE"##PauseWorld", buttonSize ) )
                    {
                        ToggleWorldPause( pGameWorld );
                    }
                    ImGuiX::ItemTooltip( "Pause" );
                }

                // Step
                ImGui::SameLine();
                ImGui::BeginDisabled( !pGameWorld->IsPaused() );
                if ( ImGui::Button( KRG_ICON_STEP_FORWARD"##StepFrame", buttonSize ) )
                {
                    RequestWorldTimeStep( pGameWorld );
                }
                ImGuiX::ItemTooltip( "Step Frame" );
                ImGui::EndDisabled();

                // Slider
                ImGui::SameLine();
                ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - buttonSize.x - ImGui::GetStyle().ItemSpacing.x );
                float currentTimeScale = m_timeScale;
                if ( ImGui::SliderFloat( "##TimeScale", &currentTimeScale, g_minTimeScaleValue, g_maxTimeScaleValue, "%.2f", ImGuiSliderFlags_NoInput ) )
                {
                    SetWorldTimeScale( pGameWorld, currentTimeScale );
                }
                ImGuiX::ItemTooltip( "Time Scale" );

                // Reset
                ImGui::SameLine();
                if ( ImGui::Button( KRG_ICON_UNDO"##ResetTimeScale", buttonSize ) )
                {
                    ResetWorldTimeScale( pGameWorld );
                }
                ImGuiX::ItemTooltip( "Reset TimeScale" );
            }
            ImGui::End();
        }
    }

    //-------------------------------------------------------------------------

    void EngineToolsUI::HandleUserInput( UpdateContext const& context, EntityWorld* pGameWorld )
    {
        auto pInputSystem = context.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );

        // Enable/disable debug overlay
        //-------------------------------------------------------------------------

        auto const pKeyboardState = pInputSystem->GetKeyboardState();
        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_Tilde ) )
        {
            m_debugOverlayEnabled = !m_debugOverlayEnabled;

            auto pPlayerManager = pGameWorld->GetWorldSystem<PlayerManager>();
            pPlayerManager->SetDebugMode( m_debugOverlayEnabled ? PlayerManager::DebugMode::OnlyDebugCamera : PlayerManager::DebugMode::None );
        }

        // Time Controls
        //-------------------------------------------------------------------------

        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_Pause ) )
        {
            ToggleWorldPause( pGameWorld );
        }

        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_PageUp ) )
        {
            SetWorldTimeScale( pGameWorld, m_timeScale + 0.1f );
        }

        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_PageDown ) )
        {
            SetWorldTimeScale( pGameWorld, m_timeScale - 0.1f );
        }

        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_Home ) )
        {
            ResetWorldTimeScale( pGameWorld );
        }

        if ( pKeyboardState->WasReleased( Input::KeyboardButton::Key_End ) )
        {
            RequestWorldTimeStep( pGameWorld );
        }
    }

    void EngineToolsUI::ToggleWorldPause( EntityWorld* pGameWorld )
    {
        // Unpause
        if ( pGameWorld->IsPaused() )
        {
            pGameWorld->SetTimeScale( m_timeScale );
        }
        else // Pause
        {
            m_timeScale = pGameWorld->GetTimeScale();
            pGameWorld->SetTimeScale( -1.0f );
        }
    }

    void EngineToolsUI::SetWorldTimeScale( EntityWorld* pGameWorld, float newTimeScale )
    {
        m_timeScale = Math::Clamp( newTimeScale, g_minTimeScaleValue, g_maxTimeScaleValue );
        pGameWorld->SetTimeScale( m_timeScale );
    }

    void EngineToolsUI::ResetWorldTimeScale( EntityWorld* pGameWorld )
    {
        m_timeScale = 1.0f;

        if ( !pGameWorld->IsPaused() )
        {
            pGameWorld->SetTimeScale( m_timeScale );
        }
    }

    void EngineToolsUI::RequestWorldTimeStep( EntityWorld* pGameWorld )
    {
        if ( pGameWorld->IsPaused() )
        {
            pGameWorld->RequestTimeStep();
        }
    }
}
#endif