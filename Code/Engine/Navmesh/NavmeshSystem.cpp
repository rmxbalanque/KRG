#include "NavmeshSystem.h"
#include "Engine/Navmesh/Components/NavmeshComponent.h"
#include "System/Entity/Entity.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavmeshSystem::Initialize( Debug::DrawingSystem* pDebugDrawingSystem )
    {
        KRG_ASSERT( pDebugDrawingSystem != nullptr );

        bfx::SystemCreate( bfx::SystemParams( 1.0f, bfx::Z_UP ), &m_allocator );
        bfx::RegisterPlannerSystem();
        bfx::SystemStart();

        #if KRG_DEVELOPMENT_TOOLS
        m_debugRenderer.SetDebugDrawingSystem( pDebugDrawingSystem );
        bfx::SetRenderer( &m_debugRenderer );
        #endif
    }

    void NavmeshSystem::Shutdown()
    {
        #if KRG_DEVELOPMENT_TOOLS
        bfx::SetRenderer( nullptr );
        m_debugRenderer.SetDebugDrawingSystem( nullptr );
        #endif

        bfx::SystemStop();
        bfx::SystemDestroy();
    }

    void NavmeshSystem::Update( UpdateContext const& ctx )
    {
        bfx::SystemSimulate( ctx.GetDeltaTime() );

        #if KRG_DEVELOPMENT_TOOLS
        m_debugRenderer.Reset();
        bfx::SystemDraw();
        #endif
    }
}