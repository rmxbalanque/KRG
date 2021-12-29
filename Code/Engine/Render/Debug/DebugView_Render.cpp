#include "DebugView_Render.h"
#include "Engine/Render/Systems/WorldSystem_WorldRenderer.h"
#include "Engine/Render/Components/Component_Lights.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Render
{
    void RenderDebugView::DrawRenderVisualizationModesMenu( EntityWorld const* pWorld )
    {
        KRG_ASSERT( pWorld != nullptr );
        auto pWorldRendererSystem = pWorld->GetWorldSystem<Render::RendererWorldSystem>();

        int32 debugMode = (int32) pWorldRendererSystem->GetVisualizationMode();

        bool stateUpdated = false;
        stateUpdated |= ImGui::RadioButton( "Render Lighting", &debugMode, (int32) RendererWorldSystem::VisualizationMode::Lighting );
        stateUpdated |= ImGui::RadioButton( "Render Albedo", &debugMode, (int32) RendererWorldSystem::VisualizationMode::Albedo );
        stateUpdated |= ImGui::RadioButton( "Render Normals", &debugMode, (int32) RendererWorldSystem::VisualizationMode::Normals );
        stateUpdated |= ImGui::RadioButton( "Render Metalness", &debugMode, (int32) RendererWorldSystem::VisualizationMode::Metalness );
        stateUpdated |= ImGui::RadioButton( "Render Roughness", &debugMode, (int32) RendererWorldSystem::VisualizationMode::Roughness );
        stateUpdated |= ImGui::RadioButton( "Render Ambient Occlusion", &debugMode, (int32) RendererWorldSystem::VisualizationMode::AmbientOcclusion );

        if ( stateUpdated )
        {
            pWorldRendererSystem->SetVisualizationMode( (RendererWorldSystem::VisualizationMode) debugMode );
        }
    }

    //-------------------------------------------------------------------------

    RenderDebugView::RenderDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Render", [this] ( EntityUpdateContext const& context ) { DrawRenderMenu( context ); } ) );
    }

    void RenderDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        EntityWorldDebugView::Initialize( systemRegistry, pWorld );
        m_pWorldRendererSystem = pWorld->GetWorldSystem<RendererWorldSystem>();
    }

    void RenderDebugView::Shutdown()
    {
        m_pWorldRendererSystem = nullptr;
        EntityWorldDebugView::Shutdown();
    }

    void RenderDebugView::DrawRenderMenu( EntityUpdateContext const& context )
    {
        DrawRenderVisualizationModesMenu( m_pWorld );
    }

    void RenderDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
    }

    void RenderDebugView::DrawOverlayElements( EntityUpdateContext const& context )
    {
    }
}
#endif