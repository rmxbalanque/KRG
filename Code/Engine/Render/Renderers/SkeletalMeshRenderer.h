#pragma once

#include "Engine/Render/_Module/API.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Render/IRenderer.h"
#include "System/Entity/EntityWorldSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class SkeletalMesh;
        class SkeletalMeshComponent;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_RENDER_API SkeletalMeshRenderer final : public IWorldEntitySystem, public Render::IRenderer
        {
        public:

            KRG_ENTITY_WORLD_SYSTEM( SkeletalMeshRenderer );
            KRG_RENDERER_ID( SkeletalMeshRenderer, RendererPriorityLevel::Game );

        private:

            struct RegisteredComponent : public EntityRegistryRecord
            {
                TInlineVector<SkeletalMeshComponent*, 2> m_components;
            };

            //-------------------------------------------------------------------------

            struct MeshGroup
            {
                MeshGroup( SkeletalMesh const* pInMesh ) : m_pMesh( pInMesh ) { KRG_ASSERT( pInMesh != nullptr ); }

            public:

                SkeletalMesh const*                     m_pMesh = nullptr;
                TVector<SkeletalMeshComponent*>         m_components;
            };

        public:

            inline bool IsInitialized() const { return m_initialized; }
            bool Initialize( RenderDevice* pRenderDevice );
            void Shutdown();
            virtual void Render( Math::Viewport const& viewport ) override final;

        private:

            virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
            virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
            virtual void UpdateEntitySystem( UpdateContext const& ctx ) override final;

        private:

            RenderDevice*                           m_pRenderDevice = nullptr;

            VertexShader                            m_vertexShader;
            PixelShader                             m_pixelShader;
            BlendState                              m_blendState;
            RasterizerState                         m_rasterizerState;
            SamplerState                            m_samplerState;
            PipelineState                           m_pipelineState;
            ResourceHandle                          m_inputBinding;

            //-------------------------------------------------------------------------

            EntityRegistry<RegisteredComponent>     m_registeredComponents;
            TVector<MeshGroup>                      m_meshGroups;

            bool                                    m_initialized = false;
        };
    }
}