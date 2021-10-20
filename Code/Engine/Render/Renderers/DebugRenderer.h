#pragma once

#include "DebugRenderStates.h"
#include "System/Render/RenderDevice.h"
#include "System/Core/Debug/DebugDrawing.h"
#include "System/Core/Debug/DebugDrawingSystem.h"
#include "System/Render/IRenderer.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API DebugRenderer final : public IRenderer
    {
        KRG_RENDERER_ID( DebugRenderer, RendererPriorityLevel::Debug );

    public:

        bool IsInitialized() const { return m_initialized; }
        bool Initialize( RenderDevice* pRenderDevice, Debug::DrawingSystem* pDrawingSystem );
        void Shutdown();
        void Render( Viewport const& viewport ) override final;

    private:

        void DrawPoints( RenderContext const& renderContext, Viewport const& viewport, TVector<Debug::Drawing::PointCommand> const& commands );
        void DrawLines( RenderContext const& renderContext, Viewport const& viewport, TVector<Debug::Drawing::LineCommand> const& commands );
        void DrawTriangles( RenderContext const& renderContext, Viewport const& viewport, TVector<Debug::Drawing::TriangleCommand> const& commands );
        void DrawText( RenderContext const& renderContext, Viewport const& viewport, TVector<Debug::Drawing::TextCommand> const& commands, IntRange cmdRange );

    private:

        RenderDevice*                               m_pRenderDevice = nullptr;

        DebugLineRenderState                        m_lineRS;
        DebugPointRenderState                       m_pointRS;
        DebugPrimitiveRenderState                   m_primitiveRS;
        DebugTextRenderState                        m_textRS;

        Debug::DrawingSystem*                       m_pDebugDrawingSystem = nullptr;
        Debug::Drawing::FrameCommandBuffer          m_drawCommands;
        bool                                        m_initialized = false;

        // Text rendering
        TVector<DebugFontGlyphVertex>               m_intermediateGlyphVertexData;
        TVector<uint16>                             m_intermediateGlyphIndexData;
    };
}
#endif