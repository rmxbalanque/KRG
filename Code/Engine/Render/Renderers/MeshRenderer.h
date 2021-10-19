#pragma once

#include "Engine/Render/_Module/API.h"
#include "System/Render/RenderDevice.h"
#include "System/Render/IRenderer.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Math/BVH/AABBTree.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshComponent;
    class SkeletalMeshComponent;
    class SkeletalMesh;
    class Viewport;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API MeshRenderer final : public IWorldEntitySystem, public Render::IRenderer
    {

    public:

        KRG_ENTITY_WORLD_SYSTEM( MeshRenderer, RequiresUpdate( UpdateStage::FrameEnd ) );
        KRG_RENDERER_ID( MeshRenderer, Render::RendererPriorityLevel::Game );

    private:

        struct RegisteredStaticMesh : public EntityRegistryRecord
        {
            StaticMeshComponent*                                m_pComponent = nullptr;
            EventBindingID                                      m_mobilityChangedEventBinding;
        };

        struct RegisteredSkeletalMesh : public EntityRegistryRecord
        {
            TInlineVector<SkeletalMeshComponent*, 2>            m_components;
        };

        struct SkeletalMeshGroup
        {
            SkeletalMeshGroup( SkeletalMesh const* pInMesh ) : m_pMesh( pInMesh ) { KRG_ASSERT( pInMesh != nullptr ); }

        public:

            SkeletalMesh const*                                 m_pMesh = nullptr;
            TVector<SkeletalMeshComponent*>                     m_components;
        };

    public:

        inline bool IsInitialized() const { return m_initialized; }
        bool Initialize( RenderDevice* pRenderDevice );
        void Shutdown();

        void RenderStatic( Viewport const& viewport );
        void RenderDynamic( Viewport const& viewport );
        void RenderSkeletal( Viewport const& viewport );
        virtual void Render( Viewport const& viewport ) override final {}

    private:

        virtual void ShutdownEntitySystem() override final;
        virtual void UpdateEntitySystem( UpdateContext const& ctx ) override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void OnMobilityUpdated( StaticMeshComponent* pComponent );

    private:

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

        // Static Meshes
        //-------------------------------------------------------------------------

        EntityRegistry<RegisteredStaticMesh>                    m_registeredStaticMeshComponents;
        TVector<StaticMeshComponent*>                           m_staticComponents;
        TVector<StaticMeshComponent*>                           m_visibleStaticComponents;
        TVector<StaticMeshComponent*>                           m_dynamicComponents;
        TVector<StaticMeshComponent*>                           m_visibleDynamicComponents;
        TVector<StaticMeshComponent*>                           m_mobilityUpdateList;           // A list of all components that switched mobility during this frame, will results in an update of the various spatial data structures next frame
        Threading::Mutex                                        m_mobilityUpdateListLock;       // Mobility switches can occur on any thread so the list needs to be threadsafe. We use a simple lock for now since we dont expect too many switches

        // Skeletal Meshes
        //-------------------------------------------------------------------------

        EntityRegistry<RegisteredSkeletalMesh>                  m_registeredSkeletalMeshComponents;
        TVector<SkeletalMeshGroup>                              m_skeletalMeshGroups;

        //-------------------------------------------------------------------------

        bool                                                    m_initialized = false;
    };
}