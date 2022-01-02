#include "DebugView_Player.h"
#include "Game/Core/Player/Systems/EntitySystem_PlayerController.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "Engine/Core/Systems/WorldSystem_PlayerManager.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"
#include "Engine/Physics/Debug/DebugView_Physics.h"

// HACK
#include "Game/Core/Player/Actions/PlayerAction_Jump.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Player
{
    PlayerDebugView::PlayerDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Player", [this] ( EntityWorldUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void PlayerDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorld = pWorld;
        m_pPlayerManager = pWorld->GetWorldSystem<PlayerManager>();
    }

    void PlayerDebugView::Shutdown()
    {
        m_pPlayerManager = nullptr;
        m_pWorld = nullptr;
    }

    void PlayerDebugView::DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        PlayerController* pPlayerController = nullptr;
        if ( m_isActionDebuggerWindowOpen || m_isPhysicsStateDebuggerWindowOpen )
        {
            if ( m_pPlayerManager->HasPlayer() )
            {
                auto pPlayerEntity = m_pWorld->GetPersistentMap()->FindEntity( m_pPlayerManager->GetPlayerEntityID() );
                for ( auto pSystem : pPlayerEntity->GetSystems() )
                {
                    pPlayerController = TryCast<PlayerController>( pSystem );
                    if ( pPlayerController != nullptr )
                    {
                        break;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        if ( pPlayerController != nullptr )
        {
            if ( m_isActionDebuggerWindowOpen )
            {
                if ( pWindowClass != nullptr ) ImGui::SetNextWindowClass( pWindowClass );
                DrawActionDebuggerWindow( context, pPlayerController );
            }

            if ( m_isPhysicsStateDebuggerWindowOpen )
            {
                if ( pWindowClass != nullptr ) ImGui::SetNextWindowClass( pWindowClass );
                DrawPhysicsStateDebuggerWindow( context, pPlayerController );
            }
        }
    }

    void PlayerDebugView::DrawMenu( EntityWorldUpdateContext const& context )
    {
        PlayerController* pPlayerController = nullptr;
        if ( m_pPlayerManager->HasPlayer() )
        {
            auto pPlayerEntity = m_pWorld->GetPersistentMap()->FindEntity( m_pPlayerManager->GetPlayerEntityID() );
            for ( auto pSystem : pPlayerEntity->GetSystems() )
            {
                pPlayerController = TryCast<PlayerController>( pSystem );
                if ( pPlayerController != nullptr )
                {
                    break;
                }
            }
        }

        if ( pPlayerController == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( ImGui::MenuItem( "Action Debugger" ) )
        {
            m_isActionDebuggerWindowOpen = true;
        }

        if ( ImGui::MenuItem( "Physics State Debugger" ) )
        {
            m_isPhysicsStateDebuggerWindowOpen = true;
        }
    }

    void PlayerDebugView::DrawActionDebuggerWindow( EntityWorldUpdateContext const& context, PlayerController const* pPlayerController )
    {
        KRG_ASSERT( pPlayerController != nullptr );

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowBgAlpha( 0.9f );
        if ( ImGui::Begin( "Player Actions", &m_isActionDebuggerWindowOpen ) )
        {
            if ( pPlayerController == nullptr )
            {
                ImGui::Text( "No Player Controller Found" );
            }
            else
            {
                ActionStateMachine const* pStateMachine = &pPlayerController->m_actionStateMachine;

                if ( ImGui::BeginTable( "DebuggerLayout", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings, ImGui::GetContentRegionAvail() ) )
                {
                    ImGui::TableSetupColumn( "CurrentState", ImGuiTableColumnFlags_WidthStretch );
                    ImGui::TableSetupColumn( "HistoryLog", ImGuiTableColumnFlags_WidthStretch );

                    ImGui::TableNextRow();

                    // Current State
                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 0 );

                    if ( ImGui::CollapsingHeader( "Overlay Actions", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        if ( ImGui::BeginTable( "OverlayActionsTable", 2, ImGuiTableFlags_Borders ) )
                        {
                            ImGui::TableSetupColumn( "Action", ImGuiTableColumnFlags_WidthStretch );
                            ImGui::TableSetupColumn( "##State", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 16 );
                            ImGui::TableHeadersRow();

                            for ( auto pOverlayAction : pStateMachine->m_overlayActions )
                            {
                                ImVec4 const color = pOverlayAction->IsActive() ? Colors::LimeGreen.ToFloat4() : (ImVec4) ImGuiX::Style::s_textColor;

                                ImGui::TableNextRow();

                                ImGui::TableSetColumnIndex( 0 );
                                ImGui::TextColored( color, "%s", pOverlayAction->GetName() );

                                ImGui::TableSetColumnIndex( 1 );
                                ImGui::TextColored( color, pOverlayAction->IsActive() ? KRG_ICON_CHECK : KRG_ICON_TIMES );
                            }

                            ImGui::EndTable();
                        }
                    }

                    Action* pBaseAction = ( pStateMachine->m_activeBaseActionID != ActionStateMachine::InvalidAction ) ? pStateMachine->m_baseActions[pStateMachine->m_activeBaseActionID] : nullptr;
                    TInlineString<50> const headerString( TInlineString<50>::CtorSprintf(), "Base Action: %s###BaseActionHeader", ( pBaseAction != nullptr ) ? pBaseAction->GetName() : "None" );

                    if ( ImGui::CollapsingHeader( headerString.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        if ( pBaseAction != nullptr )
                        {
                            pBaseAction->DrawDebugUI();
                        }
                    }

                    //-------------------------------------------------------------------------
                    // History
                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 1 );

                    if ( ImGui::BeginTable( "ActionHistoryTable", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders, ImGui::GetContentRegionAvail() - ImGui::GetStyle().WindowPadding ) )
                    {
                        ImGui::TableSetupColumn( "Frame", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 36 );
                        ImGui::TableSetupColumn( "Action", ImGuiTableColumnFlags_WidthStretch );
                        ImGui::TableSetupColumn( "Status", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 100 );
                        ImGui::TableHeadersRow();

                        for ( auto const& logEntry : pStateMachine->m_actionLog )
                        {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex( 0 );
                            ImGui::Text( "%u", logEntry.m_frameID );

                            ImGui::TableSetColumnIndex( 1 );
                            ImGui::Text( logEntry.m_pActionName );

                            ImGui::TableSetColumnIndex( 2 );
                            switch ( logEntry.m_status )
                            {
                                case ActionStateMachine::LoggedStatus::ActionStarted:
                                {
                                    ImGui::TextColored( ImGuiX::ConvertColor( Colors::LimeGreen ), "Started" );
                                }
                                break;

                                case ActionStateMachine::LoggedStatus::ActionCompleted:
                                {
                                    ImGui::TextColored( ImGuiX::ConvertColor( Colors::White ), "Completed" );
                                }
                                break;

                                case ActionStateMachine::LoggedStatus::ActionInterrupted:
                                {
                                    ImGui::TextColored( ImGuiX::ConvertColor( Colors::Red ), "Interrupted" );
                                }
                                break;

                                default:
                                {
                                    KRG_UNREACHABLE_CODE();
                                }
                                break;
                            }
                        }

                        // Auto scroll the table
                        if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
                        {
                            ImGui::SetScrollHereY( 1.0f );
                        }

                        ImGui::EndTable();
                    }


                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();
    }

    void PlayerDebugView::DrawPhysicsStateDebuggerWindow( EntityWorldUpdateContext const& context, PlayerController const* pPlayerController )
    {
        KRG_ASSERT( pPlayerController != nullptr );

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowBgAlpha( 0.9f );
        if ( ImGui::Begin( "Player Physics States", &m_isPhysicsStateDebuggerWindowOpen ) )
        {
            if ( pPlayerController->m_actionContext.m_pCharacterController != nullptr )
            {
                // TODO: Debug...
            }
            else
            {
                ImGui::Text( "No valid physics controller on player!" );
            }
        }
        ImGui::End();
    }

    // Called within the context of a large overlay window allowing you to draw helpers and widgets over a viewport
    void PlayerDebugView::DrawOverlayElements( EntityWorldUpdateContext const& context )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        PlayerController* pPlayerController = nullptr;

        if ( m_pPlayerManager->HasPlayer() )
        {
            auto pPlayerEntity = m_pWorld->GetPersistentMap()->FindEntity( m_pPlayerManager->GetPlayerEntityID() );
            for ( auto pSystem : pPlayerEntity->GetSystems() )
            {
                pPlayerController = TryCast<PlayerController>( pSystem );
                if ( pPlayerController != nullptr )
                {
                    break;
                }
            }
        }

        if ( pPlayerController == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( !m_pPlayerManager->IsDebugCameraEnabled() )
        {
            HACK_DrawPlayerHUD( context, pPlayerController );
        }
    }

    void PlayerDebugView::HACK_DrawPlayerHUD( EntityWorldUpdateContext const& context, PlayerController* pController )
    {
        InlineString statusString;

        bool const isChargedJumpReady = static_cast<JumpAction const*>( pController->m_actionStateMachine.m_baseActions[ActionStateMachine::Jump] )->IsChargedJumpReady();
        if ( isChargedJumpReady )
        {
            statusString += "Charged Jump Ready";
        }

        //-------------------------------------------------------------------------

        ImVec2 const windowDimensions = ImGui::GetContentRegionAvail();
        ImVec2 const windowCenter = windowDimensions / 2;

        {
            ImGuiX::ScopedFont sf( ImGuiX::Font::Medium );

            ImVec2 const crosshairSize = ImGui::CalcTextSize( KRG_ICON_CIRCLE_O );
            ImVec2 const crosshairCursorPos( windowCenter - ( crosshairSize / 2 ) );
            ImGui::SetCursorPos( crosshairCursorPos );
            ImGui::Text( KRG_ICON_CIRCLE_O );
        }

        if ( !statusString.empty() )
        {
            ImGuiX::ScopedFont sf( ImGuiX::Font::LargeBold );

            ImVec2 const statusTextSize = ImGui::CalcTextSize( statusString.c_str() );
            ImVec2 const statusTextCursorPos( windowCenter.x - ( statusTextSize.x / 2 ), windowCenter.y + 8 );
            ImGui::SetCursorPos( statusTextCursorPos );
            ImGui::TextColored( ImGuiX::ConvertColor( Colors::Red ), statusString.c_str() );
        }
    }
}
#endif