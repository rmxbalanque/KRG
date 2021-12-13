#include "DebugView_AI.h"
#include "Game/Core/AI/Systems/WorldSystem_AIManager.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::AI
{
    AIDebugView::AIDebugView()
    {
        m_menus.emplace_back( DebugMenu( "AI", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void AIDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorld = pWorld;
        m_pAIManager = pWorld->GetWorldSystem<AIManager>();
    }

    void AIDebugView::Shutdown()
    {
        m_pAIManager = nullptr;
        m_pWorld = nullptr;
    }

    void AIDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        if ( m_isOverviewWindowOpen )
        {
            if ( pWindowClass != nullptr ) ImGui::SetNextWindowClass( pWindowClass );
            DrawOverviewWindow( context );
        }
    }

    void AIDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        ImGui::Text( "Num AI: %u", m_pAIManager->m_AIs.size() );

        if ( ImGui::MenuItem( "Overview" ) )
        {
            m_isOverviewWindowOpen = true;
        }

        //-------------------------------------------------------------------------

        if ( ImGui::Button( "Hack Spawn" ) )
        {
            m_pAIManager->TrySpawnAI( context );
        }
    }

    void AIDebugView::DrawOverviewWindow( EntityUpdateContext const& context )
    {
        if ( ImGui::Begin( "AI Overview", &m_isOverviewWindowOpen ) )
        {
            ImGui::Text( "Num AI: %u", m_pAIManager->m_AIs.size() );
        }
        ImGui::End();
    }
}
#endif