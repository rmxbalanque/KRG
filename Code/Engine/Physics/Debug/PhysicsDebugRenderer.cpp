#include "PhysicsDebugRenderer.h"
#include "Engine/Physics/PhysicsWorldSystem.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    using namespace KRG::Render;

    //-------------------------------------------------------------------------

    bool PhysicsRenderer::Initialize( Render::RenderDevice* pRenderDevice )
    {
        KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr );
        m_pRenderDevice = pRenderDevice;

        //-------------------------------------------------------------------------

        if ( !m_pointRS.Initialize( m_pRenderDevice ) )
        {
            return false;
        }

        if ( !m_lineRS.Initialize( m_pRenderDevice ) )
        {
            return false;
        }

        if ( !m_primitiveRS.Initialize( m_pRenderDevice ) )
        {
            return false;
        }

        if ( !m_textRS.Initialize( m_pRenderDevice ) )
        {
            return false;
        }

        m_initialized = true;
        return m_initialized;
    }

    void PhysicsRenderer::Shutdown()
    {
        if ( m_pRenderDevice != nullptr )
        {
            m_textRS.Shutdown( m_pRenderDevice );
            m_primitiveRS.Shutdown( m_pRenderDevice );
            m_lineRS.Shutdown( m_pRenderDevice );
            m_pointRS.Shutdown( m_pRenderDevice );
        }

        m_pRenderDevice = nullptr;
        m_initialized = false;
    }

    //-------------------------------------------------------------------------

    void PhysicsRenderer::DrawPoints( RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugPoint const* pPoints, uint32 numPoints )
    {
        m_pointRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );
        renderContext.SetPrimitiveTopology( Topology::PointList );

        auto pData = reinterpret_cast<Float4*>( m_pointRS.m_stagingVertexData.data() );
        auto const dataSize = (uint32) m_pointRS.m_stagingVertexData.size();

        //-------------------------------------------------------------------------

        uint32 currentPointIdx = 0;
        uint32 const numDrawCalls = (uint32) Math::Ceiling( (float) numPoints / DebugPointRenderState::MaxPointsPerDrawCall );
        for ( auto i = 0u; i < numDrawCalls; i++ )
        {
            uint32 bufferVertexIdx = 0;
            uint32 const numPointsToDraw = Math::Min( numPoints - currentPointIdx, DebugPointRenderState::MaxPointsPerDrawCall );
            for ( auto p = 0u; p < numPointsToDraw; p++ )
            {
                physx::PxDebugPoint const& point = pPoints[currentPointIdx + p];
                pData[bufferVertexIdx++] = Float4( point.pos.x, point.pos.y, point.pos.z, 1.0f );
                pData[bufferVertexIdx++] = Physics::FromPxColor( point.color ).ToFloat4();
            }

            currentPointIdx += numPointsToDraw;
            renderContext.WriteToBuffer( m_pointRS.m_vertexBuffer, pData, dataSize );
            renderContext.Draw( numPointsToDraw, 0 );
        }
    }

    void PhysicsRenderer::DrawLines( RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugLine const* pLines, uint32 numLines )
    {
        m_lineRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );
        renderContext.SetPrimitiveTopology( Topology::LineList );

        auto pData = reinterpret_cast<Float4*>( m_lineRS.m_stagingVertexData.data() );
        auto const dataSize = (uint32) m_lineRS.m_stagingVertexData.size();

        //-------------------------------------------------------------------------

        uint32 currentLineIdx = 0;
        uint32 const numDrawCalls = (uint32) Math::Ceiling( (float) numLines / DebugLineRenderState::MaxLinesPerDrawCall );
        for ( auto i = 0u; i < numDrawCalls; i++ )
        {
            uint32 bufferVertexIdx = 0;
            uint32 const numLinesToDraw = Math::Min( numLines - currentLineIdx, DebugLineRenderState::MaxLinesPerDrawCall );
            for ( auto l = 0u; l < numLinesToDraw; l++ )
            {
                physx::PxDebugLine const& line = pLines[currentLineIdx + l];
                pData[bufferVertexIdx++] = Float4( line.pos0.x, line.pos0.y, line.pos0.z, 1.0f );
                pData[bufferVertexIdx++] = Physics::FromPxColor( line.color0 ).ToFloat4();
                pData[bufferVertexIdx++] = Float4( line.pos1.x, line.pos1.y, line.pos1.z, 1.0f );
                pData[bufferVertexIdx++] = Physics::FromPxColor( line.color1 ).ToFloat4();
            }

            currentLineIdx += numLinesToDraw;
            renderContext.WriteToBuffer( m_lineRS.m_vertexBuffer, pData, dataSize );
            renderContext.Draw( numLinesToDraw * 2, 0 );
        }
    }

    void PhysicsRenderer::DrawTriangles( RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugTriangle const* pTriangles, uint32 numTriangles )
    {
        m_primitiveRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        auto pData = reinterpret_cast<Float4*>( m_primitiveRS.m_stagingVertexData.data() );
        auto const dataSize = (uint32) m_primitiveRS.m_stagingVertexData.size();

        //-------------------------------------------------------------------------

        uint32 currentTriangleIdx = 0;
        uint32 const numDrawCalls = (uint32) Math::Ceiling( (float) numTriangles / DebugPrimitiveRenderState::MaxTrianglesPerDrawCall );
        for ( auto i = 0u; i < numDrawCalls; i++ )
        {
            uint32 bufferVertexIdx = 0;
            uint32 const numTrianglesToDraw = Math::Min( numTriangles - currentTriangleIdx, DebugPrimitiveRenderState::MaxTrianglesPerDrawCall );
            for ( auto t = 0u; t < numTrianglesToDraw; t++ )
            {
                physx::PxDebugTriangle const& tri = pTriangles[currentTriangleIdx + t];
                pData[bufferVertexIdx++] = Float4( tri.pos0.x, tri.pos0.y, tri.pos0.z, 1.0f );
                pData[bufferVertexIdx++] = Physics::FromPxColor( tri.color0 ).ToFloat4();
                pData[bufferVertexIdx++] = Float4( tri.pos1.x, tri.pos1.y, tri.pos1.z, 1.0f );
                pData[bufferVertexIdx++] = Physics::FromPxColor( tri.color1 ).ToFloat4();
                pData[bufferVertexIdx++] = Float4( tri.pos2.x, tri.pos2.y, tri.pos2.z, 1.0f );
                pData[bufferVertexIdx++] = Physics::FromPxColor( tri.color2 ).ToFloat4();
            }

            currentTriangleIdx += numTrianglesToDraw;
            renderContext.WriteToBuffer( m_primitiveRS.m_vertexBuffer, pData, dataSize );
            renderContext.Draw( numTrianglesToDraw * 3, 0 );
        }
    }

    void PhysicsRenderer::DrawStrings( RenderContext const& renderContext, Render::Viewport const& viewport, physx::PxDebugText const* pStrings, uint32 numStrings )
    {
        m_textRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );

        //-------------------------------------------------------------------------

        auto pVertexData = KRG_STACK_ARRAY_ALLOC( DebugFontGlyphVertex, DebugTextRenderState::MaxGlyphsPerDrawCall * 4 );
        auto const vertexDataSize = sizeof( DebugFontGlyphVertex ) * DebugTextRenderState::MaxGlyphsPerDrawCall * 4;

        auto pIndexData = KRG_STACK_ARRAY_ALLOC( uint16, DebugTextRenderState::MaxGlyphsPerDrawCall * 6 );
        auto const indexDataSize = sizeof( uint16 ) * DebugTextRenderState::MaxGlyphsPerDrawCall * 6;

        int32 numGlyphsDrawn = 0;
        for ( auto c = 0u; c < numStrings; c++ )
        {
            physx::PxDebugText const& debugText = pStrings[c];

            // Get the glyph string and number of glyphs needed to render it
            //-------------------------------------------------------------------------

            TInlineVector<int32, 100> glyphIndices;
            m_textRS.m_fontAtlas.GetGlyphsForString( 0, debugText.string, glyphIndices );

            size_t glyphCount = glyphIndices.size();

            // If we are going to overflow the buffer, draw all previously recorded glyphs
            if ( ( DebugTextRenderState::MaxGlyphsPerDrawCall - numGlyphsDrawn - glyphCount ) < 0 )
            {
                renderContext.WriteToBuffer( m_textRS.m_vertexBuffer, pVertexData, vertexDataSize );
                renderContext.WriteToBuffer( m_textRS.m_indexBuffer, pIndexData, indexDataSize );
                renderContext.DrawIndexed( numGlyphsDrawn * 6, 0 );
                numGlyphsDrawn = 0;
            }

            KRG_ASSERT( ( DebugTextRenderState::MaxGlyphsPerDrawCall - numGlyphsDrawn - glyphCount ) >= 0 );

            // Draw string
            //-------------------------------------------------------------------------

            Float2 const textPosTopLeft = viewport.WorldSpaceToScreenSpace( Physics::FromPx( debugText.position ) ) + 0.5f;
            Float4 const color = Physics::FromPxColor( debugText.color ).ToFloat4();
            numGlyphsDrawn += m_textRS.m_fontAtlas.WriteGlyphsToBuffer( &pVertexData[numGlyphsDrawn * 4], uint16( numGlyphsDrawn * 4 ), &pIndexData[numGlyphsDrawn * 6], 0, glyphIndices, textPosTopLeft, color );
        }

        // Draw glyphs
        if ( numGlyphsDrawn > 0 )
        {
            renderContext.WriteToBuffer( m_textRS.m_vertexBuffer, pVertexData, vertexDataSize );
            renderContext.WriteToBuffer( m_textRS.m_indexBuffer, pIndexData, indexDataSize );
            renderContext.DrawIndexed( numGlyphsDrawn * 6, 0 );
        }
    }

    //-------------------------------------------------------------------------

    void PhysicsRenderer::RenderWorld( EntityWorld* pWorld )
    {
        KRG_ASSERT( IsInitialized() && Threading::IsMainThread() );
        KRG_PROFILE_FUNCTION_RENDER();

        auto pPhysicsSystem = pWorld->GetWorldSystem<PhysicsWorldSystem>();
        if ( !pPhysicsSystem->IsDebugDrawingEnabled() )
        {
            return;
        }

        auto const& viewport = pWorld->GetViewport();
        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        renderContext.SetViewport( Float2( viewport.GetDimensions() ), Float2( viewport.GetTopLeftPosition() ) );

        //-------------------------------------------------------------------------

        // Offset the culling bounds in front of the camera, no point in visualizing lines off-screen
        float const debugHalfDistance = ( pPhysicsSystem->GetDebugDrawDistance() );

        Vector const viewForward = viewport.GetViewForwardDirection();
        Vector cullingBoundsPosition = viewport.GetViewOrigin();
        cullingBoundsPosition += viewForward * debugHalfDistance;

        AABB const debugBounds = AABB( cullingBoundsPosition, debugHalfDistance );
        pPhysicsSystem->m_pScene->setVisualizationCullingBox( ToPx( debugBounds ) );

        //-------------------------------------------------------------------------

        auto const& renderBuffer = pPhysicsSystem->m_pScene->getRenderBuffer();

        uint32 const numPoints = renderBuffer.getNbPoints();
        DrawPoints( renderContext, viewport, renderBuffer.getPoints(), numPoints );

        uint32 const numLines = renderBuffer.getNbLines();
        DrawLines( renderContext, viewport, renderBuffer.getLines(), numLines );

        uint32 const numTriangles = renderBuffer.getNbTriangles();
        DrawTriangles( renderContext, viewport, renderBuffer.getTriangles(), numTriangles );

        uint32 const numStrings = renderBuffer.getNbTexts();
        DrawPoints( renderContext, viewport, renderBuffer.getPoints(), numStrings );
    }
}