#include "Entity.h"
#include "EntitySystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* Entity::StaticTypeInfo = nullptr;

    TMultiUserEventInternal<Entity*> Entity::EntityStateUpdatedEvent;

    //-------------------------------------------------------------------------

    void Entity::CreateSpatialAttachment( Entity* pChildEntity, Entity* pParentEntity, StringID socketID )
    {
        KRG_ASSERT( pChildEntity != nullptr && pChildEntity->IsSpatialEntity() );
        KRG_ASSERT( pParentEntity != nullptr && pParentEntity->IsSpatialEntity() );
        KRG_ASSERT( pChildEntity->m_pParentSpatialEntity == nullptr && !pChildEntity->m_isAttachedToParent );

        // Update entity spatial attachment data
        // TODO: check for cyclic dependencies
        pChildEntity->m_pParentSpatialEntity = pParentEntity;
        pChildEntity->m_parentAttachmentSocketID = socketID;
        pParentEntity->m_attachedEntities.emplace_back( pChildEntity );

        if ( pChildEntity->IsActivated() )
        {
            pChildEntity->AttachToParent();
        }
    }

    void Entity::BreakSpatialAttachment( Entity* pChildEntity )
    {
        KRG_ASSERT( pChildEntity != nullptr && pChildEntity->IsSpatialEntity() );
        KRG_ASSERT( pChildEntity->m_pParentSpatialEntity != nullptr );

        // Deactivate the spatial attachment if we can
        if ( pChildEntity->m_isAttachedToParent )
        {
            pChildEntity->DetachFromParent();
        }

        KRG_ASSERT( !pChildEntity->m_isAttachedToParent );

        // Remove entity spatial attachment links
        auto pParentEntity = pChildEntity->m_pParentSpatialEntity;
        auto iter = eastl::find( pParentEntity->m_attachedEntities.begin(), pParentEntity->m_attachedEntities.end(), pChildEntity );
        KRG_ASSERT( iter != pParentEntity->m_attachedEntities.end() );
        pParentEntity->m_attachedEntities.erase_unsorted( iter );
        pChildEntity->m_parentAttachmentSocketID = StringID::InvalidID;
        pChildEntity->m_pParentSpatialEntity = nullptr;
    }

    void Entity::BreakAllEntitySpatialAttachments( Entity* pSpatialEntity )
    {
        KRG_ASSERT( pSpatialEntity != nullptr && pSpatialEntity->IsSpatialEntity() );

        // Clear any parent attachment we might have
        if ( pSpatialEntity->m_pParentSpatialEntity != nullptr )
        {
            BreakSpatialAttachment( pSpatialEntity );
        }

        // Clear any child attachments
        for ( auto pAttachedEntity : pSpatialEntity->m_attachedEntities )
        {
            KRG_ASSERT( pAttachedEntity->m_pParentSpatialEntity == pSpatialEntity );
            BreakSpatialAttachment( pAttachedEntity );
        }

        KRG_ASSERT( pSpatialEntity->m_attachedEntities.empty() );
    }

    //-------------------------------------------------------------------------

    Entity::~Entity()
    {
        KRG_ASSERT( IsDeactivated() );

        // Break all inter-entity links
        if ( IsSpatialEntity() )
        {
            Entity::BreakAllEntitySpatialAttachments( this );
        }

        // Clean up deferred actions
        for ( auto& action : m_deferredActions )
        {
            // Add actions took ownership of the component to add, so it needs to be deleted
            // All other actions can be ignored
            if ( action.m_type == EntityInternalStateAction::Type::AddComponent )
            {
                auto pComponent = reinterpret_cast<EntityComponent const*>( action.m_ptr );
                KRG::Delete( pComponent );
            }
        }
        m_deferredActions.clear();

        // Destroy Systems
        for ( auto& pSystem : m_systems )
        {
            KRG::Delete( pSystem );
        }
        
        m_systems.clear();

        // Destroy components
        for ( auto& pComponent : m_components )
        {
            KRG::Delete( pComponent );
        }

        m_components.clear();
    }

    //-------------------------------------------------------------------------

    void Entity::LoadComponents( EntityModel::LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status == Status::Unloaded );

        for ( auto pComponent : m_components )
        {
            KRG_ASSERT( pComponent->IsUnloaded() );
            pComponent->Load( loadingContext, m_ID );
        }

        m_status = Status::Loaded;
    }

    void Entity::UnloadComponents( EntityModel::LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status == Status::Loaded );

        for ( auto pComponent : m_components )
        {
            if ( pComponent->IsUnloaded() )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            // Shutdown any initialized components
            if ( pComponent->IsInitialized() )
            {
                // Only deal with system registrations if we were activated
                if ( IsActivated() )
                {
                    // Unregister from local systems
                    for ( auto pSystem : m_systems )
                    {
                        pSystem->UnregisterComponent( pComponent );
                    }

                    // Unregister from global systems
                    loadingContext.m_unregisterFromGlobalSystems( this, pComponent );
                }

                pComponent->Shutdown();
            }

            pComponent->Unload( loadingContext, m_ID );
        }

        m_status = Status::Unloaded;
    }
    
    //-------------------------------------------------------------------------

    void Entity::Activate( EntityModel::LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status == Status::Loaded );
        m_status = Status::Activated;

        // Initialize spatial hierarchy
        //-------------------------------------------------------------------------
        // Transforms are set at serialization time so we have all information available to update the world transforms

        if ( IsSpatialEntity() )
        {
            // Calculate the initial world transform but do not trigger the callback to the components
            m_pRootSpatialComponent->CalculateWorldTransform( false );
        }

        // Register components with systems
        //-------------------------------------------------------------------------

        for ( auto pComponent : m_components )
        {
            if ( pComponent->IsInitialized() )
            {
                for ( auto pSystem : m_systems )
                {
                    pSystem->RegisterComponent( pComponent );
                }

                loadingContext.m_registerWithGlobalSystems( this, pComponent );
            }
        }

        // Generate system update list
        //-------------------------------------------------------------------------

        GenerateSystemUpdateList();

        // Spatial Attachments
        //-------------------------------------------------------------------------
        
        if ( IsSpatialEntity() )
        {
            if ( m_pParentSpatialEntity != nullptr )
            {
                AttachToParent();
            }

            //-------------------------------------------------------------------------

            RefreshEntityAttachments();
        }

        //-------------------------------------------------------------------------

        loadingContext.m_registerEntityUpdate( this );
    }

    void Entity::Deactivate( EntityModel::LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status == Status::Activated );

        loadingContext.m_unregisterEntityUpdate( this );

        // Spatial Attachments
        //-------------------------------------------------------------------------

        if ( m_isAttachedToParent )
        {
            DetachFromParent();
        }

        // Clear systems update list
        //-------------------------------------------------------------------------

        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            m_systemUpdateLists[i].clear();
        }

        // Unregister components from systems
        //-------------------------------------------------------------------------

        for ( auto pComponent : m_components )
        {
            if ( pComponent->IsInitialized() )
            {
                for ( auto pSystem : m_systems )
                {
                    pSystem->UnregisterComponent( pComponent );
                }

                loadingContext.m_unregisterFromGlobalSystems( this, pComponent );
            }
        }

        //-------------------------------------------------------------------------

        m_status = Status::Loaded;
    }

    //-------------------------------------------------------------------------

    void Entity::UpdateSystems( UpdateContext const& context )
    {
        S8 const updateStageIdx = (S8) context.GetUpdateStage();
        for( auto pSystem : m_systemUpdateLists[updateStageIdx] )
        {
            KRG_ASSERT( pSystem->GetRequiredUpdatePriorities().IsUpdateStageEnabled( (UpdateStage) updateStageIdx ) );
            pSystem->Update( context );
        }
    }

    //-------------------------------------------------------------------------

    SpatialEntityComponent* Entity::FindSocketAttachmentComponent( SpatialEntityComponent* pComponentToSearch, StringID socketID ) const
    {
        KRG_ASSERT( pComponentToSearch != nullptr );

        if ( pComponentToSearch->HasSocket( socketID ) )
        {
            return pComponentToSearch;
        }

        for ( auto pChildComponent : pComponentToSearch->m_spatialChildren )
        {
            if ( auto pFoundComponent = FindSocketAttachmentComponent( pChildComponent, socketID ) )
            {
                return pFoundComponent;
            }
        }

        return nullptr;
    }

    void Entity::AttachToParent()
    {
        KRG_ASSERT( IsSpatialEntity() );
        KRG_ASSERT( m_pParentSpatialEntity != nullptr && !m_isAttachedToParent );

        // Find component to attach to
        //-------------------------------------------------------------------------

        auto pParentEntity = m_pParentSpatialEntity;
        SpatialEntityComponent* pParentRootComponent = pParentEntity->m_pRootSpatialComponent;
        if ( m_parentAttachmentSocketID.IsValid() )
        {
            if ( auto pFoundComponent = pParentEntity->FindSocketAttachmentComponent( m_parentAttachmentSocketID ) )
            {
                pParentRootComponent = pFoundComponent;
            }
            else
            {
                KRG_LOG_WARNING( "Entity", "Could not find attachment socket '%s' on entity '%s' (%s)", m_parentAttachmentSocketID.c_str(), pParentEntity->GetName().c_str(), pParentEntity->GetID().ToString().c_str() );
            }
        }

        // Perform attachment
        //-------------------------------------------------------------------------

        KRG_ASSERT( pParentRootComponent != nullptr );

        // Set component hierarchy values
        m_pRootSpatialComponent->m_pSpatialParent = pParentRootComponent;
        m_pRootSpatialComponent->m_parentAttachmentSocketID = m_parentAttachmentSocketID;
        m_pRootSpatialComponent->CalculateWorldTransform();

        // Add to the list of child components on the component to attach to
        pParentRootComponent->m_spatialChildren.emplace_back( m_pRootSpatialComponent );

        //-------------------------------------------------------------------------

        m_isAttachedToParent = true;
    }

    void Entity::DetachFromParent()
    {
        KRG_ASSERT( IsSpatialEntity() );
        KRG_ASSERT( m_pParentSpatialEntity != nullptr && m_isAttachedToParent );

        // Remove from parent component child list
        //-------------------------------------------------------------------------

        auto pParentComponent = m_pRootSpatialComponent->m_pSpatialParent;
        auto foundIter = VectorFind( pParentComponent->m_spatialChildren, m_pRootSpatialComponent );
        KRG_ASSERT( foundIter != pParentComponent->m_spatialChildren.end() );
        pParentComponent->m_spatialChildren.erase_unsorted( foundIter );

        // Remove component hierarchy values
        m_pRootSpatialComponent->m_pSpatialParent = nullptr;
        m_pRootSpatialComponent->m_parentAttachmentSocketID = StringID::InvalidID;

        //-------------------------------------------------------------------------

        m_isAttachedToParent = false;
    }

    void Entity::RefreshEntityAttachments()
    {
        KRG_ASSERT( IsSpatialEntity() );
        for ( auto pAttachedEntity: m_attachedEntities )
        {
            // Only refresh active attachments
            if ( pAttachedEntity->m_isAttachedToParent )
            {
                pAttachedEntity->DetachFromParent();
                pAttachedEntity->AttachToParent();
            }
        }
    }

    //-------------------------------------------------------------------------

    void Entity::GenerateSystemUpdateList()
    {
        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            m_systemUpdateLists[i].clear();

            for ( auto& pSystem : m_systems )
            {
                if ( pSystem->GetRequiredUpdatePriorities().IsUpdateStageEnabled( (UpdateStage) i ) )
                {
                    m_systemUpdateLists[i].push_back( pSystem );
                }
            }

            // Sort update list
            auto comparator = [i] ( IEntitySystem* const& pSystemA, IEntitySystem* const& pSystemB )
            {
                U8 const A = pSystemA->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                U8 const B = pSystemB->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                return A > B;
            };

            eastl::sort( m_systemUpdateLists[i].begin(), m_systemUpdateLists[i].end(), comparator );
        }
    }

    void Entity::CreateSystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        KRG_ASSERT( pSystemTypeInfo != nullptr && pSystemTypeInfo->IsDerivedFrom( IEntitySystem::StaticTypeInfo->m_ID ) );

        #if KRG_DEBUG_INSTRUMENTATION
        // Ensure that we only allow a single system of a specific family
        for ( auto pExistingSystem : m_systems )
        {
            auto const pExistingSystemTypeInfo = pExistingSystem->GetTypeInfo();
            if ( pSystemTypeInfo->IsDerivedFrom( pExistingSystemTypeInfo->m_ID ) || pExistingSystemTypeInfo->IsDerivedFrom( pSystemTypeInfo->m_ID ) )
            {
                KRG_HALT();
            }
        }
        #endif

        // Create the new system and add it
        auto pSystem = (IEntitySystem*) pSystemTypeInfo->m_pTypeHelper->CreateType();
        m_systems.emplace_back( pSystem );
    }

    void Entity::CreateSystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        CreateSystemImmediate( pSystemTypeInfo );
        GenerateSystemUpdateList();

        // If we are already activated, notify the world systems that our entity update requirements may have changed
        if ( IsActivated() )
        {
            loadingContext.m_unregisterEntityUpdate( this );
            loadingContext.m_registerEntityUpdate( this );
        }
    }

    void Entity::DestroySystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        KRG_ASSERT( pSystemTypeInfo != nullptr && pSystemTypeInfo->IsDerivedFrom( IEntitySystem::StaticTypeInfo->m_ID ) );

        S32 const systemIdx = VectorFindIndex( m_systems, pSystemTypeInfo->m_ID, [] ( IEntitySystem* pSystem, TypeSystem::TypeID systemTypeID ) { return pSystem->GetTypeInfo()->m_ID == systemTypeID; } );
        KRG_ASSERT( systemIdx != InvalidIndex );

        KRG::Delete( m_systems[systemIdx] );
        VectorEraseUnsorted( m_systems, systemIdx );
    }

    void Entity::DestroySystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        DestroySystemImmediate( pSystemTypeInfo );
        GenerateSystemUpdateList();

        // If we are already activated, notify the world systems that our entity update requirements may have changed
        if ( IsActivated() )
        {
            loadingContext.m_unregisterEntityUpdate( this );
            loadingContext.m_registerEntityUpdate( this );
        }
    }
    
    //-------------------------------------------------------------------------

    void Entity::AddComponent( EntityComponent* pComponent, UUID const& parentSpatialComponentID )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->GetID().IsValid() );
        KRG_ASSERT( !pComponent->m_entityID.IsValid() && pComponent->IsUnloaded() );
        KRG_ASSERT( !VectorContains( m_components, (EntityComponent*) pComponent ) );

        //-------------------------------------------------------------------------

        SpatialEntityComponent* pSpatialComponent = nullptr;
        if( pComponent->GetTypeInfo()->IsDerivedFrom( SpatialEntityComponent::GetStaticTypeID() ) )
        {
            pSpatialComponent = static_cast<SpatialEntityComponent*>( pComponent );
        }
        else
        {
            // Parent ID can only be set when adding a spatial component
            KRG_ASSERT( !parentSpatialComponentID.IsValid() );
        }

        //-------------------------------------------------------------------------

        if ( IsUnloaded() )
        {
            SpatialEntityComponent* pParentComponent = nullptr;
            if ( parentSpatialComponentID.IsValid() )
            {
                KRG_ASSERT( pSpatialComponent != nullptr );
                S32 const componentIdx = VectorFindIndex( m_components, parentSpatialComponentID, [] ( EntityComponent* pComponent, UUID const& componentID ) { return pComponent->GetID() == componentID; } );
                KRG_ASSERT( componentIdx != InvalidIndex );
                KRG_ASSERT( m_components[componentIdx]->GetTypeInfo()->IsDerivedFrom( SpatialEntityComponent::GetStaticTypeID() ) );
                pParentComponent = (SpatialEntityComponent*) m_components[componentIdx];
            }

            AddComponentImmediate( pComponent, pParentComponent );
        }
        else // Defer the operation to the next loading phase
        {
            auto& action = m_deferredActions.emplace_back( EntityInternalStateAction() );
            action.m_type = EntityInternalStateAction::Type::AddComponent;
            action.m_ptr = pComponent;
            action.m_ID = parentSpatialComponentID;

            // Send notification that the internal state changed
            EntityStateUpdatedEvent.Execute( this );
        }
    }

    void Entity::DestroyComponent( UUID const& componentID )
    {
        S32 const componentIdx = VectorFindIndex( m_components, componentID, [] ( EntityComponent* pComponent, UUID const& componentID ) { return pComponent->GetID() == componentID; } );

        // If you hit this assert either the component doesnt exist or this entity has still to process deferred actions and the component might be in that list
        // We dont support adding and destroying a component in the same frame, please avoid doing stupid things
        KRG_ASSERT( componentIdx != InvalidIndex );
        auto pComponent = m_components[componentIdx];

        //-------------------------------------------------------------------------

        if ( IsUnloaded() )
        {
            // Root removal validation
            if ( pComponent == m_pRootSpatialComponent )
            {
                // You are only remove the root component if it has a single child or no children. Otherwise we cant fix the spatial hierarchy for this entity.
                KRG_ASSERT( m_pRootSpatialComponent->m_spatialChildren.size() <= 1 );
                return;
            }

            DestroyComponentImmediate( pComponent );
        }
        else // Defer the operation to the next loading phase
        {
            auto& action = m_deferredActions.emplace_back( EntityInternalStateAction() );
            action.m_type = EntityInternalStateAction::Type::DestroyComponent;
            action.m_ptr = pComponent;
            KRG_ASSERT( action.m_ptr != nullptr );

            // Send notification that the internal state changed
            EntityStateUpdatedEvent.Execute( this );
        }
    }

    void Entity::AddComponentImmediate( EntityComponent* pComponent, SpatialEntityComponent* pParentSpatialComponent )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->GetID().IsValid() && !pComponent->m_entityID.IsValid() );

        // Update spatial hierarchy
        //-------------------------------------------------------------------------

        bool const isSpatialComponent = pComponent->GetTypeInfo()->IsDerivedFrom( SpatialEntityComponent::GetStaticTypeID() );
        SpatialEntityComponent* pSpatialEntityComponent = isSpatialComponent ? static_cast<SpatialEntityComponent*>( pComponent ) : nullptr;
        if ( pSpatialEntityComponent != nullptr )
        {
            // If the parent component is null, attach it to the root by default
            if ( pParentSpatialComponent == nullptr )
            {
                pParentSpatialComponent = m_pRootSpatialComponent;
            }

            // Update hierarchy and transform
            if ( pParentSpatialComponent != nullptr )
            {
                pSpatialEntityComponent->m_pSpatialParent = pParentSpatialComponent;
                pParentSpatialComponent->m_spatialChildren.emplace_back( pSpatialEntityComponent );
                pSpatialEntityComponent->CalculateWorldTransform( false );
            }
            else // Make this component the root component
            {
                m_pRootSpatialComponent = pSpatialEntityComponent;
                KRG_ASSERT( pSpatialEntityComponent->m_pSpatialParent == nullptr );
            }
        }

        // Add component
        //-------------------------------------------------------------------------

        m_components.emplace_back( pComponent );
        pComponent->m_entityID = m_ID;
    }

    void Entity::AddComponentDeferred( EntityModel::LoadingContext const& loadingContext, EntityComponent* pComponent, SpatialEntityComponent* pParentSpatialComponent )
    {
        KRG_ASSERT( loadingContext.IsValid() );
        AddComponentImmediate( pComponent, pParentSpatialComponent );
        pComponent->Load( loadingContext, m_ID );
    }

    void Entity::DestroyComponentImmediate( EntityComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        S32 const componentIdx = VectorFindIndex( m_components, pComponent );
        KRG_ASSERT( componentIdx != InvalidIndex );

        // Update spatial hierarchy
        //-------------------------------------------------------------------------

        SpatialEntityComponent* pSpatialComponent = nullptr;
        if ( pComponent->GetTypeInfo()->IsDerivedFrom( SpatialEntityComponent::GetStaticTypeID() ) )
        {
            pSpatialComponent = static_cast<SpatialEntityComponent*>( pComponent );

            if ( pSpatialComponent->IsRootComponent() )
            {
                KRG_ASSERT( pSpatialComponent == m_pRootSpatialComponent );
                S32 const numChildrenForRoot = (S32) m_pRootSpatialComponent->m_spatialChildren.size();

                // Removal of the root component if it has more than one child is not supported!
                if ( numChildrenForRoot > 1 )
                {
                    KRG_HALT();
                }
                else if ( numChildrenForRoot == 1 )
                {
                    // Replace the root component of this entity with the child of the current root component that we are removing
                    pSpatialComponent->m_spatialChildren[0]->m_pSpatialParent = nullptr;
                    m_pRootSpatialComponent = pSpatialComponent->m_spatialChildren[0];
                    pSpatialComponent->m_spatialChildren.clear();
                }
                else // No children, so completely remove the root component
                {
                    m_pRootSpatialComponent = nullptr;
                }
            }
            else // 'Fix' spatial hierarchy parenting
            {
                auto pParentComponent = pSpatialComponent->m_pSpatialParent;
                KRG_ASSERT( pParentComponent != nullptr );

                // Remove from parent
                pParentComponent->m_spatialChildren.erase_first_unsorted( pSpatialComponent );

                // Reparent all children to parent component
                for ( auto pChildComponent : pSpatialComponent->m_spatialChildren )
                {
                    pChildComponent->m_pSpatialParent = pParentComponent;
                    pParentComponent->m_spatialChildren.emplace_back( pChildComponent );
                }
            }
        }

        // Remove component
        //-------------------------------------------------------------------------

        VectorEraseUnsorted( m_components, componentIdx );
        KRG::Delete( pComponent );
    }

    void Entity::DestroyComponentDeferred( EntityModel::LoadingContext const& loadingContext, EntityComponent* pComponent )
    {
        KRG_ASSERT( loadingContext.IsValid() );

        // Shutdown and unload component
        //-------------------------------------------------------------------------

        if ( pComponent->IsInitialized() )
        {
            // Remove any system registrations
            if ( IsActivated() )
            {
                // Remove from local systems
                for ( auto pSystem : m_systems )
                {
                    pSystem->UnregisterComponent( pComponent );
                }

                // Remove from global systems
                loadingContext.m_unregisterFromGlobalSystems( this, pComponent );
            }

            pComponent->Shutdown();
        }

        pComponent->Unload( loadingContext, m_ID );

        // Destroy the component
        //-------------------------------------------------------------------------

        DestroyComponentImmediate( pComponent );
    }

    //-------------------------------------------------------------------------

    bool Entity::UpdateLoadingAndEntityState( EntityModel::LoadingContext const& loadingContext )
    {
        //-------------------------------------------------------------------------
        // Deferred Actions
        //-------------------------------------------------------------------------

        for ( auto const& action : m_deferredActions )
        {
            switch ( action.m_type )
            {
                case EntityInternalStateAction::Type::CreateSystem : 
                {
                    CreateSystemDeferred( loadingContext, (TypeSystem::TypeInfo const*) action.m_ptr );
                }
                break;

                case EntityInternalStateAction::Type::DestroySystem:
                {
                    DestroySystemDeferred( loadingContext, (TypeSystem::TypeInfo const*) action.m_ptr );
                }
                break;

                case EntityInternalStateAction::Type::AddComponent:
                {
                    SpatialEntityComponent* pParentComponent = nullptr;

                    // Try to resolve the ID
                    if ( action.m_ID.IsValid() )
                    {
                        S32 const componentIdx = VectorFindIndex( m_components, action.m_ID, [] ( EntityComponent* pComponent, UUID const& componentID ) { return pComponent->GetID() == componentID; } );
                        KRG_ASSERT( componentIdx != InvalidIndex );
                        KRG_ASSERT( m_components[componentIdx]->GetTypeInfo()->IsDerivedFrom( SpatialEntityComponent::GetStaticTypeID() ) );
                        pParentComponent = (SpatialEntityComponent*) m_components[componentIdx];
                    }

                    AddComponentDeferred( loadingContext, (EntityComponent*) action.m_ptr, pParentComponent );
                }
                break;

                case EntityInternalStateAction::Type::DestroyComponent:
                {
                    DestroyComponentDeferred( loadingContext, (EntityComponent*) action.m_ptr );
                }
                break;
            }
        }

        m_deferredActions.clear();

        //-------------------------------------------------------------------------
        // Component Loading
        //-------------------------------------------------------------------------

        for ( auto pComponent : m_components )
        {
            if ( pComponent->IsLoading() )
            {
                pComponent->UpdateLoading();

                // If we are still loading, return
                if ( pComponent->IsLoading() )
                {
                    return false;
                }

                // Once we finish loading a component immediately initialize it
                if ( pComponent->IsLoaded() )
                {
                    pComponent->Initialize();
                    KRG_ASSERT( pComponent->IsInitialized() );

                    // If we are already activated, then register with entity systems
                    if ( IsActivated() )
                    {
                        // Register with local systems
                        for ( auto pSystem : m_systems )
                        {
                            pSystem->RegisterComponent( pComponent );
                        }

                        // Register with global systems
                        loadingContext.m_registerWithGlobalSystems( this, pComponent );
                    }
                }
            }
        }

        return true;
    }
}