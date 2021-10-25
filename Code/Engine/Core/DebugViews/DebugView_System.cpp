#include "DebugView_System.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    SystemDebugView::SystemDebugView()
    {
        m_menus.emplace_back( DebugMenu( "System", "System", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void SystemDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        ImGui::Separator();

        if ( ImGui::Button( "Open Profiler" ) )
        {
            Profiling::OpenProfiler();
        }
    }
}
#endif