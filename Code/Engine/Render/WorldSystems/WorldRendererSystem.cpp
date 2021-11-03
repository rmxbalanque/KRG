#include "WorldRendererSystem.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "Engine/Render/Components/LightComponents.h"
#include "Engine/Render/Components/EnvironmentMapComponents.h"
#include "Engine/Render/Shaders/EngineShaders.h"
#include "System/Render/RenderDefaultResources.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    #if KRG_DEVELOPMENT_TOOLS
    static DebugSettingBool g_showStaticMeshBounds( "ShowStaticMeshBounds", "Rendering/Static Meshes", "", false );
    static DebugSettingBool g_showSkeletalMeshBounds( "ShowSkeletalMeshBounds", "Rendering/Skeletal Meshes", "", false );
    static DebugSettingBool g_showSkeletalMeshBones( "ShowSkeletalMeshBones", "Rendering/Skeletal Meshes", "", false );
    static DebugSettingBool g_showSkeletalMeshBindPoses( "ShowSkeletalMeshBindPoses", "Rendering/Skeletal Meshes", "", false );
    #endif

    //-------------------------------------------------------------------------

    void WorldRendererSystem::InitializeSystem( SystemRegistry const& systemRegistry )
    {
    }

    void WorldRendererSystem::ShutdownSystem()
    {
        KRG_ASSERT( m_registeredStaticMeshComponents.IsEmpty() );
        KRG_ASSERT( m_registeredSkeletalMeshComponents.IsEmpty() );
        KRG_ASSERT( m_skeletalMeshGroups.empty() );

        KRG_ASSERT( m_registeredDirectionLightComponents.IsEmpty() );
        KRG_ASSERT( m_registeredPointLightComponents.IsEmpty() );
        KRG_ASSERT( m_registeredSpotLightComponents.IsEmpty() );

        KRG_ASSERT( m_registeredLocalEnvironmentMaps.IsEmpty() );
        KRG_ASSERT( m_registeredGlobalEnvironmentMaps.IsEmpty() );
    }

    void WorldRendererSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        // Meshes
        //-------------------------------------------------------------------------

        if ( auto pStaticMeshComponent = ComponentCast<StaticMeshComponent>( pComponent ) )
        {
            RegisterStaticMeshComponent( pEntity, pStaticMeshComponent );
        }
        else if ( auto pSkeletalMeshComponent = ComponentCast<SkeletalMeshComponent>( pComponent ) )
        {
            RegisterSkeletalMeshComponent( pEntity, pSkeletalMeshComponent );
        }
        
        // Lights
        //-------------------------------------------------------------------------

        else if ( auto pLightComponent = ComponentCast<LightComponent>( pComponent ) )
        {
            if ( auto pDirectionalLightComponent = ComponentCast<DirectionalLightComponent>( pComponent ) )
            {
                auto& registeredComponent = m_registeredDirectionLightComponents.AddRecord( pEntity->GetID() );
                registeredComponent.m_pComponent = pDirectionalLightComponent;
            }
            else if ( auto pPointLightComponent = ComponentCast<PointLightComponent>( pComponent ) )
            {
                auto& registeredComponent = m_registeredPointLightComponents.AddRecord( pEntity->GetID() );
                registeredComponent.m_pComponent = pPointLightComponent;
            }
            else if ( auto pSpotLightComponent = ComponentCast<SpotLightComponent>( pComponent ) )
            {
                auto& registeredComponent = m_registeredSpotLightComponents.AddRecord( pEntity->GetID() );
                registeredComponent.m_pComponent = pSpotLightComponent;
            }
        }

        // Environment Maps
        //-------------------------------------------------------------------------

        else if ( auto pLocalEnvMapComponent = ComponentCast<LocalEnvironmentMapComponent>( pComponent ) )
        {
            auto& registeredComponent = m_registeredLocalEnvironmentMaps.AddRecord( pEntity->GetID() );
            registeredComponent.m_pComponent = pLocalEnvMapComponent;
        }
        else if ( auto pGlobalEnvMapComponent = ComponentCast<GlobalEnvironmentMapComponent>( pComponent ) )
        {
            auto& registeredComponent = m_registeredGlobalEnvironmentMaps.AddRecord( pEntity->GetID() );
            registeredComponent.m_pComponent = pGlobalEnvMapComponent;
        }
    }

    void WorldRendererSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        // Meshes
        //-------------------------------------------------------------------------

        if ( auto pStaticMeshComponent = ComponentCast<StaticMeshComponent>( pComponent ) )
        {
            UnregisterStaticMeshComponent( pEntity, pStaticMeshComponent );
        }
        else if ( auto pSkeletalMeshComponent = ComponentCast<SkeletalMeshComponent>( pComponent ) )
        {
            UnregisterSkeletalMeshComponent( pEntity, pSkeletalMeshComponent );
        }

        // Lights
        //-------------------------------------------------------------------------

        else if ( auto pLightComponent = ComponentCast<LightComponent>( pComponent ) )
        {
            if ( auto pDirectionalLightComponent = ComponentCast<DirectionalLightComponent>( pComponent ) )
            {
                m_registeredDirectionLightComponents.RemoveRecord( pEntity->GetID() );
            }
            else if ( auto pPointLightComponent = ComponentCast<PointLightComponent>( pComponent ) )
            {
                m_registeredPointLightComponents.RemoveRecord( pEntity->GetID() );
            }
            else if ( auto pSpotLightComponent = ComponentCast<SpotLightComponent>( pComponent ) )
            {
                m_registeredSpotLightComponents.RemoveRecord( pEntity->GetID() );
            }
        }

        // Environment Maps
        //-------------------------------------------------------------------------

        else if ( auto pLocalEnvMapComponent = ComponentCast<LocalEnvironmentMapComponent>( pComponent ) )
        {
            m_registeredLocalEnvironmentMaps.RemoveRecord( pEntity->GetID() );
        }
        else if ( auto pGlobalEnvMapComponent = ComponentCast<GlobalEnvironmentMapComponent>( pComponent ) )
        {
            m_registeredGlobalEnvironmentMaps.RemoveRecord( pEntity->GetID() );
        }
    }

    void WorldRendererSystem::RegisterStaticMeshComponent( Entity const* pEntity, StaticMeshComponent* pMeshComponent )
    {
        auto& registeredComponent = m_registeredStaticMeshComponents.AddRecord( pEntity->GetID() );
        registeredComponent.m_pComponent = pMeshComponent;
        registeredComponent.m_mobilityChangedEventBinding = pMeshComponent->OnMobilityChanged().Bind( [this] ( StaticMeshComponent* pMeshComponent ) { OnStaticMeshMobilityUpdated( pMeshComponent ); } );

        // Add to appropriate sub-list
        if ( pMeshComponent->GetMobility() == Mobility::Dynamic )
        {
            m_dynamicStaticMeshComponents.push_back( pMeshComponent );
        }
        else
        {
            m_staticStaticMeshComponents.push_back( pMeshComponent );
        }
    }

    void WorldRendererSystem::UnregisterStaticMeshComponent( Entity const* pEntity, StaticMeshComponent* pMeshComponent )
    {
        auto const pRecord = m_registeredStaticMeshComponents[pEntity->GetID()];
        if ( pRecord != nullptr )
        {
            KRG_ASSERT( pRecord->IsSet() );
            auto const pMesh = pRecord->m_pComponent->GetMesh();

            // Unbind mobility change handler and remove from various lists
            pRecord->m_pComponent->OnMobilityChanged().Unbind( pRecord->m_mobilityChangedEventBinding );

            // Get the real mobility of the component
            Mobility realMobility = pRecord->m_pComponent->GetMobility();
            int32 const mobilityListIdx = VectorFindIndex( m_mobilityUpdateList, pRecord->m_pComponent );
            if ( mobilityListIdx != InvalidIndex )
            {
                realMobility = ( realMobility == Mobility::Dynamic ) ? Mobility::Static : Mobility::Dynamic;
                m_mobilityUpdateList.erase_unsorted( m_mobilityUpdateList.begin() + mobilityListIdx );
            }

            // Remove from the relevant runtime list
            if ( realMobility == Mobility::Dynamic )
            {
                m_dynamicStaticMeshComponents.erase_first_unsorted( pRecord->m_pComponent );
            }
            else
            {
                m_staticStaticMeshComponents.erase_first_unsorted( pRecord->m_pComponent );
            }

            // Remove record
            m_registeredStaticMeshComponents.RemoveRecord( pEntity->GetID() );
        }
    }

    void WorldRendererSystem::RegisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent )
    {
        auto pRecord = m_registeredSkeletalMeshComponents[pEntity->GetID()];
        if ( pRecord == nullptr )
        {
            pRecord = &m_registeredSkeletalMeshComponents.AddRecord( pEntity->GetID() );
        }

        pRecord->m_components.emplace_back( pMeshComponent );

        //-------------------------------------------------------------------------

        auto pMesh = pMeshComponent->GetMesh();
        KRG_ASSERT( pMesh != nullptr && pMesh->IsValid() );

        // Add to mesh groups
        auto predicate = [] ( SkeletalMeshGroup const& group, SkeletalMesh const* pMesh ) { return group.m_pMesh == pMesh; };
        int32 const foundGroupIdx = VectorFindIndex( m_skeletalMeshGroups, pMesh, predicate );
        if ( foundGroupIdx == InvalidIndex )
        {
            auto& foundGroup = m_skeletalMeshGroups.emplace_back( SkeletalMeshGroup( pMesh ) );
            foundGroup.m_components.emplace_back( pMeshComponent );
        }
        else
        {
            m_skeletalMeshGroups[foundGroupIdx].m_components.emplace_back( pMeshComponent );
        }
    }

    void WorldRendererSystem::UnregisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent )
    {
        auto const pRecord = m_registeredSkeletalMeshComponents[pEntity->GetID()];
        KRG_ASSERT( pRecord != nullptr && pRecord->IsSet() );

        // Remove from record
        //-------------------------------------------------------------------------

        int32 const foundIdx = VectorFindIndex( pRecord->m_components, pMeshComponent );
        KRG_ASSERT( foundIdx != InvalidIndex );
        pRecord->m_components.erase_unsorted( pRecord->m_components.begin() + foundIdx );

        // Remove empty records
        if ( pRecord->m_components.empty() )
        {
            m_registeredSkeletalMeshComponents.RemoveRecord( pEntity->GetID() );
        }

        // Remove from mesh groups
        //-------------------------------------------------------------------------

        auto pMesh = pMeshComponent->GetMesh();

        auto predicate = [] ( SkeletalMeshGroup const& group, SkeletalMesh const* pMesh ) { return group.m_pMesh == pMesh; };
        int32 const foundGroupIdx = VectorFindIndex( m_skeletalMeshGroups, pMesh, predicate );
        KRG_ASSERT( foundGroupIdx != InvalidIndex );
        m_skeletalMeshGroups[foundGroupIdx].m_components.erase_first( pMeshComponent );

        // Remove empty groups
        if ( m_skeletalMeshGroups[foundGroupIdx].m_components.size() == 0 )
        {
            m_skeletalMeshGroups.erase_unsorted( m_skeletalMeshGroups.begin() + foundGroupIdx );
        }
    }

    //-------------------------------------------------------------------------

    void WorldRendererSystem::UpdateSystem( EntityUpdateContext const& ctx )
    {
        KRG_PROFILE_FUNCTION_RENDER();

        //-------------------------------------------------------------------------
        // Mobility Updates
        //-------------------------------------------------------------------------

        for ( auto pMeshComponent : m_mobilityUpdateList )
        {
            Mobility const mobility = pMeshComponent->GetMobility();
            if ( mobility == Mobility::Dynamic )
            {
                auto foundIter = VectorFind( m_staticStaticMeshComponents, pMeshComponent );
                KRG_ASSERT( foundIter != m_staticStaticMeshComponents.end() );
                m_staticStaticMeshComponents.erase( foundIter );

                m_dynamicStaticMeshComponents.push_back( pMeshComponent );
            }
            else // Make static
            {
                auto foundIter = VectorFind( m_dynamicStaticMeshComponents, pMeshComponent );
                KRG_ASSERT( foundIter != m_dynamicStaticMeshComponents.end() );
                m_dynamicStaticMeshComponents.erase( foundIter );

                m_staticStaticMeshComponents.emplace_back( pMeshComponent );
            }
        }

        m_mobilityUpdateList.clear();

        //-------------------------------------------------------------------------
        // Culling
        //-------------------------------------------------------------------------
        // TODO: Broad phase culling

        m_visibleStaticMeshComponents.clear();

        for ( auto pMeshComponent : m_staticStaticMeshComponents )
        {
            m_visibleStaticMeshComponents.emplace_back( pMeshComponent );
        }

        for ( auto pMeshComponent : m_dynamicStaticMeshComponents )
        {
            m_visibleStaticMeshComponents.emplace_back( pMeshComponent );
        }

        //-------------------------------------------------------------------------

        m_visibleSkeletalMeshComponents.clear();

        for ( auto const& meshGroup : m_skeletalMeshGroups )
        {
            for ( auto pMeshComponent : meshGroup.m_components )
            {
                m_visibleSkeletalMeshComponents.emplace_back( pMeshComponent );
            }
        }

        //-------------------------------------------------------------------------
        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        Debug::DrawingContext drawCtx = ctx.GetDrawingContext();

        if ( g_showStaticMeshBounds )
        {
            for ( auto const& record : m_registeredStaticMeshComponents )
            {
                if ( !record.IsSet() )
                {
                    continue;
                }

                drawCtx.DrawWireBox( record.m_pComponent->GetWorldBounds(), Colors::Cyan );
                drawCtx.DrawWireBox( record.m_pComponent->GetWorldBounds().GetAABB(), Colors::LimeGreen );
            }
        }

        for ( auto const& record : m_registeredSkeletalMeshComponents )
        {
            if ( !record.IsSet() )
            {
                continue;
            }

            for ( auto pMeshComponent : record.m_components )
            {
                if ( g_showSkeletalMeshBounds )
                {
                    drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds(), Colors::Cyan );
                    drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds().GetAABB(), Colors::LimeGreen );
                }

                if ( g_showSkeletalMeshBones )
                {
                    pMeshComponent->DrawPose( drawCtx );
                }

                if ( g_showSkeletalMeshBindPoses )
                {
                    pMeshComponent->GetMesh()->DrawBindPose( drawCtx, pMeshComponent->GetWorldTransform() );
                }
            }
        }
        #endif
    }

    //-------------------------------------------------------------------------

    void WorldRendererSystem::OnStaticMeshMobilityUpdated( StaticMeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );

        Threading::ScopeLock lock( m_mobilityUpdateListLock );
        KRG_ASSERT( !VectorContains( m_mobilityUpdateList, pComponent ) );
        m_mobilityUpdateList.emplace_back( pComponent );
    }
}