#include "DebugView_Render.h"
#include "Engine/Render/WorldSystems/WorldRendererSystem.h"
#include "Engine/Render/Components/LightComponents.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Render
{
    RenderDebugView::RenderDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Debug Options", "Render", [this] ( EntityUpdateContext const& context ) { DrawRenderMenu( context ); } ) );
    }

    void RenderDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorldRendererSystem = pWorld->GetWorldSystem<WorldRendererSystem>();
    }

    void RenderDebugView::Shutdown()
    {
        m_pWorldRendererSystem = nullptr;
    }

    //-------------------------------------------------------------------------

    void RenderDebugView::DrawRenderMenu( EntityUpdateContext const& context )
    {
    }

    void RenderDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
    }

    void RenderDebugView::DrawOverlayElements( EntityUpdateContext const& context )
    {
        
    }
}
#endif