#include "DebugRenderer.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        bool DebugRenderer::Initialize( RenderDevice* pRenderDevice, Debug::DrawingSystem* pDrawingSystem )
        {
            KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr && pDrawingSystem != nullptr );
            m_pRenderDevice = pRenderDevice;
            m_pDebugDrawingSystem = pDrawingSystem;

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

            //-------------------------------------------------------------------------

            m_intermediateGlyphVertexData.resize( DebugTextRenderState::MaxGlyphsPerDrawCall * 4 );
            m_intermediateGlyphIndexData.resize( DebugTextRenderState::MaxGlyphsPerDrawCall * 6 );

            //-------------------------------------------------------------------------

            m_initialized = true;
            return m_initialized;
        }

        void DebugRenderer::Shutdown()
        {
            m_pDebugDrawingSystem->ReflectFrameCommandBuffer( m_drawCommands );
            m_drawCommands.Clear();

            //-------------------------------------------------------------------------

            if( m_pRenderDevice != nullptr )
            {
                m_textRS.Shutdown( m_pRenderDevice );
                m_primitiveRS.Shutdown( m_pRenderDevice );
                m_lineRS.Shutdown( m_pRenderDevice );
                m_pointRS.Shutdown( m_pRenderDevice );
            }

            m_pDebugDrawingSystem = nullptr;
            m_pRenderDevice = nullptr;
            m_initialized = false;
        }

        //-------------------------------------------------------------------------

        void DebugRenderer::DrawPoints( RenderContext const& renderContext, Math::Viewport const& viewport, TVector<Debug::Drawing::PointCommand> const& commands )
        {
            // Set render state
            renderContext.SetPrimitiveTopology( Topology::PointList );

            //-------------------------------------------------------------------------

            auto pData = reinterpret_cast<Float4*>( m_pointRS.m_stagingVertexData.data() );
            auto const dataSize = (U32) m_pointRS.m_stagingVertexData.size();

            S32 const numPoints = (S32) commands.size();
            if ( numPoints > 0 )
            {
                // Fill the points vertex buffer
                U32 const numDrawCalls = (U32) Math::Ceiling( (F32) numPoints / DebugPointRenderState::MaxPointsPerDrawCall );
                for ( auto i = 0u; i < numDrawCalls; i++ )
                {
                    U32 const drawRangeStart = ( i * DebugPointRenderState::MaxPointsPerDrawCall );
                    U32 const drawRangeEnd = drawRangeStart + Math::Min( U32( numPoints - ( i * DebugPointRenderState::MaxPointsPerDrawCall ) ), (U32) DebugPointRenderState::MaxPointsPerDrawCall );
                    U32 const drawRangeLength = ( drawRangeEnd - drawRangeStart );
                    U32 const drawCommandMemorySize = drawRangeLength * sizeof( Debug::Drawing::PointCommand );
                    memcpy( pData, &commands[drawRangeStart], drawCommandMemorySize );

                    renderContext.WriteToBuffer( m_pointRS.m_vertexBuffer, pData, dataSize );
                    renderContext.Draw( drawRangeLength, 0 );
                }
            }
        }

        void DebugRenderer::DrawLines( RenderContext const& renderContext, Math::Viewport const& viewport, TVector<Debug::Drawing::LineCommand> const& commands )
        {
            // Set render state
            renderContext.SetPrimitiveTopology( Topology::LineList );

            //-------------------------------------------------------------------------

            auto pData = reinterpret_cast<Float4*>( m_lineRS.m_stagingVertexData.data() );
            auto const dataSize = (U32) m_lineRS.m_stagingVertexData.size();

            S32 const numLines = (S32) commands.size();
            if ( numLines > 0 )
            {
                // Fill the lines vertex buffer
                U32 const numDrawCalls = (U32) Math::Ceiling( (F32) numLines / DebugLineRenderState::MaxLinesPerDrawCall );
                for ( auto i = 0u; i < numDrawCalls; i++ )
                {
                    U32 const drawRangeStart = ( i * DebugLineRenderState::MaxLinesPerDrawCall );
                    U32 const drawRangeEnd = drawRangeStart + Math::Min( U32( numLines - ( i * DebugLineRenderState::MaxLinesPerDrawCall ) ), (U32) DebugLineRenderState::MaxLinesPerDrawCall );
                    U32 const drawRangeLength = ( drawRangeEnd - drawRangeStart );
                    U32 const drawCommandMemorySize = drawRangeLength * sizeof( Debug::Drawing::LineCommand );
                    memcpy( pData, &commands[drawRangeStart], drawCommandMemorySize );

                    renderContext.WriteToBuffer( m_lineRS.m_vertexBuffer, pData, dataSize );
                    renderContext.Draw( drawRangeLength * 2, 0 );
                }
            }
        }

        void DebugRenderer::DrawTriangles( RenderContext const& renderContext, Math::Viewport const& viewport, TVector<Debug::Drawing::TriangleCommand> const& commands )
        {
            // Set render state
            renderContext.SetPrimitiveTopology( Topology::TriangleList );

            //-------------------------------------------------------------------------

            auto pData = reinterpret_cast<Float4*>( m_primitiveRS.m_stagingVertexData.data() );
            auto const dataSize = (U32) m_primitiveRS.m_stagingVertexData.size();

            S32 const numTriangles = (S32) commands.size();
            if ( numTriangles > 0 )
            {
                // Fill the triangles vertex buffer
                U32 const numDrawCalls = (U32) Math::Ceiling( (F32) numTriangles / DebugPrimitiveRenderState::MaxTrianglesPerDrawCall );
                for ( auto i = 0u; i < numDrawCalls; i++ )
                {
                    U32 const drawRangeStart = ( i * DebugPrimitiveRenderState::MaxTrianglesPerDrawCall );
                    U32 const drawRangeEnd = drawRangeStart + Math::Min( numTriangles - ( i * DebugPrimitiveRenderState::MaxTrianglesPerDrawCall ), DebugPrimitiveRenderState::MaxTrianglesPerDrawCall );
                    U32 const drawRangeLength = ( drawRangeEnd - drawRangeStart );
                    U32 const drawCommandMemorySize = drawRangeLength * sizeof( Debug::Drawing::TriangleCommand );
                    memcpy( pData, &commands[drawRangeStart], drawCommandMemorySize );

                    renderContext.WriteToBuffer( m_primitiveRS.m_vertexBuffer, pData, dataSize );
                    renderContext.Draw( drawRangeLength * 3, 0 );
                }
            }
        }

        void DebugRenderer::DrawText( RenderContext const& renderContext, Math::Viewport const& viewport, TVector<Debug::Drawing::TextCommand> const& commands, TRange<U32> cmdRange )
        {
            KRG_ASSERT( cmdRange.IsValid() );

            S32 const fontIdx = ( commands[cmdRange.m_min].m_size == Debug::Drawing::TextSize::Small ) ? 0 : 1;

            //-------------------------------------------------------------------------

            auto pVertexData = m_intermediateGlyphVertexData.data();
            size_t const vertexDataSize = m_intermediateGlyphVertexData.size() * sizeof( DebugFontGlyphVertex );

            auto pIndexData = m_intermediateGlyphIndexData.data();
            size_t const indexDataSize = m_intermediateGlyphIndexData.size() * sizeof( U16 );

            //-------------------------------------------------------------------------

            S32 numGlyphsDrawn = 0;
            S32 const numCommands = (S32) cmdRange.GetLength();
            for ( auto c = cmdRange.m_min; c < cmdRange.m_max; c++ )
            {
                auto const& cmd = commands[c];

                // Get the glyph string and number of glyphs needed to render it
                //-------------------------------------------------------------------------

                TInlineVector<S32, 100> glyphIndices;
                m_textRS.m_fontAtlas.GetGlyphsForString( fontIdx, cmd.m_text, glyphIndices );

                S32 numGlyphsToDraw = (S32) glyphIndices.size();
                bool const drawTextBox = cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::DrawBackground );
                if ( drawTextBox )
                {
                    numGlyphsToDraw++;
                }

                // If we are going to overflow the buffer, draw all previously recorded glyphs
                if ( DebugTextRenderState::MaxGlyphsPerDrawCall < numGlyphsDrawn + numGlyphsToDraw )
                {
                    renderContext.WriteToBuffer( m_textRS.m_vertexBuffer, pVertexData, vertexDataSize );
                    renderContext.WriteToBuffer( m_textRS.m_indexBuffer, pIndexData, indexDataSize );
                    renderContext.DrawIndexed( numGlyphsDrawn * 6, 0 );
                    numGlyphsDrawn = 0;
                }

                KRG_ASSERT( DebugTextRenderState::MaxGlyphsPerDrawCall >= numGlyphsDrawn + numGlyphsToDraw );

                // Get text top left position
                //-------------------------------------------------------------------------

                Float2 textPosTopLeft;
                if ( cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::WorldSpace ) )
                {
                    // Ensure that the world position is on-screen
                    Vector const textPosTopLeftCS = viewport.WorldSpaceToClipSpace( cmd.m_position );
                    if ( textPosTopLeftCS.GetW() < 0 )
                    {
                        continue;
                    }

                    textPosTopLeft = viewport.ClipSpaceToScreenSpace( textPosTopLeftCS );
                }
                else // Screen-space
                {
                    textPosTopLeft = (Float2) cmd.m_position;
                }

                // Adjust text position based on alignment settings
                //-------------------------------------------------------------------------

                static S32 const textBoxPadding = 3;
                Float2 const extents( m_textRS.m_fontAtlas.GetTextExtents( fontIdx, cmd.m_text.c_str() ) );

                // Horizontal Alignment
                if( cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::AlignLeft ) )
                {
                    if ( drawTextBox )
                    {
                        textPosTopLeft.x += textBoxPadding;
                    }
                }
                else if ( cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::AlignRight ) )
                {
                    textPosTopLeft.x -= extents.x;

                    if ( drawTextBox )
                    {
                        textPosTopLeft.x -= textBoxPadding;
                    }
                }
                else if ( cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::AlignCenter ) )
                {
                    textPosTopLeft.x -= ( extents.x / 2 );

                    if ( drawTextBox )
                    {
                        textPosTopLeft.x -= 0.5f * textBoxPadding;
                    }
                }

                // Vertical Alignment
                if ( cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::AlignTop ) )
                {
                    if ( drawTextBox )
                    {
                        textPosTopLeft.y += textBoxPadding;
                    }
                }
                else if ( cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::AlignBottom ) )
                {
                    textPosTopLeft.y -= extents.y;

                    if ( drawTextBox )
                    {
                        textPosTopLeft.y -= textBoxPadding;
                    }
                }
                else if ( cmd.m_textFlags.IsFlagSet( Debug::Drawing::TextFlags::AlignMiddle ) )
                {
                    textPosTopLeft.y -= extents.y / 2;

                    if ( drawTextBox )
                    {
                        textPosTopLeft.y -= 0.5f * textBoxPadding;
                    }
                }

                // Draw characters
                //-------------------------------------------------------------------------

                KRG_ASSERT( numGlyphsDrawn < DebugTextRenderState::MaxGlyphsPerDrawCall );

                if ( drawTextBox )
                {
                    textPosTopLeft.y += textBoxPadding;
                    m_textRS.m_fontAtlas.WriteCustomGlyphToBuffer( &pVertexData[numGlyphsDrawn * 4], U16( numGlyphsDrawn * 4 ), &pIndexData[numGlyphsDrawn * 6], fontIdx, glyphIndices[0], m_textRS.m_nonZeroAlphaTexCoords, textPosTopLeft, extents, textBoxPadding, Float4( 0, 0, 0, 128.0f / 255 ) );
                    numGlyphsDrawn++;
                }

                numGlyphsDrawn += m_textRS.m_fontAtlas.WriteGlyphsToBuffer( &pVertexData[numGlyphsDrawn * 4], U16( numGlyphsDrawn * 4 ), &pIndexData[numGlyphsDrawn * 6], 0, glyphIndices, textPosTopLeft, cmd.m_color );
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

        static void DrawTextCommands( TVector<Debug::Drawing::TextCommand> const& commands, RenderContext const& renderContext, Math::Viewport const& viewport, TFunction<void( RenderContext const&, Math::Viewport const&, TVector<Debug::Drawing::TextCommand> const&, TRange<U32> )> renderFunction )
        {
            U32 const numCommands = (U32) commands.size();
            TRange<U32> cmdRange = TRange<U32>( 0, 0 );
            for ( U32 i = cmdRange.m_min; i < numCommands; i++ )
            {
                // If the text sizes differ
                if ( commands[i].m_size != commands[cmdRange.m_min].m_size )
                {
                    cmdRange.m_max = i;
                    renderFunction( renderContext, viewport, commands, cmdRange );
                    cmdRange.m_min = i;
                }

                if ( i == ( numCommands - 1 ) )
                {
                    cmdRange.m_max = numCommands;
                    renderFunction( renderContext, viewport, commands, cmdRange );
                }
            }
        }

        //-------------------------------------------------------------------------

        void DebugRenderer::Render( Math::Viewport const& viewport )
        {
            KRG_ASSERT( IsInitialized() && Threading::IsMainThread() );
            KRG_PROFILE_FUNCTION_RENDER();

            m_pDebugDrawingSystem->ReflectFrameCommandBuffer( m_drawCommands );

            if ( !viewport.IsValid() )
            {
                return;
            }

            auto renderContext = m_pRenderDevice->GetImmediateContext();
            renderContext.SetViewport( Float2( viewport.GetSize() ), Float2( viewport.GetTopLeftPosition() ) );

            //-------------------------------------------------------------------------
            // Draw Points
            //-------------------------------------------------------------------------

            {
                KRG_PROFILE_SCOPE_RENDER( "DebugDrawing::DrawPoints" );
                m_pointRS.SetState( renderContext, viewport );

                renderContext.SetDepthTestMode( DepthTestMode::On );
                DebugRenderer::DrawPoints( renderContext, viewport, m_drawCommands.m_opaqueDepthOn.m_pointCommands );

                renderContext.SetDepthTestMode( DepthTestMode::Off );
                DebugRenderer::DrawPoints( renderContext, viewport, m_drawCommands.m_opaqueDepthOff.m_pointCommands );

                //-------------------------------------------------------------------------

                renderContext.SetDepthTestMode( DepthTestMode::On );
                DebugRenderer::DrawPoints( renderContext, viewport, m_drawCommands.m_transparentDepthOn.m_pointCommands );

                renderContext.SetDepthTestMode( DepthTestMode::Off );
                DebugRenderer::DrawPoints( renderContext, viewport, m_drawCommands.m_transparentDepthOff.m_pointCommands );
            }

            //-------------------------------------------------------------------------
            // Draw Lines
            //-------------------------------------------------------------------------

            {
                KRG_PROFILE_SCOPE_RENDER( "DebugDrawing::DrawLines" );
                m_lineRS.SetState( renderContext, viewport );

                renderContext.SetDepthTestMode( DepthTestMode::On );
                DebugRenderer::DrawLines( renderContext, viewport, m_drawCommands.m_opaqueDepthOn.m_lineCommands );

                renderContext.SetDepthTestMode( DepthTestMode::Off );
                DebugRenderer::DrawLines( renderContext, viewport, m_drawCommands.m_opaqueDepthOff.m_lineCommands );

                //-------------------------------------------------------------------------

                renderContext.SetDepthTestMode( DepthTestMode::On );
                DebugRenderer::DrawLines( renderContext, viewport, m_drawCommands.m_transparentDepthOn.m_lineCommands );

                renderContext.SetDepthTestMode( DepthTestMode::Off );
                DebugRenderer::DrawLines( renderContext, viewport, m_drawCommands.m_transparentDepthOff.m_lineCommands );
            }

            //-------------------------------------------------------------------------
            // Draw Primitives
            //-------------------------------------------------------------------------

            {
                KRG_PROFILE_SCOPE_RENDER( "DebugDrawing::DrawPrimitives" );
                m_primitiveRS.SetState( renderContext, viewport );

                renderContext.SetDepthTestMode( DepthTestMode::On );
                DebugRenderer::DrawTriangles( renderContext, viewport, m_drawCommands.m_opaqueDepthOn.m_triangleCommands );

                renderContext.SetDepthTestMode( DepthTestMode::Off );
                DebugRenderer::DrawTriangles( renderContext, viewport, m_drawCommands.m_opaqueDepthOff.m_triangleCommands );

                //-------------------------------------------------------------------------

                renderContext.SetDepthTestMode( DepthTestMode::On );
                DebugRenderer::DrawTriangles( renderContext, viewport, m_drawCommands.m_transparentDepthOn.m_triangleCommands );

                renderContext.SetDepthTestMode( DepthTestMode::Off );
                DebugRenderer::DrawTriangles( renderContext, viewport, m_drawCommands.m_transparentDepthOff.m_triangleCommands );
            }

            //-------------------------------------------------------------------------
            // Draw text
            //-------------------------------------------------------------------------

            {
                KRG_PROFILE_SCOPE_RENDER( "DebugDrawing::DrawText" );
                m_textRS.SetState( renderContext, viewport );

                auto textRenderfunc = [this] ( RenderContext const& renderContext, Math::Viewport const& viewport, TVector<Debug::Drawing::TextCommand> const& commands, TRange<U32> cmdRange ) { DebugRenderer::DrawText( renderContext, viewport, commands, cmdRange ); };

                renderContext.SetDepthTestMode( DepthTestMode::On );
                DrawTextCommands( m_drawCommands.m_opaqueDepthOn.m_textCommands, renderContext, viewport, textRenderfunc );
                DrawTextCommands( m_drawCommands.m_transparentDepthOn.m_textCommands, renderContext, viewport, textRenderfunc );

                renderContext.SetDepthTestMode( DepthTestMode::Off );
                DrawTextCommands( m_drawCommands.m_opaqueDepthOff.m_textCommands, renderContext, viewport, textRenderfunc );
                DrawTextCommands( m_drawCommands.m_transparentDepthOff.m_textCommands, renderContext, viewport, textRenderfunc );
            }

            //-------------------------------------------------------------------------

            m_drawCommands.Clear();
        }
    }
}
