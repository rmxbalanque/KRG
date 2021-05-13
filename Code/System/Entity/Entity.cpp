#include "Entity.h"
#include "EntitySystem.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TMultiUserEventInternal<Entity*> Entity::EntityStateUpdatedEvent;

    //-------------------------------------------------------------------------

    Entity::~Entity()
    {
        KRG_ASSERT( m_status == Status::Unloaded );
        KRG_ASSERT( !m_isSpatialAttachmentCreated );
        KRG_ASSERT( !m_isRegisteredForUpdates );

        // Break all inter-entity links
        if ( IsSpatialEntity() )
        {
            // Clear any child attachments
            for ( auto pAttachedEntity : m_attachedEntities )
            {
                KRG_ASSERT( pAttachedEntity->m_pParentSpatialEntity == this );
                pAttachedEntity->ClearSpatialParent();
            }

            // Clear any parent attachment we might have
            if ( m_pParentSpatialEntity != nullptr )
            {
                ClearSpatialParent();
            }
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

            if ( pComponent->IsInitialized() )
            {
                pComponent->Shutdown();
                KRG_ASSERT( !pComponent->IsInitialized() ); // Did you forget to call the parent class shutdown?
            }

            pComponent->Unload( loadingContext, m_ID );
        }

        m_status = Status::Unloaded;
    }
    
    //-------------------------------------------------------------------------

    void Entity::Activate( EntityModel::LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext )
    {
        KRG_ASSERT( m_status == Status::Loaded );

        // If we are attached to another entity we CANNOT be activated if our parent is not. This ensures that attachment chains have consistent activation state
        if ( HasSpatialParent() )
        {
            KRG_ASSERT( m_pParentSpatialEntity->IsActivated() );
        }

        // Update internal status
        //-------------------------------------------------------------------------

        m_status = Status::Activated;

        // Initialize spatial hierarchy
        //-------------------------------------------------------------------------
        // Transforms are set at serialization time so we have all information available to update the world transforms

        if ( IsSpatialEntity() )
        {
            // Calculate the initial world transform but do not trigger the callback to the components
            m_pRootSpatialComponent->CalculateWorldTransform( false );
        }

        // Components
        //-------------------------------------------------------------------------

        for ( auto pComponent : m_components )
        {
            if ( pComponent->IsInitialized() )
            {
                RegisterComponentWithLocalSystems( pComponent );
                activationContext.m_componentsToRegister.enqueue( TPair<Entity*, EntityComponent*>( this, pComponent ) );
            }
        }

        // Systems
        //-------------------------------------------------------------------------

        if ( !m_systems.empty() )
        {
            GenerateSystemUpdateList();
            activationContext.m_registerForEntityUpdate.enqueue( this );
        }

        // Spatial Attachments
        //-------------------------------------------------------------------------
        
        if ( IsSpatialEntity() )
        {
            if ( m_pParentSpatialEntity != nullptr )
            {
                CreateSpatialAttachment();
            }

            //-------------------------------------------------------------------------

            RefreshChildSpatialAttachments();
        }

        // Activate attached entities
        //-------------------------------------------------------------------------

        for ( auto pAttachedEntity : m_attachedEntities )
        {
            KRG_ASSERT( !pAttachedEntity->IsActivated() );
            if ( pAttachedEntity->IsLoaded() )
            {
                pAttachedEntity->Activate( loadingContext, activationContext );
            }
        }
    }

    void Entity::Deactivate( EntityModel::LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext )
    {
        KRG_ASSERT( m_status == Status::Activated );

        // If we are attached to another entity, that entity must also have been activated, else we have a problem in our attachment chains
        if ( HasSpatialParent() )
        {
            KRG_ASSERT( m_pParentSpatialEntity->IsActivated() );
        }

        // Deactivate attached entities
        //-------------------------------------------------------------------------

        for ( auto pAttachedEntity : m_attachedEntities )
        {
            if ( pAttachedEntity->IsActivated() )
            {
                pAttachedEntity->Deactivate( loadingContext, activationContext );
            }
        }

        // Spatial Attachments
        //-------------------------------------------------------------------------

        if ( m_isSpatialAttachmentCreated )
        {
            DestroySpatialAttachment();
        }

        // Systems
        //-------------------------------------------------------------------------

        if ( !m_systems.empty() )
        {
            if ( m_isRegisteredForUpdates )
            {
                activationContext.m_unregisterForEntityUpdate.enqueue( this );
            }

            for ( int8 i = 0; i < (int8) UpdateStage::NumStages; i++ )
            {
                m_systemUpdateLists[i].clear();
            }
        }

        // Components
        //-------------------------------------------------------------------------

        for ( auto pComponent : m_components )
        {
            if ( pComponent->m_isRegisteredWithWorld )
            {
                activationContext.m_componentsToUnregister.enqueue( TPair<Entity*, EntityComponent*>( this, pComponent ) );
            }

            if ( pComponent->m_isRegisteredWithEntity )
            {
                UnregisterComponentFromLocalSystems( pComponent );
            }
        }

        //-------------------------------------------------------------------------

        m_status = Status::Loaded;
    }

    //-------------------------------------------------------------------------

    void Entity::UpdateSystems( UpdateContext const& context )
    {
        int8 const updateStageIdx = (int8) context.GetUpdateStage();
        for( auto pSystem : m_systemUpdateLists[updateStageIdx] )
        {
            KRG_ASSERT( pSystem->GetRequiredUpdatePriorities().IsStageEnabled( (UpdateStage) updateStageIdx ) );
            pSystem->Update( context );
        }
    }

    void Entity::RegisterComponentWithLocalSystems( EntityComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() && pComponent->m_entityID == m_ID && !pComponent->m_isRegisteredWithEntity );

        for ( auto pSystem : m_systems )
        {
            pSystem->RegisterComponent( pComponent );
            pComponent->m_isRegisteredWithEntity = true;
        }
    }

    void Entity::UnregisterComponentFromLocalSystems( EntityComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() && pComponent->m_entityID == m_ID && pComponent->m_isRegisteredWithEntity );

        for ( auto pSystem : m_systems )
        {
            pSystem->UnregisterComponent( pComponent );
            pComponent->m_isRegisteredWithEntity = false;
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

    void Entity::SetSpatialParent( Entity* pParentEntity, StringID attachmentSocketID )
    {
        KRG_ASSERT( IsSpatialEntity() );
        KRG_ASSERT( m_pParentSpatialEntity == nullptr && !m_isSpatialAttachmentCreated);
        KRG_ASSERT( pParentEntity != nullptr && pParentEntity->IsSpatialEntity() );

        // Check for circular dependencies
        #if KRG_DEVELOPMENT_TOOLS
        Entity const* pAncestorEntity = pParentEntity->GetSpatialParent();
        while ( pAncestorEntity != nullptr )
        {
            KRG_ASSERT( pAncestorEntity != this );
            pAncestorEntity = pAncestorEntity->GetSpatialParent();
        }
        #endif

        // Set attachment data
        m_pParentSpatialEntity = pParentEntity;
        m_parentAttachmentSocketID = attachmentSocketID;

        // Add myself to the parent's list of attached entities
        {
            Threading::ScopeLock lock( pParentEntity->m_internalStateMutex );
            pParentEntity->m_attachedEntities.emplace_back( this );
        }
    }

    void Entity::ClearSpatialParent()
    {
        KRG_ASSERT( IsSpatialEntity() );
        KRG_ASSERT( HasSpatialParent() && !m_isSpatialAttachmentCreated );

        // Remove myself from parent's attached entity list
        auto iter = eastl::find( m_pParentSpatialEntity->m_attachedEntities.begin(), m_pParentSpatialEntity->m_attachedEntities.end(), this );
        KRG_ASSERT( iter != m_pParentSpatialEntity->m_attachedEntities.end() );
        m_pParentSpatialEntity->m_attachedEntities.erase_unsorted( iter );

        // Clear attachment data
        m_parentAttachmentSocketID = StringID::InvalidID;
        m_pParentSpatialEntity = nullptr;
    }

    void Entity::CreateSpatialAttachment()
    {
        KRG_ASSERT( IsSpatialEntity() );
        KRG_ASSERT( m_pParentSpatialEntity != nullptr && !m_isSpatialAttachmentCreated );
        KRG_ASSERT( IsActivated() && m_pParentSpatialEntity->IsActivated() );

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
        {
            Threading::ScopeLock lock( pParentEntity->m_internalStateMutex );
            pParentRootComponent->m_spatialChildren.emplace_back( m_pRootSpatialComponent );
        }

        //-------------------------------------------------------------------------

        m_isSpatialAttachmentCreated = true;

        //-------------------------------------------------------------------------

        // Send notification that the internal state changed
        EntityStateUpdatedEvent.Execute( this );
    }

    void Entity::DestroySpatialAttachment()
    {
        KRG_ASSERT( IsSpatialEntity() );
        KRG_ASSERT( IsActivated() );
        KRG_ASSERT( m_pParentSpatialEntity != nullptr && m_isSpatialAttachmentCreated );

        // Remove from parent component child list
        //-------------------------------------------------------------------------

        auto pParentComponent = m_pRootSpatialComponent->m_pSpatialParent;
        auto foundIter = VectorFind( pParentComponent->m_spatialChildren, m_pRootSpatialComponent );
        KRG_ASSERT( foundIter != pParentComponent->m_spatialChildren.end() );

        {
            Threading::ScopeLock lock( m_pParentSpatialEntity->m_internalStateMutex );
            pParentComponent->m_spatialChildren.erase_unsorted( foundIter );
        }

        // Remove component hierarchy values
        m_pRootSpatialComponent->m_pSpatialParent = nullptr;
        m_pRootSpatialComponent->m_parentAttachmentSocketID = StringID::InvalidID;

        //-------------------------------------------------------------------------

        m_isSpatialAttachmentCreated = false;

        //-------------------------------------------------------------------------

        // Send notification that the internal state changed
        EntityStateUpdatedEvent.Execute( this );
    }

    void Entity::RefreshChildSpatialAttachments()
    {
        KRG_ASSERT( IsSpatialEntity() );
        for ( auto pAttachedEntity: m_attachedEntities )
        {
            // Only refresh active attachments
            if ( pAttachedEntity->m_isSpatialAttachmentCreated )
            {
                pAttachedEntity->DestroySpatialAttachment();
                pAttachedEntity->CreateSpatialAttachment();
            }
        }
    }

    void Entity::RemoveComponentFromSpatialHierarchy( SpatialEntityComponent* pSpatialComponent )
    {
        KRG_ASSERT( pSpatialComponent != nullptr );

        if ( pSpatialComponent == m_pRootSpatialComponent )
        {
            int32 const numChildrenForRoot = (int32) m_pRootSpatialComponent->m_spatialChildren.size();

            // Ensure that we break any spatial attachments before we mess with the root component
            bool const recreateSpatialAttachment = m_isSpatialAttachmentCreated;
            if ( HasSpatialParent() && m_isSpatialAttachmentCreated )
            {
                DestroySpatialAttachment();
            }

            //-------------------------------------------------------------------------

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

                // We are no longer a spatial entity so break any attachments
                if ( HasSpatialParent() )
                {
                    ClearSpatialParent();
                }
            }

            //-------------------------------------------------------------------------

            // Should we recreate the spatial attachment
            if ( HasSpatialParent() && recreateSpatialAttachment )
            {
                CreateSpatialAttachment();
            }
        }
        else // 'Fix' spatial hierarchy parenting
        {
            auto pParentComponent = pSpatialComponent->m_pSpatialParent;
            if ( pParentComponent != nullptr ) // Component may already have been removed from the hierarchy
            {
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
    }

    //-------------------------------------------------------------------------

    void Entity::GenerateSystemUpdateList()
    {
        for ( int8 i = 0; i < (int8) UpdateStage::NumStages; i++ )
        {
            m_systemUpdateLists[i].clear();

            for ( auto& pSystem : m_systems )
            {
                if ( pSystem->GetRequiredUpdatePriorities().IsStageEnabled( (UpdateStage) i ) )
                {
                    m_systemUpdateLists[i].push_back( pSystem );
                }
            }

            // Sort update list
            auto comparator = [i] ( IEntitySystem* const& pSystemA, IEntitySystem* const& pSystemB )
            {
                uint8 const A = pSystemA->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                uint8 const B = pSystemB->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                return A > B;
            };

            eastl::sort( m_systemUpdateLists[i].begin(), m_systemUpdateLists[i].end(), comparator );
        }
    }

    void Entity::CreateSystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        KRG_ASSERT( pSystemTypeInfo != nullptr && pSystemTypeInfo->IsDerivedFrom<IEntitySystem>() );

        #if KRG_DEVELOPMENT_TOOLS
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

        // Register all components with the new system
        for ( auto pComponent : m_components )
        {
            if ( pComponent->m_isRegisteredWithEntity )
            {
                pSystem->RegisterComponent( pComponent );
            }
        }
    }

    void Entity::CreateSystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        CreateSystemImmediate( pSystemTypeInfo );
        GenerateSystemUpdateList();
    }

    void Entity::DestroySystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        KRG_ASSERT( pSystemTypeInfo != nullptr && pSystemTypeInfo->IsDerivedFrom<IEntitySystem>() );

        int32 const systemIdx = VectorFindIndex( m_systems, pSystemTypeInfo->m_ID, [] ( IEntitySystem* pSystem, TypeSystem::TypeID systemTypeID ) { return pSystem->GetTypeInfo()->m_ID == systemTypeID; } );
        KRG_ASSERT( systemIdx != InvalidIndex );
        auto pSystem = m_systems[systemIdx];

        // Unregister all components from the system to remove
        for ( auto pComponent : m_components )
        {
            if ( pComponent->m_isRegisteredWithEntity )
            {
                pSystem->UnregisterComponent( pComponent );
            }
        }

        // Destroy the system
        KRG::Delete( pSystem );
        m_systems.erase_unsorted( m_systems.begin() + systemIdx );
    }

    void Entity::DestroySystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        DestroySystemImmediate( pSystemTypeInfo );
        GenerateSystemUpdateList();
    }
    
    //-------------------------------------------------------------------------

    void Entity::AddComponent( EntityComponent* pComponent, UUID const& parentSpatialComponentID )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->GetID().IsValid() );
        KRG_ASSERT( !pComponent->m_entityID.IsValid() && pComponent->IsUnloaded() );
        KRG_ASSERT( !VectorContains( m_components, (EntityComponent*) pComponent ) );

        //-------------------------------------------------------------------------

        SpatialEntityComponent* pSpatialComponent = ComponentCast<SpatialEntityComponent>( pComponent );
        
        // Parent ID can only be set when adding a spatial component
        if( pSpatialComponent == nullptr )
        {
            KRG_ASSERT( !parentSpatialComponentID.IsValid() );
        }

        //-------------------------------------------------------------------------

        if ( IsUnloaded() )
        {
            SpatialEntityComponent* pParentComponent = nullptr;
            if ( parentSpatialComponentID.IsValid() )
            {
                KRG_ASSERT( pSpatialComponent != nullptr );
                int32 const componentIdx = VectorFindIndex( m_components, parentSpatialComponentID, [] ( EntityComponent* pComponent, UUID const& componentID ) { return pComponent->GetID() == componentID; } );
                KRG_ASSERT( componentIdx != InvalidIndex );

                pParentComponent = ComponentCast<SpatialEntityComponent>( m_components[componentIdx] );
                KRG_ASSERT( pParentComponent != nullptr );
            }

            AddComponentImmediate( pComponent, pParentComponent );
        }
        else // Defer the operation to the next loading phase
        {
            Threading::ScopeLock lock( m_internalStateMutex );

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
        int32 const componentIdx = VectorFindIndex( m_components, componentID, [] ( EntityComponent* pComponent, UUID const& componentID ) { return pComponent->GetID() == componentID; } );

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
            Threading::ScopeLock lock( m_internalStateMutex );

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
        KRG_ASSERT( pComponent->IsUnloaded() && !pComponent->m_isRegisteredWithWorld && !pComponent->m_isRegisteredWithEntity );

        // Update spatial hierarchy
        //-------------------------------------------------------------------------

        SpatialEntityComponent* pSpatialEntityComponent = ComponentCast<SpatialEntityComponent>( pComponent );
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
        KRG_ASSERT( pComponent->IsUnloaded() && !pComponent->m_isRegisteredWithWorld && !pComponent->m_isRegisteredWithEntity );

        int32 const componentIdx = VectorFindIndex( m_components, pComponent );
        KRG_ASSERT( componentIdx != InvalidIndex );

        // Update spatial hierarchy
        //-------------------------------------------------------------------------

        SpatialEntityComponent* pSpatialComponent = ComponentCast<SpatialEntityComponent>( pComponent );
        if ( pSpatialComponent != nullptr )
        {
            RemoveComponentFromSpatialHierarchy( pSpatialComponent );
        }

        // Remove component
        //-------------------------------------------------------------------------

        m_components.erase_unsorted( m_components.begin() + componentIdx );
        KRG::Delete( pComponent );
    }

    void Entity::DestroyComponentDeferred( EntityModel::LoadingContext const& loadingContext, EntityComponent* pComponent )
    {
        KRG_ASSERT( loadingContext.IsValid() );

        // Shutdown and unload component
        //-------------------------------------------------------------------------

        if ( pComponent->IsInitialized() )
        {
            pComponent->Shutdown();
        }

        pComponent->Unload( loadingContext, m_ID );

        // Destroy the component
        //-------------------------------------------------------------------------

        DestroyComponentImmediate( pComponent );
    }

    //-------------------------------------------------------------------------

    bool Entity::UpdateEntityState( EntityModel::LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext )
    {
        Threading::ScopeLock lock( m_internalStateMutex );

        //-------------------------------------------------------------------------
        // Deferred Actions
        //-------------------------------------------------------------------------
        // Note: ORDER OF OPERATIONS MATTERS!

        for ( int32 i = 0; i < (int32) m_deferredActions.size(); i++ )
        {
            EntityInternalStateAction& action = m_deferredActions[i];

            switch ( action.m_type )
            {
                case EntityInternalStateAction::Type::CreateSystem:
                {
                    CreateSystemDeferred( loadingContext, (TypeSystem::TypeInfo const*) action.m_ptr );
                    m_deferredActions.erase( m_deferredActions.begin() + i );
                    i--;
                }
                break;

                case EntityInternalStateAction::Type::DestroySystem:
                {
                    DestroySystemDeferred( loadingContext, (TypeSystem::TypeInfo const*) action.m_ptr );
                    m_deferredActions.erase( m_deferredActions.begin() + i );
                    i--;
                }
                break;

                case EntityInternalStateAction::Type::AddComponent:
                {
                    SpatialEntityComponent* pParentComponent = nullptr;

                    // Try to resolve the ID
                    if ( action.m_ID.IsValid() )
                    {
                        int32 const componentIdx = VectorFindIndex( m_components, action.m_ID, [] ( EntityComponent* pComponent, UUID const& componentID ) { return pComponent->GetID() == componentID; } );
                        KRG_ASSERT( componentIdx != InvalidIndex );

                        pParentComponent = ComponentCast<SpatialEntityComponent>( m_components[componentIdx] );
                        KRG_ASSERT( pParentComponent != nullptr );
                    }

                    AddComponentDeferred( loadingContext, (EntityComponent*) action.m_ptr, pParentComponent );
                    m_deferredActions.erase( m_deferredActions.begin() + i );
                    i--;
                }
                break;

                case EntityInternalStateAction::Type::DestroyComponent:
                {
                    auto pComponentToDestroy = (EntityComponent*) action.m_ptr;

                    // Remove spatial components from the hierarchy immediately
                    SpatialEntityComponent* pSpatialComponent = ComponentCast<SpatialEntityComponent>( pComponentToDestroy );
                    if ( pSpatialComponent != nullptr )
                    {
                        RemoveComponentFromSpatialHierarchy( pSpatialComponent );
                    }

                    // Unregister from local systems
                    if ( pComponentToDestroy->m_isRegisteredWithEntity )
                    {
                        UnregisterComponentFromLocalSystems( pComponentToDestroy );
                    }

                    // Request unregister from global systems
                    if ( pComponentToDestroy->m_isRegisteredWithWorld )
                    {
                        activationContext.m_componentsToUnregister.enqueue( TPair<Entity*, EntityComponent*>( this, pComponentToDestroy ) );
                        action.m_type = EntityInternalStateAction::Type::WaitForComponentUnregistration;
                    }
                }
                // No Break: Intentional Fall-through here

                case EntityInternalStateAction::Type::WaitForComponentUnregistration:
                {
                    auto pComponentToDestroy = (EntityComponent*) action.m_ptr;

                    // We can destroy the component safely
                    if ( !pComponentToDestroy->m_isRegisteredWithEntity && !pComponentToDestroy->m_isRegisteredWithWorld )
                    {
                        DestroyComponentDeferred( loadingContext, (EntityComponent*) action.m_ptr );
                        m_deferredActions.erase( m_deferredActions.begin() + i );
                        i--;
                    }
                }
                break;
            }
        }

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
                    KRG_ASSERT( pComponent->IsInitialized() ); // Did you forget to call the parent class initialize?

                    // If we are already activated, then register with entity systems
                    if ( IsActivated() )
                    {
                        RegisterComponentWithLocalSystems( pComponent );
                        activationContext.m_componentsToRegister.enqueue( TPair<Entity*, EntityComponent*>( this, pComponent ) );
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        // Entity update registration
        //-------------------------------------------------------------------------

        bool const shouldBeRegisteredForUpdates = IsActivated() && !m_systems.empty() && !HasSpatialParent();

        if ( shouldBeRegisteredForUpdates )
        {
            if ( !m_isRegisteredForUpdates )
            {
                activationContext.m_registerForEntityUpdate.enqueue( this );
            }
        }
        else // Doesnt require an update
        {
            if ( m_isRegisteredForUpdates )
            {
                activationContext.m_unregisterForEntityUpdate.enqueue( this );
            }
        }

        //-------------------------------------------------------------------------

        return m_deferredActions.empty();
    }
}