#pragma once

#include "../_Module/API.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Render/IRenderer.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Render{ class Viewport; }

    //-------------------------------------------------------------------------

    namespace ImGuiX
    {
        class KRG_SYSTEM_IMGUI_API ImguiRenderer final : public Render::IRenderer
        {
        public:

            KRG_RENDERER_ID( ImguiRenderer, Render::RendererPriorityLevel::DebugUI );

        private:

            struct RecordedCmdBuffer
            {
                TVector<ImDrawCmd>          m_cmdBuffer;
                U32                         m_numVertices;
            };

        public:

            bool IsInitialized() const { return m_initialized; }
            bool Initialize( Render::RenderDevice* pRenderDevice );
            void Shutdown();
            void Render( Math::Viewport const& viewport ) override final;

        private:

            Render::RenderDevice*                   m_pRenderDevice = nullptr;

            Render::VertexShader                    m_vertexShader;
            Render::PixelShader                     m_pixelShader;
            Render::ResourceHandle                  m_inputBinding;
            Render::BlendState                      m_blendState;
            Render::SamplerState                    m_samplerState;
            Render::RasterizerState                 m_rasterizerState;
            TVector<Render::ScissorRect>            m_scissorRects;
            Render::RenderBuffer                    m_indexBuffer;
            Render::VertexBuffer                    m_vertexBuffer;
            Render::Texture                         m_fontTexture;

            TVector<RecordedCmdBuffer>              m_cmdBuffers;

            Render::PipelineState                   m_PSO;
            bool                                    m_initialized = false;
        };
    }
}
#endif