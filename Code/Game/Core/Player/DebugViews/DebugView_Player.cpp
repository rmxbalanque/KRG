#include "DebugView_Player.h"
#include "Game/Core/Player/Systems/EntitySystem_PlayerController.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "Engine/Core/Systems/WorldSystem_PlayerManager.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Player
{
    PlayerDebugView::PlayerDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Player", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
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

    void PlayerDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        if ( m_isActionDebuggerWindowOpen )
        {
            if ( pWindowClass != nullptr ) ImGui::SetNextWindowClass( pWindowClass );
            DrawActionDebuggerWindow( context );
        }
    }

    void PlayerDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        if ( ImGui::MenuItem( "Actions" ) )
        {
            m_isActionDebuggerWindowOpen = true;
        }
    }

    void PlayerDebugView::DrawActionDebuggerWindow( EntityUpdateContext const& context )
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

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "Player Actions", &m_isActionDebuggerWindowOpen ) )
        {
            if ( pPlayerController == nullptr )
            {
                ImGui::Text( "No Player Controller Found" );
            }
            else
            {
                for ( auto pBaseAction : pPlayerController->m_baseActions )
                {
                    ImVec4 const color = pBaseAction->IsActive() ? Colors::LimeGreen.ToFloat4() : (ImVec4) ImGuiX::Style::s_textColor;
                    ImGui::TextColored( color, "%s", pBaseAction->GetName() );
                }

                for ( auto pOverlayAction : pPlayerController->m_overlayActions )
                {
                    ImVec4 const color = pOverlayAction->IsActive() ? Colors::LimeGreen.ToFloat4() : (ImVec4) ImGuiX::Style::s_textColor;
                    ImGui::TextColored( color, "%s", pOverlayAction->GetName() );
                }
            }
        }
        ImGui::End();
    }
}
#endif