#include "NavPower.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavPowerRenderer::SetDebugDrawingSystem( Debug::DrawingSystem* pDebugDrawingSystem )
    {
        m_pDebugDrawingSystem = pDebugDrawingSystem;
    }

    void NavPowerRenderer::Reset()
    {
        m_statsPos = Float2( 5, 20 );
    }

    void NavPowerRenderer::DrawLineList( bfx::LineSegment const* pLines, uint32 numLines, bfx::Color const& color )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        for ( auto i = 0u; i < numLines; i++ )
        {
            bfx::LineSegment const& line = pLines[i];
            ctx.DrawLine( FromBfx( line.m_v0 ), FromBfx( line.m_v1 ), FromBfx( color ), 1.0f, m_depthTestEnabled ? Debug::DepthTestState::On : Debug::DepthTestState::Off );
        }
    }

    void NavPowerRenderer::DrawTriList( bfx::Triangle const* pTris, uint32 numTris, bfx::Color const& color )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        for ( auto i = 0u; i < numTris; i++ )
        {
            bfx::Triangle const& tri = pTris[i];
            ctx.DrawTriangle( FromBfx( tri.m_v0 ), FromBfx( tri.m_v1 ), FromBfx( tri.m_v2 ), FromBfx( color ), m_depthTestEnabled ? Debug::DepthTestState::On : Debug::DepthTestState::Off );
        }
    }

    void NavPowerRenderer::DrawString( bfx::Color const& color, char const* str )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        ctx.DrawText2D( m_statsPos, str, FromBfx( color ), Debug::TextSize::Small );
        m_statsPos += Float2( 0, 15 );
    }

    void NavPowerRenderer::DrawString( bfx::Color const& color, bfx::Vector3 const& pos, char const* str )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        ctx.DrawText3D( FromBfx( pos ), str, FromBfx( color ), Debug::TextSize::Small );
    }
}