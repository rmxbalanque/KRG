#pragma once

#include "Engine/Render/_Module/API.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Render/Renderers/IRenderer.h"
#include "System/Entity/EntityWorldSystem.h"
#include "System/Entity/EntityWorldSystem.h"
#include "System/Core/Math/BVH/AABBTree.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class StaticMeshComponent;
        class Viewport;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_RENDER_API StaticMeshRenderer final : public IWorldEntitySystem, public Render::IRenderer
        {

        public:

            KRG_ENTITY_WORLD_SYSTEM( StaticMeshRenderer, RequiresUpdate( UpdateStage::FrameEnd ) );
            KRG_RENDERER_ID( StaticMeshRenderer, Render::RendererPriorityLevel::Game );

        private:

            struct ComponentRecord : public EntityRegistryRecord
            {
                StaticMeshComponent*                m_pComponent = nullptr;
                EventBindingID                      m_mobilityChangedEventBinding;
            };

        public:

            inline bool IsInitialized() const { return m_initialized; }
            bool Initialize( RenderDevice* pRenderDevice );
            void Shutdown();

            void RenderStatic( Math::Viewport const& viewport );
            void RenderDynamic( Math::Viewport const& viewport );
            virtual void Render( Math::Viewport const& viewport ) override final {}

        private:

            virtual void ShutdownEntitySystem() override final;
            virtual void UpdateEntitySystem( UpdateContext const& ctx ) override final;
            virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
            virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

            void OnMobilityUpdated( StaticMeshComponent* pComponent );

        private:

            RenderDevice*                                           m_pRenderDevice = nullptr;

            VertexShader                                            m_vertexShader;
            PixelShader                                             m_pixelShader;
            BlendState                                              m_blendState;
            RasterizerState                                         m_rasterizerState;
            SamplerState                                            m_samplerState;
            PipelineState                                           m_pipelineState;
            ResourceHandle                                          m_inputBinding;

            EntityRegistry<ComponentRecord>                         m_meshComponents;

            //-------------------------------------------------------------------------

            //Math::AABBTree                                        m_staticComponentBVH;
            TVector<StaticMeshComponent*>                           m_staticComponents;
            TVector<StaticMeshComponent*>                           m_visibleStaticComponents;
            TVector<StaticMeshComponent*>                           m_dynamicComponents;
            TVector<StaticMeshComponent*>                           m_visibleDynamicComponents;

            //-------------------------------------------------------------------------

            TVector<StaticMeshComponent*>                           m_mobilityUpdateList;           // A list of all components that switched mobility during this frame, will results in an update of the various spatial data structures next frame
            Threading::Mutex                                        m_mobilityUpdateListLock;       // Mobility switches can occur on any thread so the list needs to be threadsafe. We use a simple lock for now since we dont expect too many switches

            //-------------------------------------------------------------------------

            bool                                                    m_initialized = false;
        };
    }
}