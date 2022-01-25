#include "WorldSystem_WorldRenderer.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "Engine/Render/Components/Component_StaticMesh.h"
#include "Engine/Render/Components/Component_SkeletalMesh.h"
#include "Engine/Render/Components/Component_Lights.h"
#include "Engine/Render/Components/Component_EnvironmentMaps.h"
#include "Engine/Render/Shaders/EngineShaders.h"
#include "System/Render/RenderDefaultResources.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Logging/Log.h"

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

    void RendererWorldSystem::InitializeSystem( SystemRegistry const& systemRegistry )
    {
        m_staticMeshMobilityChangedEventBinding = StaticMeshComponent::OnMobilityChanged().Bind( [this] ( StaticMeshComponent* pMeshComponent ) { OnStaticMeshMobilityUpdated( pMeshComponent ); } );
        m_staticMeshStaticTransformUpdatedEventBinding = StaticMeshComponent::OnStaticMobilityTransformUpdated().Bind( [this] ( StaticMeshComponent* pMeshComponent ) { OnStaticMobilityComponentTransformUpdated( pMeshComponent ); } );
    }

    void RendererWorldSystem::ShutdownSystem()
    {
        // Unbind mobility change handler and remove from various lists
        StaticMeshComponent::OnStaticMobilityTransformUpdated().Unbind( m_staticMeshStaticTransformUpdatedEventBinding );
        StaticMeshComponent::OnMobilityChanged().Unbind( m_staticMeshMobilityChangedEventBinding );

        KRG_ASSERT( m_registeredStaticMeshComponents.empty() );
        KRG_ASSERT( m_registeredSkeletalMeshComponents.empty() );
        KRG_ASSERT( m_skeletalMeshGroups.empty() );

        KRG_ASSERT( m_registeredDirectionLightComponents.empty() );
        KRG_ASSERT( m_registeredPointLightComponents.empty() );
        KRG_ASSERT( m_registeredSpotLightComponents.empty() );

        KRG_ASSERT( m_registeredLocalEnvironmentMaps.empty() );
        KRG_ASSERT( m_registeredGlobalEnvironmentMaps.empty() );
    }

    void RendererWorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        // Meshes
        //-------------------------------------------------------------------------

        if ( auto pStaticMeshComponent = TryCast<StaticMeshComponent>( pComponent ) )
        {
            RegisterStaticMeshComponent( pEntity, pStaticMeshComponent );
        }
        else if ( auto pSkeletalMeshComponent = TryCast<SkeletalMeshComponent>( pComponent ) )
        {
            RegisterSkeletalMeshComponent( pEntity, pSkeletalMeshComponent );
        }
        
        // Lights
        //-------------------------------------------------------------------------

        else if ( auto pLightComponent = TryCast<LightComponent>( pComponent ) )
        {
            if ( auto pDirectionalLightComponent = TryCast<DirectionalLightComponent>( pComponent ) )
            {
                m_registeredDirectionLightComponents.Add( pDirectionalLightComponent );
            }
            else if ( auto pPointLightComponent = TryCast<PointLightComponent>( pComponent ) )
            {
                m_registeredPointLightComponents.Add( pPointLightComponent );
            }
            else if ( auto pSpotLightComponent = TryCast<SpotLightComponent>( pComponent ) )
            {
                m_registeredSpotLightComponents.Add( pSpotLightComponent );
            }
        }

        // Environment Maps
        //-------------------------------------------------------------------------

        else if ( auto pLocalEnvMapComponent = TryCast<LocalEnvironmentMapComponent>( pComponent ) )
        {
            m_registeredLocalEnvironmentMaps.Add( pLocalEnvMapComponent );
        }
        else if ( auto pGlobalEnvMapComponent = TryCast<GlobalEnvironmentMapComponent>( pComponent ) )
        {
            m_registeredGlobalEnvironmentMaps.Add( pGlobalEnvMapComponent );
        }
    }

    void RendererWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        // Meshes
        //-------------------------------------------------------------------------

        if ( auto pStaticMeshComponent = TryCast<StaticMeshComponent>( pComponent ) )
        {
            UnregisterStaticMeshComponent( pEntity, pStaticMeshComponent );
        }
        else if ( auto pSkeletalMeshComponent = TryCast<SkeletalMeshComponent>( pComponent ) )
        {
            UnregisterSkeletalMeshComponent( pEntity, pSkeletalMeshComponent );
        }

        // Lights
        //-------------------------------------------------------------------------

        else if ( auto pLightComponent = TryCast<LightComponent>( pComponent ) )
        {
            if ( auto pDirectionalLightComponent = TryCast<DirectionalLightComponent>( pComponent ) )
            {
                m_registeredDirectionLightComponents.Remove( pDirectionalLightComponent->GetID() );
            }
            else if ( auto pPointLightComponent = TryCast<PointLightComponent>( pComponent ) )
            {
                m_registeredPointLightComponents.Remove( pPointLightComponent->GetID() );
            }
            else if ( auto pSpotLightComponent = TryCast<SpotLightComponent>( pComponent ) )
            {
                m_registeredSpotLightComponents.Remove( pSpotLightComponent->GetID() );
            }
        }

        // Environment Maps
        //-------------------------------------------------------------------------

        else if ( auto pLocalEnvMapComponent = TryCast<LocalEnvironmentMapComponent>( pComponent ) )
        {
            m_registeredLocalEnvironmentMaps.Remove( pLocalEnvMapComponent->GetID() );
        }
        else if ( auto pGlobalEnvMapComponent = TryCast<GlobalEnvironmentMapComponent>( pComponent ) )
        {
            m_registeredGlobalEnvironmentMaps.Remove( pGlobalEnvMapComponent->GetID() );
        }
    }

    void RendererWorldSystem::RegisterStaticMeshComponent( Entity const* pEntity, StaticMeshComponent* pMeshComponent )
    {
        m_registeredStaticMeshComponents.Add( pMeshComponent );

        //-------------------------------------------------------------------------

        // Add to appropriate sub-list
        if ( pMeshComponent->HasMeshResourceSet() )
        {
            if ( pMeshComponent->GetMobility() == Mobility::Dynamic )
            {
                m_dynamicStaticMeshComponents.Add( pMeshComponent );
            }
            else
            {
                m_staticStaticMeshComponents.Add( pMeshComponent );
            }
        }
    }

    void RendererWorldSystem::UnregisterStaticMeshComponent( Entity const* pEntity, StaticMeshComponent* pMeshComponent )
    {
        // Unregistrations occur at the start of the frame
        // The world might be paused so we might leave an invalid component in this array
        m_visibleStaticMeshComponents.clear();

        if ( pMeshComponent->HasMeshResourceSet() )
        {
            // Get the real mobility of the component
            Mobility realMobility = pMeshComponent->GetMobility();
            int32 const mobilityListIdx = VectorFindIndex( m_mobilityUpdateList, pMeshComponent );
            if ( mobilityListIdx != InvalidIndex )
            {
                realMobility = ( realMobility == Mobility::Dynamic ) ? Mobility::Static : Mobility::Dynamic;
                m_mobilityUpdateList.erase_unsorted( m_mobilityUpdateList.begin() + mobilityListIdx );
            }

            // Remove from the relevant runtime list
            if ( realMobility == Mobility::Dynamic )
            {
                m_dynamicStaticMeshComponents.Remove( pMeshComponent->GetID() );
            }
            else
            {
                m_staticStaticMeshComponents.Remove( pMeshComponent->GetID() );
            }
        }

        // Remove record
        m_registeredStaticMeshComponents.Remove( pMeshComponent->GetID() );
    }

    void RendererWorldSystem::RegisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent )
    {
        m_registeredSkeletalMeshComponents.Add( pMeshComponent );

        // Add to mesh groups
        //-------------------------------------------------------------------------

        if ( pMeshComponent->HasMeshResourceSet() )
        {
            auto pMesh = pMeshComponent->GetMesh();
            KRG_ASSERT( pMesh != nullptr && pMesh->IsValid() );
            uint32 const meshID = pMesh->GetResourceID().GetID();

            auto pMeshGroup = m_skeletalMeshGroups.FindOrAdd( meshID, pMesh );
            pMeshGroup->m_components.emplace_back( pMeshComponent );
        }
    }

    void RendererWorldSystem::UnregisterSkeletalMeshComponent( Entity const* pEntity, SkeletalMeshComponent* pMeshComponent )
    {
        // Unregistrations occur at the start of the frame
        // The world might be paused so we might leave an invalid component in this array
        m_visibleSkeletalMeshComponents.clear();

        // Remove component from mesh group
        if ( pMeshComponent->HasMeshResourceSet() )
        {
            uint32 const meshID = pMeshComponent->GetMesh()->GetResourceID().GetID();
            auto pMeshGroup = m_skeletalMeshGroups.Get( meshID );
            pMeshGroup->m_components.erase_first_unsorted( pMeshComponent );

            // Remove empty groups
            if ( pMeshGroup->m_components.empty() )
            {
                m_skeletalMeshGroups.Remove( meshID );
            }
        }

        // Remove record
        m_registeredSkeletalMeshComponents.Remove( pMeshComponent->GetID() );
    }

    //-------------------------------------------------------------------------

    void RendererWorldSystem::UpdateSystem( EntityWorldUpdateContext const& ctx )
    {
        KRG_PROFILE_FUNCTION_RENDER();

        if ( ctx.IsWorldPaused() && ctx.GetUpdateStage() != UpdateStage::Paused )
        {
            return;
        }

        KRG_ASSERT( ( ctx.GetUpdateStage() == UpdateStage::Paused ) ? ctx.IsWorldPaused() : true );

        //-------------------------------------------------------------------------
        // Mobility Updates
        //-------------------------------------------------------------------------

        for ( auto pMeshComponent : m_mobilityUpdateList )
        {
            Mobility const mobility = pMeshComponent->GetMobility();
            if ( mobility == Mobility::Dynamic )
            {
                m_staticStaticMeshComponents.Remove( pMeshComponent->GetID() );
                m_dynamicStaticMeshComponents.Add( pMeshComponent );
            }
            else // Make static
            {
                m_dynamicStaticMeshComponents.Remove( pMeshComponent->GetID() );
                m_staticStaticMeshComponents.Add( pMeshComponent );
            }
        }

        m_mobilityUpdateList.clear();

        //-------------------------------------------------------------------------

        for ( auto pMeshComponent : m_staticMobilityTransformUpdateList )
        {
            if ( ctx.IsGameWorld() )
            {
                KRG_LOG_ERROR( "Render", "Someone moved a mesh with static mobility: %s with entity ID %u. This should not be done!", pMeshComponent->GetName().c_str(), pMeshComponent->GetEntityID().m_ID );
            }

            // Right now there is nothing to do, in future we need to remove from the AABB and re-add to the AABB
        }

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
        Drawing::DrawContext drawCtx = ctx.GetDrawingContext();

        if ( g_showStaticMeshBounds )
        {
            for ( auto const& pMeshComponent : m_registeredStaticMeshComponents )
            {
                drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds(), Colors::Cyan );
                drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds().GetAABB(), Colors::LimeGreen );
            }
        }

        for ( auto const& pMeshComponent : m_registeredSkeletalMeshComponents )
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
        #endif
    }

    //-------------------------------------------------------------------------

    void RendererWorldSystem::OnStaticMeshMobilityUpdated( StaticMeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );

        if ( m_registeredStaticMeshComponents.HasItemForID( pComponent->GetID() ) )
        {
            Threading::ScopeLock lock( m_mobilityUpdateListLock );
            KRG_ASSERT( !VectorContains( m_mobilityUpdateList, pComponent ) );
            m_mobilityUpdateList.emplace_back( pComponent );
        }
    }

    void RendererWorldSystem::OnStaticMobilityComponentTransformUpdated( StaticMeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );
        KRG_ASSERT( pComponent->GetMobility() == Mobility::Static );

        if ( m_registeredStaticMeshComponents.HasItemForID( pComponent->GetID() ) )
        {
            Threading::ScopeLock lock( m_mobilityUpdateListLock );
            if ( !VectorContains( m_staticMobilityTransformUpdateList, pComponent ) )
            {
                m_staticMobilityTransformUpdateList.emplace_back( pComponent );
            }
        }
    }
}