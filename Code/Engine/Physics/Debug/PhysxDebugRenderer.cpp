#include "PhysxDebugRenderer.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysX.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    using namespace KRG::Render;

    //-------------------------------------------------------------------------

    bool PhysicsRenderer::Initialize( Render::RenderDevice* pRenderDevice, PhysicsSystem* pPhysicsWorld )
    {
        KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr );
        m_pRenderDevice = pRenderDevice;

        KRG_ASSERT( pPhysicsWorld != nullptr );
        m_pPhysicsWorld = pPhysicsWorld;

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

        m_pPhysicsWorld = nullptr;
        m_pRenderDevice = nullptr;
        m_initialized = false;
    }

    //-------------------------------------------------------------------------

    void PhysicsRenderer::DrawPoints( RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugPoint const* pPoints, U32 numPoints )
    {
        m_pointRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );
        renderContext.SetPrimitiveTopology( Topology::PointList );

        auto pData = reinterpret_cast<Float4*>( m_pointRS.m_stagingVertexData.data() );
        auto const dataSize = (U32) m_pointRS.m_stagingVertexData.size();

        //-------------------------------------------------------------------------

        U32 currentPointIdx = 0;
        U32 const numDrawCalls = (U32) Math::Ceiling( (F32) numPoints / DebugPointRenderState::MaxPointsPerDrawCall );
        for ( auto i = 0u; i < numDrawCalls; i++ )
        {
            U32 bufferVertexIdx = 0;
            U32 const numPointsToDraw = Math::Min( numPoints - currentPointIdx, DebugPointRenderState::MaxPointsPerDrawCall );
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

    void PhysicsRenderer::DrawLines( RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugLine const* pLines, U32 numLines )
    {
        m_lineRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );
        renderContext.SetPrimitiveTopology( Topology::LineList );

        auto pData = reinterpret_cast<Float4*>( m_lineRS.m_stagingVertexData.data() );
        auto const dataSize = (U32) m_lineRS.m_stagingVertexData.size();

        //-------------------------------------------------------------------------

        U32 currentLineIdx = 0;
        U32 const numDrawCalls = (U32) Math::Ceiling( (F32) numLines / DebugLineRenderState::MaxLinesPerDrawCall );
        for ( auto i = 0u; i < numDrawCalls; i++ )
        {
            U32 bufferVertexIdx = 0;
            U32 const numLinesToDraw = Math::Min( numLines - currentLineIdx, DebugLineRenderState::MaxLinesPerDrawCall );
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

    void PhysicsRenderer::DrawTriangles( RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugTriangle const* pTriangles, U32 numTriangles )
    {
        m_primitiveRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        auto pData = reinterpret_cast<Float4*>( m_primitiveRS.m_stagingVertexData.data() );
        auto const dataSize = (U32) m_primitiveRS.m_stagingVertexData.size();

        //-------------------------------------------------------------------------

        U32 currentTriangleIdx = 0;
        U32 const numDrawCalls = (U32) Math::Ceiling( (F32) numTriangles / DebugPrimitiveRenderState::MaxTrianglesPerDrawCall );
        for ( auto i = 0u; i < numDrawCalls; i++ )
        {
            U32 bufferVertexIdx = 0;
            U32 const numTrianglesToDraw = Math::Min( numTriangles - currentTriangleIdx, DebugPrimitiveRenderState::MaxTrianglesPerDrawCall );
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

    void PhysicsRenderer::DrawStrings( RenderContext const& renderContext, Math::Viewport const& viewport, physx::PxDebugText const* pStrings, U32 numStrings )
    {
        m_textRS.SetState( renderContext, viewport );
        renderContext.SetDepthTestMode( DepthTestMode::Off );

        //-------------------------------------------------------------------------

        auto pVertexData = KRG_STACK_ARRAY_ALLOC( DebugFontGlyphVertex, DebugTextRenderState::MaxGlyphsPerDrawCall * 4 );
        auto const vertexDataSize = sizeof( DebugFontGlyphVertex ) * DebugTextRenderState::MaxGlyphsPerDrawCall * 4;

        auto pIndexData = KRG_STACK_ARRAY_ALLOC( U16, DebugTextRenderState::MaxGlyphsPerDrawCall * 6 );
        auto const indexDataSize = sizeof( U16 ) * DebugTextRenderState::MaxGlyphsPerDrawCall * 6;

        S32 numGlyphsDrawn = 0;
        for ( auto c = 0u; c < numStrings; c++ )
        {
            physx::PxDebugText const& debugText = pStrings[c];

            // Get the glyph string and number of glyphs needed to render it
            //-------------------------------------------------------------------------

            TInlineVector<S32, 100> glyphIndices;
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
            numGlyphsDrawn += m_textRS.m_fontAtlas.WriteGlyphsToBuffer( &pVertexData[numGlyphsDrawn * 4], U16( numGlyphsDrawn * 4 ), &pIndexData[numGlyphsDrawn * 6], 0, glyphIndices, textPosTopLeft, color );
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

    void PhysicsRenderer::Render( Math::Viewport const& viewport )
    {
        KRG_ASSERT( IsInitialized() && Threading::IsMainThread() );
        KRG_PROFILE_GROUPED_SCOPE_COLOR( "Render", "PhysXRenderer::Render", MP_PINK );

        if ( ( m_pPhysicsWorld->GetDebugFlags() && ( 1 << physx::PxVisualizationParameter::eSCALE ) ) == 0 )
        {
            return;
        }

        auto renderContext = m_pRenderDevice->GetImmediateContext();
        renderContext.SetViewport( Float2( viewport.GetSize() ), Float2( viewport.GetTopLeftPosition() ) );

        //-------------------------------------------------------------------------

        // Offset the culling bounds in front of the camera, no point in visualizing lines off-screen
        F32 const debugHalfDistance = ( m_pPhysicsWorld->GetDebugDrawDistance() );

        Vector const viewForward = viewport.GetViewForwardDirection();
        Vector cullingBoundsPosition = viewport.GetViewOrigin();
        cullingBoundsPosition += viewForward * debugHalfDistance;

        AABB const debugBounds = AABB( cullingBoundsPosition, debugHalfDistance );

        auto pPhysicsScene = m_pPhysicsWorld->GetScene();
        pPhysicsScene->setVisualizationCullingBox( ToPx( debugBounds ) );

        //-------------------------------------------------------------------------

        auto const& renderBuffer = pPhysicsScene->getRenderBuffer();

        U32 const numPoints = renderBuffer.getNbPoints();
        DrawPoints( renderContext, viewport, renderBuffer.getPoints(), numPoints );

        U32 const numLines = renderBuffer.getNbLines();
        DrawLines( renderContext, viewport, renderBuffer.getLines(), numLines );

        U32 const numTriangles = renderBuffer.getNbTriangles();
        DrawTriangles( renderContext, viewport, renderBuffer.getTriangles(), numTriangles );

        U32 const numStrings = renderBuffer.getNbTexts();
        DrawPoints( renderContext, viewport, renderBuffer.getPoints(), numStrings );
    }
}