#include "NavmeshSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavmeshSystem::Initialize()
    {
        bfx::SystemCreate( bfx::SystemParams( 2.0f, bfx::Z_UP ), &m_allocator );
        bfx::RegisterPlannerSystem();
        bfx::RegisterNavigatorSystem();
        bfx::RegisterMoverSystem();
        bfx::SystemStart();
    }

    void NavmeshSystem::Shutdown()
    {
        bfx::SystemStop();
        bfx::SystemDestroy();
    }

    void NavmeshSystem::Update( UpdateContext const& ctx )
    {
        bfx::SystemSimulate( ctx.GetDeltaTime() );
    }
}