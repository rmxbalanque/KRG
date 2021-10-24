#include "NavmeshSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavmeshSystem::Initialize()
    {
        #if KRG_ENABLE_NAVPOWER
        bfx::SystemCreate( bfx::SystemParams( 2.0f, bfx::Z_UP ), &m_allocator );
        bfx::RegisterPlannerSystem();
        bfx::SystemStart();
        #endif
    }

    void NavmeshSystem::Shutdown()
    {
        #if KRG_ENABLE_NAVPOWER
        bfx::SystemStop();
        bfx::SystemDestroy();
        #endif
    }

    void NavmeshSystem::Update( UpdateContext const& ctx )
    {
        #if KRG_ENABLE_NAVPOWER
        bfx::SystemSimulate( ctx.GetDeltaTime() );
        #endif
    }
}