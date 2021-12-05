#include "DebugView_Navmesh.h"
#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

#include "bfxMoverSpace.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Navmesh
{
    NavmeshDebugView::NavmeshDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Navmesh", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void NavmeshDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pNavmeshWorldSystem = pWorld->GetWorldSystem<NavmeshWorldSystem>();
    }

    void NavmeshDebugView::Shutdown()
    {
        m_pNavmeshWorldSystem = nullptr;
    }

    //-------------------------------------------------------------------------

    void NavmeshDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        auto CreatePlannerCheckboxForFlag = [this] ( char const* pLabel, bfx::PlannerDebugFlag flag )
        {
            bool isEnabled = bfx::GetGlobalDebugFlag( m_pNavmeshWorldSystem->m_pInstance, flag );
            if ( ImGui::Checkbox( pLabel, &isEnabled ) )
            {
                bfx::SetGlobalDebugFlag( m_pNavmeshWorldSystem->m_pInstance, flag, isEnabled );
            }
        };

        auto CreateMoverCheckboxForFlag = [this] ( char const* pLabel, bfx::MoverDebugFlag flag )
        {
            bool isEnabled = bfx::GetGlobalDebugFlag( m_pNavmeshWorldSystem->m_pInstance, flag );
            if ( ImGui::Checkbox( pLabel, &isEnabled ) )
            {
                bfx::SetGlobalDebugFlag( m_pNavmeshWorldSystem->m_pInstance, flag, isEnabled );
            }
        };

        //-------------------------------------------------------------------------

        bool isDrawingNavpowerStats = bfx::GetGlobalDebugFlag( m_pNavmeshWorldSystem->m_pInstance, bfx::BFX_DRAW_STATS );
        if ( ImGui::Checkbox( "Draw NavPower Stats", &isDrawingNavpowerStats ) )
        {
            bfx::SetGlobalDebugFlag( m_pNavmeshWorldSystem->m_pInstance, bfx::BFX_DRAW_STATS, isDrawingNavpowerStats );
        }

        //-------------------------------------------------------------------------

        ImGui::Separator();

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Areas" ) )
        {
            CreatePlannerCheckboxForFlag( "Draw Area Edges", bfx::BFX_DRAW_PLANNER_AREAS );
            CreatePlannerCheckboxForFlag( "Draw Area Normals", bfx::BFX_DRAW_AREA_NORMALS );
            CreatePlannerCheckboxForFlag( "Draw Areas", bfx::BFX_DRAW_PLANNER_AREAS_SOLID );
            CreatePlannerCheckboxForFlag( "Draw Area Connectivity", bfx::BFX_DRAW_PLANNER_CONNECTIVITY );

            CreatePlannerCheckboxForFlag( "Draw Area Usage Flags", bfx::BFX_DRAW_AREA_USAGE_FLAGS );
            CreatePlannerCheckboxForFlag( "Draw Area Penalties", bfx::BFX_DRAW_AREA_PENALTY_MULTS );
            CreatePlannerCheckboxForFlag( "Draw Area Sharing Penalties", bfx::BFX_DRAW_SHARING_PENALTY_COUNTS );

            CreatePlannerCheckboxForFlag( "Colorize Areas Usage Flags", bfx::BFX_COLORIZE_AREA_USAGE_FLAGS );

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Movers" ) )
        {
            CreateMoverCheckboxForFlag( "Draw Mover Cylinders", bfx::BFX_DRAW_MOVER_CYLINDERS );
            CreateMoverCheckboxForFlag( "Draw Mover Goals", bfx::BFX_DRAW_MOVER_GOALS );
            CreateMoverCheckboxForFlag( "Draw Mover Goals Reached", bfx::BFX_DRAW_MOVER_GOALS_REACHED );
            CreateMoverCheckboxForFlag( "Draw Mover Debug Names", bfx::BFX_DRAW_MOVER_DEBUG_NAMES );
            CreateMoverCheckboxForFlag( "Draw Mover State", bfx::BFX_DRAW_MOVER_STATE );
            CreateMoverCheckboxForFlag( "Draw Mover Attractions", bfx::BFX_DRAW_MOVER_ATTRACTIONS );
            CreateMoverCheckboxForFlag( "Draw Mover Client Motion", bfx::BFX_DRAW_CLIENT_MOTION );
            CreateMoverCheckboxForFlag( "Draw Mover Splines", bfx::BFX_DRAW_SPLINE );
            CreateMoverCheckboxForFlag( "Draw Mover Path Section", bfx::BFX_DRAW_CUR_PATH_SECTION );
            CreateMoverCheckboxForFlag( "Draw Mover Follower Goals", bfx::BFX_DRAW_FOLLOWER_GOALS );
            CreateMoverCheckboxForFlag( "Draw Mover Trails", bfx::BFX_DRAW_MOVER_TRAIL );

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Obstacles" ) )
        {
            CreatePlannerCheckboxForFlag( "Draw Obstacles", bfx::BFX_DRAW_OBSTACLES );
            CreatePlannerCheckboxForFlag( "Draw Obstacle Flags", bfx::BFX_DRAW_OBSTACLE_FLAGS );

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Drawing Options" ) )
        {
            bool isDepthTestEnabled = !m_pNavmeshWorldSystem->m_renderer.IsDepthTestEnabled();
            if ( ImGui::Checkbox( "Enable Depth Test", &isDepthTestEnabled ) )
            {
                m_pNavmeshWorldSystem->m_renderer.SetDepthTestState( isDepthTestEnabled );
            }

            ImGui::EndMenu();
        }
    }

    void NavmeshDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {}
}
#endif