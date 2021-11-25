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
        m_menus.emplace_back( DebugMenu( "Render", [this] ( EntityUpdateContext const& context ) { DrawRenderMenu( context ); } ) );
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
        bool stateUpdated = false;

        stateUpdated |= ImGui::RadioButton( "Render Lighting", &m_debugMode, WorldRendererSystem::VIS_MODE_LIGHTING );
        stateUpdated |= ImGui::RadioButton( "Render Albedo", &m_debugMode, WorldRendererSystem::VIS_MODE_ALBEDO );
        stateUpdated |= ImGui::RadioButton( "Render Normals", &m_debugMode, WorldRendererSystem::VIS_MODE_NORMALS );
        stateUpdated |= ImGui::RadioButton( "Render Metalness", &m_debugMode, WorldRendererSystem::VIS_MODE_METALNESS );
        stateUpdated |= ImGui::RadioButton( "Render Roughness", &m_debugMode, WorldRendererSystem::VIS_MODE_ROUGHNESS );
        stateUpdated |= ImGui::RadioButton( "Render AO", &m_debugMode, WorldRendererSystem::VIS_MODE_AO );

        if ( stateUpdated )
        {
            m_pWorldRendererSystem->SetVisualizationMode( m_debugMode );
        }
    }

    void RenderDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
    }

    void RenderDebugView::DrawOverlayElements( EntityUpdateContext const& context )
    {
        
    }
}
#endif