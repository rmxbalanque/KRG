#include "NavmeshSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavmeshSystem::Initialize( Debug::DrawingSystem* pDebugDrawingSystem )
    {
        KRG_ASSERT( pDebugDrawingSystem != nullptr );

        #if KRG_ENABLE_NAVPOWER
        bfx::SystemCreate( bfx::SystemParams( 2.0f, bfx::Z_UP ), &m_allocator );
        bfx::RegisterPlannerSystem();
        bfx::SystemStart();

        #if KRG_DEVELOPMENT_TOOLS
        m_debugRenderer.SetDebugDrawingSystem( pDebugDrawingSystem );
        bfx::SetRenderer( &m_debugRenderer );
        #endif
        #endif
    }

    void NavmeshSystem::Shutdown()
    {
        #if KRG_ENABLE_NAVPOWER
        #if KRG_DEVELOPMENT_TOOLS
        bfx::SetRenderer( nullptr );
        m_debugRenderer.SetDebugDrawingSystem( nullptr );
        #endif

        bfx::SystemStop();
        bfx::SystemDestroy();
        #endif
    }

    void NavmeshSystem::Update( UpdateContext const& ctx )
    {
        #if KRG_ENABLE_NAVPOWER
        bfx::SystemSimulate( ctx.GetDeltaTime() );

        #if KRG_DEVELOPMENT_TOOLS
        m_debugRenderer.Reset();
        bfx::SystemDraw();
        #endif
        #endif
    }
}