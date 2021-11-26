#pragma once

#include "Engine/Render/_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Render/RenderDevice.h"
#include "System/Core/Math/BVH/AABBTree.h"
#include "System/Core/Types/Event.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Types/IDVector.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class SkeletalMeshComponent;
    class DirectionalLightComponent;
    class PointLightComponent;
    class SpotLightComponent;
    class GlobalEnvironmentMapComponent;
    class LocalEnvironmentMapComponent;

    //-------------------------------------------------------------------------
    // TODO: There's a huge flaw how we use the entity registration records in this system
    // Right now this will horribly break when we have more than a single component of the same type on the same entity!!!
    // FIX THIS SHIT!

    class KRG_ENGINE_RENDER_API WorldRendererSystem final : public IWorldEntitySystem
    {
        friend class WorldRenderer;
        friend class RenderDebugView;

    public:

        KRG_REGISTER_TYPE( WorldRendererSystem );
        KRG_ENTITY_WORLD_SYSTEM( WorldRendererSystem, RequiresUpdate( UpdateStage::FrameEnd ) );

    private:

        // TODO: disable in ship, sync with shader
        enum VisualizationMode
        {
            VIS_MODE_LIGHTING  = 0,
            VIS_MODE_ALBEDO    = 1,
            VIS_MODE_NORMALS   = 2,
            VIS_MODE_METALNESS = 3,
            VIS_MODE_ROUGHNESS = 4,
            VIS_MODE_AO        = 5,

            VIS_MODE_BITS_SHIFT = 32 - 3,
        };

        //-------------------------------------------------------------------------

        // Static meshes need special handling for mobility changes
        struct RegisteredStaticMesh
        {
            inline UUID const& GetID() const { return m_pComponent->GetID(); }

            StaticMeshComponent*                                m_pComponent = nullptr;
            EventBindingID                                      m_mobilityChangedEventBinding;
            EventBindingID                                      m_staticMobilityTransformUpdatedEventBinding;
        };

        // Track all instances of a given mesh together - to limit the number of vertex buffer changes
        struct SkeletalMeshGroup
        {
            SkeletalMeshGroup( SkeletalMesh const* pInMesh ) : m_pMesh( pInMesh ) { KRG_ASSERT( pInMesh != nullptr ); }

            inline uint32 GetID() const { return m_pMesh->GetResourceID().GetID(); }

        public:

            SkeletalMesh const*                                 m_pMesh = nullptr;
            TVector<SkeletalMeshComponent*>                     m_components;
        };

    private:

        // Entity System
        //-------------------------------------------------------------------------

        virtual void InitializeSystem( SystemRegistry const& systemRegistry ) override final;
        virtual void ShutdownSystem() override final;
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        // Static Meshes
        //-------------------------------------------------------------------------

        void RegisterStaticMeshComponent( Entity const* pEntity, StaticMeshComponent* pMeshComponent );
        void UnregisterStaticMeshComponent( Entity const* pEntity, StaticMeshComponent* pMeshComponent );
        void OnStaticMeshMobilityUpdated( StaticMeshComponent* pComponent );
        void OnStaticMobilityComponentTransformUpdated( StaticMeshComponent* pComponent );

        // Skeletal Meshes
        //-------------------------------------------------------------------------

        void RegisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent );
        void UnregisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent );

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void SetVisualizationMode( uint32 mode ) { m_visualizationMode = mode; }
        uint32 GetVisualizationMode() { return m_visualizationMode; }
        #endif

    private:

        // Static meshes
        TIDVector<UUID, RegisteredStaticMesh>                   m_registeredStaticMeshComponents;
        TIDVector<UUID, StaticMeshComponent*>                   m_staticStaticMeshComponents;
        TIDVector<UUID, StaticMeshComponent*>                   m_dynamicStaticMeshComponents;
        TVector<StaticMeshComponent const*>                     m_visibleStaticMeshComponents;
        Threading::Mutex                                        m_mobilityUpdateListLock;               // Mobility switches can occur on any thread so the list needs to be threadsafe. We use a simple lock for now since we dont expect too many switches
        TVector<StaticMeshComponent*>                           m_mobilityUpdateList;                   // A list of all components that switched mobility during this frame, will results in an update of the various spatial data structures next frame
        TVector<StaticMeshComponent*>                           m_staticMobilityTransformUpdateList;    // A list of all static mobility components that have moved during this frame, will results in an update of the various spatial data structures next frame

        // Skeletal meshes
        TIDVector<UUID, SkeletalMeshComponent*>                 m_registeredSkeletalMeshComponents;
        TIDVector<uint32, SkeletalMeshGroup>                    m_skeletalMeshGroups;
        TVector<SkeletalMeshComponent const*>                   m_visibleSkeletalMeshComponents;

        // Lights
        TIDVector<UUID, DirectionalLightComponent*>             m_registeredDirectionLightComponents;
        TIDVector<UUID, PointLightComponent*>                   m_registeredPointLightComponents;
        TIDVector<UUID, SpotLightComponent*>                    m_registeredSpotLightComponents;
        TIDVector<UUID, LocalEnvironmentMapComponent*>          m_registeredLocalEnvironmentMaps;
        TIDVector<UUID, GlobalEnvironmentMapComponent*>         m_registeredGlobalEnvironmentMaps;

        #if KRG_DEVELOPMENT_TOOLS
        uint32                                                  m_visualizationMode = 0;
        #endif
    };
}