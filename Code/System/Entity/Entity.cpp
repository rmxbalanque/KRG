#include "Entity.h"
#include "EntitySystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* Entity::TypeInfoPtr = nullptr;

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

        Entity::BreakAllEntitySpatialAttachments( this );

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

    void Entity::Activate( EntityModel::LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status == Status::Deactivated );
        m_status = Status::Activated;

        // Initialize spatial hierarchy
        //-------------------------------------------------------------------------
        // Transforms are set at serialization time so we have all information available to update the world transforms

        if ( m_pRootSpatialComponent != nullptr )
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

        if ( m_pParentSpatialEntity != nullptr )
        {
            AttachToParent();
        }

        //-------------------------------------------------------------------------

        RefreshEntityAttachments();

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

        m_status = Status::Deactivated;
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

    //-------------------------------------------------------------------------

    void Entity::LoadComponents( EntityModel::LoadingContext const& loadingContext )
    {
        for ( auto pComponent : m_components )
        {
            KRG_ASSERT( pComponent->IsUnloaded() );
            pComponent->Load( loadingContext, m_ID );
        }
    }

    void Entity::UnloadComponents( EntityModel::LoadingContext const& loadingContext )
    {
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
    }

    bool Entity::UpdateComponentLoading( EntityModel::LoadingContext const& loadingContext )
    {
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