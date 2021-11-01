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
        if ( ImGui::Button( "Show Light Debugger" ) )
        {
            m_isLightDebuggerWindowOpen = true;
        }
    }

    void RenderDebugView::DrawWindows( EntityUpdateContext const& context )
    {
        if ( m_isLightDebuggerWindowOpen )
        {
            DrawLightDebuggerWindow( context );
        }
    }

    //-------------------------------------------------------------------------

    void RenderDebugView::DrawLightDebuggerWindow( EntityUpdateContext const& context )
    {
        ImGui::SetNextWindowBgAlpha( 0.5f );
        if ( ImGui::Begin( "Light Components", &m_isLightDebuggerWindowOpen ) )
        {
            for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredDirectionLightComponents.GetRecords() )
            {
                DirectionalLightComponent* pLightComponent = lightComponentRecord.m_pComponent;
                ImGui::Text( pLightComponent->GetName().c_str() );
            }

            for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredPointLightComponents.GetRecords() )
            {
                PointLightComponent* pLightComponent = lightComponentRecord.m_pComponent;
                ImGui::Text( pLightComponent->GetName().c_str() );
            }

            for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredSpotLightComponents.GetRecords() )
            {
                SpotLightComponent* pLightComponent = lightComponentRecord.m_pComponent;
                ImGui::Text( pLightComponent->GetName().c_str() );
            }
        }
        ImGui::End();
    }
}
#endif