#include "NavmeshDebugRenderer.h"
#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Navmesh/NavPower.h"
#include "System/Core/Debug/DebugDrawingSystem.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Navmesh
{
    class BfxRenderer final : public bfx::Renderer
    {

    public:

        void SetDebugDrawingSystem( Debug::DrawingSystem* pDebugDrawingSystem )
        {
            m_pDebugDrawingSystem = pDebugDrawingSystem;
        }

        void Reset()
        {
            m_statsPos = Float2( 5, 20 );
        }

        inline bool IsDepthTestEnabled() const { return m_depthTestEnabled; }
        inline void SetDepthTestState( bool isEnabled ) { m_depthTestEnabled = isEnabled; }

    private:

        void DrawLineList( bfx::LineSegment const* pLines, uint32 numLines, bfx::Color const& color ) override
        {
            auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
            for ( auto i = 0u; i < numLines; i++ )
            {
                bfx::LineSegment const& line = pLines[i];
                ctx.DrawLine( FromBfx( line.m_v0 ), FromBfx( line.m_v1 ), FromBfx( color ), 1.0f, m_depthTestEnabled ? Debug::DepthTestState::On : Debug::DepthTestState::Off );
            }
        }

        void DrawTriList( bfx::Triangle const* pTris, uint32 numTris, bfx::Color const& color ) override
        {
            auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
            for ( auto i = 0u; i < numTris; i++ )
            {
                bfx::Triangle const& tri = pTris[i];
                ctx.DrawTriangle( FromBfx( tri.m_v0 ), FromBfx( tri.m_v1 ), FromBfx( tri.m_v2 ), FromBfx( color ), m_depthTestEnabled ? Debug::DepthTestState::On : Debug::DepthTestState::Off );
            }
        }

        void DrawString( bfx::Color const& color, char const* str ) override
        {
            auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
            ctx.DrawText2D( m_statsPos, str, FromBfx( color ), Debug::TextSize::Small );
            m_statsPos += Float2( 0, 15 );
        }

        void DrawString( bfx::Color const& color, bfx::Vector3 const& pos, char const* str ) override
        {
            auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
            ctx.DrawText3D( FromBfx( pos ), str, FromBfx( color ), Debug::TextSize::Small );
        }

    private:

        Debug::DrawingSystem*                       m_pDebugDrawingSystem = nullptr;
        Float2                                      m_statsPos = Float2::Zero;
        bool                                        m_depthTestEnabled = true;
    };

    //-------------------------------------------------------------------------

    void NavmeshDebugRenderer::Initialize()
    {
        m_pRenderer = KRG::New<BfxRenderer>();
        bfx::SetRenderer( m_pRenderer );
    }

    void NavmeshDebugRenderer::Shutdown()
    {
        bfx::SetRenderer( nullptr );
        KRG::Delete( m_pRenderer );
    }

    void NavmeshDebugRenderer::RenderWorld( Render::RenderTarget const& target, Render::Viewport const& viewport, EntityWorld* pWorld )
    {
        KRG_ASSERT( m_pRenderer != nullptr && Threading::IsMainThread() );
        KRG_PROFILE_FUNCTION_RENDER();

        auto pWorldSystem = pWorld->GetWorldSystem<NavmeshWorldSystem>();

        // TODO: Support spaces correctly
        m_pRenderer->SetDepthTestState( !pWorldSystem->m_disableDebugDrawDepthTest );
        m_pRenderer->SetDebugDrawingSystem( pWorld->GetDebugDrawingSystem() );
        m_pRenderer->Reset();
        bfx::SystemDraw();
    }
}
#endif