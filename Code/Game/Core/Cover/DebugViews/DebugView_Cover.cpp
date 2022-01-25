#include "DebugView_Cover.h"
#include "Game/Core/Cover/Systems/WorldSystem_CoverManager.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    CoverDebugView::CoverDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Gameplay/Covers", [this] ( EntityWorldUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void CoverDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorld = pWorld;
        m_pCoverManager = pWorld->GetWorldSystem<CoverManager>();
    }

    void CoverDebugView::Shutdown()
    {
        m_pCoverManager = nullptr;
        m_pWorld = nullptr;
    }

    void CoverDebugView::DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        if ( m_isOverviewWindowOpen )
        {
            if ( pWindowClass != nullptr ) ImGui::SetNextWindowClass( pWindowClass );
            DrawOverviewWindow( context );
        }
    }

    void CoverDebugView::DrawMenu( EntityWorldUpdateContext const& context )
    {
       
    }

    void CoverDebugView::DrawOverviewWindow( EntityWorldUpdateContext const& context )
    {
       
    }
}
#endif