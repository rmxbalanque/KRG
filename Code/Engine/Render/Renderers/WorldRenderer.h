#pragma once

#include "Engine/Render/_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "Engine/Render/IRenderer.h"
#include "System/Render/RenderDevice.h"
#include "System/Core/Math/BVH/AABBTree.h"
#include "System/Core/Types/Event.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshComponent;
    class SkeletalMeshComponent;
    class SkeletalMesh;
    class Viewport;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API WorldRenderer : public IRenderer
    {
    public:

        KRG_RENDERER_ID( WorldRenderer, Render::RendererPriorityLevel::Game );

    public:

        inline bool IsInitialized() const { return m_initialized; }
        bool Initialize( RenderDevice* pRenderDevice );
        void Shutdown();

        virtual void RenderWorld( Viewport const& viewport, EntityWorld* pWorld ) override final;

    private:

        void RenderStaticMeshes( Viewport const& viewport, TVector<StaticMeshComponent const*> const& meshComponents );
        void RenderSkeletalMeshes( Viewport const& viewport, TVector<SkeletalMeshComponent const*> const& meshComponents );

    private:

        bool                                                    m_initialized = false;

        // Render State
        RenderDevice*                                           m_pRenderDevice = nullptr;
        VertexShader                                            m_vertexShaderStatic;
        VertexShader                                            m_vertexShaderSkeletal;
        PixelShader                                             m_pixelShader;
        BlendState                                              m_blendState;
        RasterizerState                                         m_rasterizerState;
        SamplerState                                            m_samplerState;
        ResourceHandle                                          m_inputBindingStatic;
        ResourceHandle                                          m_inputBindingSkeletal;
        PipelineState                                           m_pipelineStateStatic;
        PipelineState                                           m_pipelineStateSkeletal;
    };
}