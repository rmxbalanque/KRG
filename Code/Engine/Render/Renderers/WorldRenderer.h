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
    class DirectionalLightComponent;
    class GlobalEnvironmentMapComponent;
    class PointLightComponent;
    class StaticMeshComponent;
    class SkeletalMeshComponent;
    class SkeletalMesh;
    class StaticMesh;
    class Viewport;
    class Material;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API WorldRenderer : public IRenderer
    {
        enum
        {
            LIGHTING_ENABLE_SUN = ( 1 << 0 ),
            LIGHTING_ENABLE_SUN_SHADOW = ( 1 << 1 ),
            LIGHTING_ENABLE_SKYLIGHT = ( 1 << 2 ),
        };

        enum
        {
            MATERIAL_USE_ALBEDO_TEXTURE = ( 1 << 0 ),
            MATERIAL_USE_NORMAL_TEXTURE = ( 1 << 1 ),
            MATERIAL_USE_METALNESS_TEXTURE = ( 1 << 2 ),
            MATERIAL_USE_ROUGHNESS_TEXTURE = ( 1 << 3 ),
            MATERIAL_USE_AO_TEXTURE = ( 1 << 4 ),
        };

        enum
        {
            MAX_PUNCTUAL_LIGHTS = 16,
        };

        struct PunctualLight
        {
            Vector m_positionInvRadiusSqr;
            Vector m_dir;
            Vector m_color;
            Vector m_spotAngles;
        };

        struct LightData
        {
            Vector          m_SunDirIndirectIntensity = Vector::Zero;// TODO: refactor to Float3 and float
            Vector          m_SunColorRoughnessOneLevel = Vector::Zero;// TODO: refactor to Float3 and float
            Matrix          m_sunShadowMapMatrix = Matrix( ZeroInit );
            float           m_manualExposure = -1.0f;
            uint32          m_lightingFlags = 0;
            uint32          m_numPunctualLights = 0;
            PunctualLight   m_punctualLights[MAX_PUNCTUAL_LIGHTS];
        };

        struct alignas(16) PickingData
        {
            PickingData() = default;

            inline PickingData( uint64 v0, uint64 v1 )
            {
                m_ID[0] = (uint32) ( v0 & 0x00000000FFFFFFFF );
                m_ID[1] = (uint32) ( ( v0 >> 32 ) & 0x00000000FFFFFFFF );
                m_ID[2] = (uint32) ( v1 & 0x00000000FFFFFFFF );
                m_ID[3] = (uint32) ( ( v1 >> 32 ) & 0x00000000FFFFFFFF );
            }

            uint32 m_ID[4];
            Float4 m_padding;
        };

        struct MaterialData
        {
            uint32   m_surfaceFlags = 0;
            float    m_metalness = 0.0f;
            float    m_roughness = 0.0f;
            float    m_normalScaler = 1.0f;
            Vector   m_albedo;
        };

        struct ObjectTransforms
        {
            Matrix  m_worldTransform = Matrix( ZeroInit );
            Matrix  m_normalTransform = Matrix( ZeroInit );
            Matrix  m_viewprojTransform = Matrix( ZeroInit );
        };

        struct RenderData //TODO: optimize - there should not be per frame updates
        {
            ObjectTransforms                        m_transforms;
            LightData                               m_lightData;
            CubemapTexture const*                   m_pSkyboxRadianceTexture;
            CubemapTexture const*                   m_pSkyboxTexture;
            TVector<StaticMeshComponent const*>&    m_staticMeshComponents;
            TVector<SkeletalMeshComponent const*>&  m_skeletalMeshComponents;
        };

    public:

        KRG_RENDERER_ID( WorldRenderer, Render::RendererPriorityLevel::Game );

        static void SetMaterial( RenderContext const& renderContext, PixelShader& pixelShader, Material const* pMaterial );
        static void SetDefaultMaterial( RenderContext const& renderContext, PixelShader& pixelShader );

    public:

        inline bool IsInitialized() const { return m_initialized; }
        bool Initialize( RenderDevice* pRenderDevice );
        void Shutdown();

        virtual void RenderWorld( Seconds const deltaTime, Viewport const& viewport, RenderTarget const& renderTarget, EntityWorld* pWorld ) override final;

    private:

        void RenderSunShadows( Viewport const& viewport, DirectionalLightComponent* pDirectionalLightComponent, RenderData const& data );
        void RenderStaticMeshes( Viewport const& viewport, RenderTarget const& renderTarget, RenderData const& data );
        void RenderSkeletalMeshes( Viewport const& viewport, RenderTarget const& renderTarget, RenderData const& data );
        void RenderSkybox( Viewport const& viewport, RenderData const& data );

        void SetupRenderStates( Viewport const& viewport, PixelShader* pShader, RenderData const& data );

    private:

        bool                                                    m_initialized = false;

        // Render State
        VertexShader                                            m_vertexShaderSkybox;
        PixelShader                                             m_pixelShaderSkybox;
        RenderDevice*                                           m_pRenderDevice = nullptr;
        VertexShader                                            m_vertexShaderStatic;
        VertexShader                                            m_vertexShaderSkeletal;
        PixelShader                                             m_pixelShader;
        PixelShader                                             m_emptyPixelShader;
        BlendState                                              m_blendState;
        RasterizerState                                         m_rasterizerState;
        SamplerState                                            m_bilinearSampler;
        SamplerState                                            m_bilinearClampedSampler;
        SamplerState                                            m_shadowSampler;
        ShaderInputBindingHandle                                m_inputBindingStatic;
        ShaderInputBindingHandle                                m_inputBindingSkeletal;
        PipelineState                                           m_pipelineStateStatic;
        PipelineState                                           m_pipelineStateSkeletal;
        PipelineState                                           m_pipelineStateStaticShadow;
        PipelineState                                           m_pipelineStateSkeletalShadow;
        PipelineState                                           m_pipelineSkybox;
        ComputeShader                                           m_precomputeDFGComputeShader;
        Texture                                                 m_precomputedBRDF;
        Texture                                                 m_shadowMap;
        PipelineState                                           m_pipelinePrecomputeBRDF;

        PixelShader                                             m_pixelShaderPicking;
        PipelineState                                           m_pipelineStateStaticPicking;
        PipelineState                                           m_pipelineStateSkeletalPicking;
    };
}