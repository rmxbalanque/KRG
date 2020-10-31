#include "NavmeshDebugViewController.h"
#include "System/Imgui/ImguiCore.h"

#include <bfxSystem.h>
#include <bfxPlanner.h>

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG::Navmesh
{
    NavmeshDebugViewController::NavmeshDebugViewController()
    {
        auto drawDebugMenu = [this] ( UpdateContext const& context )
        {
            DrawDebugMenu( context );
        };

        m_menuCallbacks.emplace_back( Debug::DebugMenuCallback( "Debug Options", "Navmesh", drawDebugMenu ) );
    }

    void NavmeshDebugViewController::Initialize()
    {}

    void NavmeshDebugViewController::Shutdown()
    {}

    //-------------------------------------------------------------------------

    void NavmeshDebugViewController::DrawDebugMenu( UpdateContext const& context )
    {
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
    }

    void NavmeshDebugViewController::DrawWindows( UpdateContext const& context )
    {}
}
#endif