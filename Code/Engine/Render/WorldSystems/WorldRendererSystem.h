#pragma once

#include "Engine/Render/_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
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

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API WorldRendererSystem final : public IWorldEntitySystem
    {
        friend class WorldRenderer;

    public:

        KRG_REGISTER_TYPE( WorldRendererSystem );
        KRG_ENTITY_WORLD_SYSTEM( WorldRendererSystem, RequiresUpdate( UpdateStage::FrameEnd ) );

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

        // Track all instances of a given mesh together - to limit the number of vertex buffer changes
        struct SkeletalMeshGroup
        {
            SkeletalMeshGroup( SkeletalMesh const* pInMesh ) : m_pMesh( pInMesh ) { KRG_ASSERT( pInMesh != nullptr ); }

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

        // Skeletal Meshes
        //-------------------------------------------------------------------------

        void RegisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent );
        void UnregisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent );

    private:

        // Static meshes
        EntityRegistry<RegisteredStaticMesh>                    m_registeredStaticMeshComponents;
        TVector<StaticMeshComponent*>                           m_staticStaticMeshComponents;
        TVector<StaticMeshComponent*>                           m_dynamicStaticMeshComponents;
        TVector<StaticMeshComponent const*>                     m_visibleStaticMeshComponents;
        TVector<StaticMeshComponent*>                           m_mobilityUpdateList;           // A list of all components that switched mobility during this frame, will results in an update of the various spatial data structures next frame
        Threading::Mutex                                        m_mobilityUpdateListLock;       // Mobility switches can occur on any thread so the list needs to be threadsafe. We use a simple lock for now since we dont expect too many switches

        // Skeletal meshes
        EntityRegistry<RegisteredSkeletalMesh>                  m_registeredSkeletalMeshComponents;
        TVector<SkeletalMeshGroup>                              m_skeletalMeshGroups;
        TVector<SkeletalMeshComponent const*>                   m_visibleSkeletalMeshComponents;

        // Lights

    };
}