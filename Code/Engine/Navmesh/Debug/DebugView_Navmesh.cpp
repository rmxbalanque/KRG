#include "DebugView_Navmesh.h"
#include "Engine/Navmesh/NavmeshSystem.h"
#include "Engine/Navmesh/NavmeshWorldSystem.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Navmesh
{
    NavmeshDebugView::NavmeshDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Debug Options", "Navmesh", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void NavmeshDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pNavmeshSystem = systemRegistry.GetSystem<NavmeshSystem>();
        m_pNavmeshWorldSystem = pWorld->GetWorldSystem<NavmeshWorldSystem>();
    }

    void NavmeshDebugView::Shutdown()
    {
        m_pNavmeshSystem = nullptr;
        m_pNavmeshWorldSystem = nullptr;
    }

    //-------------------------------------------------------------------------

    void NavmeshDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        #if KRG_ENABLE_NAVPOWER
        auto CreateCheckboxForFlag = [] ( char const* pLabel, bfx::PlannerDebugFlag flag )
        {
            bool isEnabled = bfx::GetGlobalDebugFlag( flag );
            if ( ImGui::Checkbox( pLabel, &isEnabled ) )
            {
                bfx::SetGlobalDebugFlag( flag, isEnabled );
            }
        };

        //-------------------------------------------------------------------------

        bool isDrawingNavpowerStats = bfx::GetGlobalDebugFlag( bfx::BFX_DRAW_STATS );
        if ( ImGui::Checkbox( "Draw NavPower Stats", &isDrawingNavpowerStats ) )
        {
            bfx::SetGlobalDebugFlag( bfx::BFX_DRAW_STATS, isDrawingNavpowerStats );
        }

        //-------------------------------------------------------------------------

        ImGui::Separator();

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Areas" ) )
        {
            CreateCheckboxForFlag( "Draw Area Edges", bfx::BFX_DRAW_PLANNER_AREAS );
            CreateCheckboxForFlag( "Draw Area Normals", bfx::BFX_DRAW_AREA_NORMALS );
            CreateCheckboxForFlag( "Draw Areas", bfx::BFX_DRAW_PLANNER_AREAS_SOLID );
            CreateCheckboxForFlag( "Draw Area Connectivity", bfx::BFX_DRAW_PLANNER_CONNECTIVITY );

            CreateCheckboxForFlag( "Draw Area Usage Flags", bfx::BFX_DRAW_AREA_USAGE_FLAGS );
            CreateCheckboxForFlag( "Draw Area Penalties", bfx::BFX_DRAW_AREA_PENALTY_MULTS );
            CreateCheckboxForFlag( "Draw Area Sharing Penalties", bfx::BFX_DRAW_SHARING_PENALTY_COUNTS );

            CreateCheckboxForFlag( "Colorize Areas Usage Flags", bfx::BFX_COLORIZE_AREA_USAGE_FLAGS );

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Obstacles" ) )
        {
            CreateCheckboxForFlag( "Draw Obstacles", bfx::BFX_DRAW_OBSTACLES );
            CreateCheckboxForFlag( "Draw Obstacle Flags", bfx::BFX_DRAW_OBSTACLE_FLAGS );

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Drawing Options" ) )
        {
            /*bool isDepthTestEnabled = m_pNavmeshWorldSystem->IsDepthTestEnabled();
            if ( ImGui::Checkbox( "Enable Depth Test", &isDepthTestEnabled ) )
            {
                m_pNavmeshWorldSystem->SetDepthTestState( isDepthTestEnabled );
            }*/

            ImGui::EndMenu();
        }
        #endif
    }

    void NavmeshDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {}
}
#endif