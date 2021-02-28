#pragma once

#include "System/Core/Math/Viewport.h"
#include "System/Render/RenderDevice/RenderDevice.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    namespace Render
    {
        //-------------------------------------------------------------------------
        // Primitives
        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_RENDER_API DebugLineRenderState
        {
            constexpr static U32 const MaxLinesPerDrawCall = 100000;

        public:

            bool Initialize( RenderDevice* pRenderDevice );
            void Shutdown( RenderDevice* pRenderDevice );
            void SetState( RenderContext const& renderContext, Math::Viewport const& viewport );

        public:

            VertexShader                    m_vertexShader;
            GeometryShader                  m_geometryShader;
            PixelShader                     m_pixelShader;
            ResourceHandle                  m_inputBinding;
            VertexBuffer                    m_vertexBuffer;
            BlendState                      m_blendState;
            RasterizerState                 m_rasterizerState;
            TVector<U8>                     m_stagingVertexData;

            PipelineState                   m_PSO;
        };

        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_RENDER_API DebugPointRenderState
        {
            constexpr static U32 const MaxPointsPerDrawCall = 100000;

        public:

            bool Initialize( RenderDevice* pRenderDevice );
            void Shutdown( RenderDevice* pRenderDevice );
            void SetState( RenderContext const& renderContext, Math::Viewport const& viewport );

        public:

            VertexShader                    m_vertexShader;
            GeometryShader                  m_geometryShader;
            PixelShader                     m_pixelShader;
            ResourceHandle                  m_inputBinding;
            VertexBuffer                    m_vertexBuffer;
            BlendState                      m_blendState;
            RasterizerState                 m_rasterizerState;
            TVector<U8>                     m_stagingVertexData;

            PipelineState                   m_PSO;
        };

        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_RENDER_API DebugPrimitiveRenderState
        {
            constexpr static U32 const MaxTrianglesPerDrawCall = 100000;

        public:

            bool Initialize( RenderDevice* pRenderDevice );
            void Shutdown( RenderDevice* pRenderDevice );
            void SetState( RenderContext const& renderContext, Math::Viewport const& viewport );

        public:

            VertexShader                    m_vertexShader;
            PixelShader                     m_pixelShader;
            ResourceHandle                  m_inputBinding;
            VertexBuffer                    m_vertexBuffer;
            BlendState                      m_blendState;
            RasterizerState                 m_rasterizerState;
            TVector<U8>                     m_stagingVertexData;

            PipelineState                   m_PSO;
        };

        //-------------------------------------------------------------------------
        // Text
        //-------------------------------------------------------------------------

        struct DebugFontGlyph
        {
            Float2                          m_positionTL;
            Float2                          m_positionBR;
            Float2                          m_texCoordsTL;
            Float2                          m_texCoordsBR;
            F32                             m_advanceX;
        };

        struct DebugFontGlyphVertex
        {
            Float2                          m_pos;
            Float2                          m_texcoord;
            U32                             m_color;
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RENDER_API DebugTextFontAtlas
        {

        public:

            struct FontDesc
            {
                Byte const*                 m_compressedFontData = nullptr;
                F32                         m_fontSize = 10.0f;
            };

        private:

            struct FontInfo
            {

            public:

                inline F32 GetAscent() const { return m_ascent; }
                inline F32 GetDescent() const { return m_descent; }
                inline F32 GetLineGap() const { return m_lineGap; }
                inline TRange<S32> GetValidGlyphRange() const { return TRange<S32>( 0x20, 0xFF ); }
                inline S32 GetGlyphIndex( char c ) const { KRG_ASSERT( GetValidGlyphRange().IsInsideInclusive( c ) ); return c - GetValidGlyphRange().m_min; }
                inline DebugFontGlyph const& GetGlyph( S32 glyphIdx ) const { return m_glyphs[glyphIdx]; }

            public:

                TVector<DebugFontGlyph>     m_glyphs;
                F32                         m_ascent = 0.0f;
                F32                         m_descent = 0.0f;
                F32                         m_lineGap = 0.0f;
            };

        public:

            // Generate a font atlas with the specified fonts
            bool Generate( FontDesc* pFonts, S32 numFonts );

            // Returns the 2D dimensions of the atlas data
            inline Int2 GetDimensions() const { return Int2( 256 ); }

            // Returns the raw font atlas bitmap data
            inline Byte const* GetAtlasData() const { return m_atlasData.data(); }

            // Get a list of glyphs indices that correspond to the supplied string
            void GetGlyphsForString( U32 fontIdx, String const& str, TInlineVector<S32, 100>& outGlyphIndices ) const;

            // Get a list of glyphs indices that correspond to the supplied string
            void GetGlyphsForString( U32 fontIdx, char const* pStr, TInlineVector<S32, 100>& outGlyphIndices ) const;

            // Get the 2D pixel size of the string if it were rendered
            Int2 GetTextExtents( U32 fontIdx, char const* pText ) const;

            // Fill the supplied vertex and index buffer with the necessary data to render the supplied glyphs
            U32 WriteGlyphsToBuffer( DebugFontGlyphVertex* pVertexBuffer, U16 indexStartOffset, U16* pIndexBuffer, U32 fontIdx, TInlineVector<S32, 100> const& glyphIndices, Float2 const& textPosTopLeft, Float4 const& color ) const;

            // Writes a glyph with custom texture coords to the render buffers
            void WriteCustomGlyphToBuffer( DebugFontGlyphVertex* pVertexBuffer, U16 indexStartOffset, U16* pIndexBuffer, U32 fontIdx, S32 firstGlyphIdx, Float2 const& texCoords, Float2 const& baselinePos, Int2 const& textExtents, S32 pixelPadding, Float4 const& color ) const;

        private:

            TVector<Byte>                   m_atlasData;
            TInlineVector<FontInfo, 3>      m_fonts;
        };

        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_RENDER_API DebugTextRenderState
        {
            constexpr static U32 const MaxGlyphsPerDrawCall = 10000;
            static Float4 const ClipSpaceTopLeft;

        public:

            bool Initialize( RenderDevice* pRenderDevice );
            void Shutdown( RenderDevice* pRenderDevice );
            void SetState( RenderContext const& renderContext, Math::Viewport const& viewport );

        public:

            VertexShader                    m_vertexShader;
            PixelShader                     m_pixelShader;
            ResourceHandle                  m_inputBinding;
            VertexBuffer                    m_vertexBuffer;
            VertexBuffer                    m_indexBuffer;
            SamplerState                    m_samplerState;
            BlendState                      m_blendState;
            RasterizerState                 m_rasterizerState;

            PipelineState                   m_PSO;

            DebugTextFontAtlas              m_fontAtlas;
            Texture                         m_fontAtlasTexture;
            Float2                          m_nonZeroAlphaTexCoords;
        };
    }
}
#endif