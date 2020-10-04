#include "Entity.h"
#include "EntitySystem.h"
#include "System/Core/Update/UpdateContext.h"

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

        if ( pChildEntity->IsInitialized() )
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
        KRG_ASSERT( !IsLoaded() );
        KRG_ASSERT( m_pParentSpatialEntity == nullptr && m_attachedEntities.empty() && !m_parentAttachmentSocketID.IsValid() );
        KRG_ASSERT( !m_isAttachedToParent );
    }

    void Entity::Load( EntityLoadingContext const& context )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_status == Status::Unloaded );

        for ( auto pComponent : m_components )
        {
            pComponent->Load( context, m_ID );
        }

        m_status = Status::Loading;
    }

    void Entity::Unload( EntityLoadingContext const& context )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_status != Status::Unloaded );

        for( auto pComponent : m_components )
        {
            pComponent->Unload( context, m_ID );
        }

        m_status = Status::Unloaded;
    }

    void Entity::UpdateLoading()
    {
        KRG_ASSERT( m_status == Status::Loading );

        bool haveAllComponentsFinishedLoading = true;

        for ( auto pComponent : m_components )
        {
            pComponent->UpdateLoading();

            KRG_ASSERT( pComponent->IsLoading() || pComponent->IsLoaded() || pComponent->HasLoadingFailed() );

            if ( pComponent->IsLoading() )
            {
                haveAllComponentsFinishedLoading = false;
                break;
            }
        }

        //-------------------------------------------------------------------------

        if( haveAllComponentsFinishedLoading )
        {
            m_status = Status::Loaded;
        }
    }

    //-------------------------------------------------------------------------

    void Entity::Initialize( SystemRegistry const& systemRegistry )
    {
        KRG_ASSERT( m_status == Status::Loaded );

        // Initialize spatial hierarchy
        //-------------------------------------------------------------------------
        // Transforms are set at serialization time so we have all information available to update the world transforms

        if ( m_pRootSpatialComponent != nullptr )
        {
            // Calculate the initial world transform but do not trigger the callback to the components
            m_pRootSpatialComponent->CalculateWorldTransform( false );
        }

        // Initialize all components
        //-------------------------------------------------------------------------
        // An entity can only be initialized once all loading for its components has completed (i.e. fully loaded or failed loading)

        for ( auto pComponent : m_components )
        {
            if ( pComponent->HasLoadingFailed() )
            {
                continue;
            }

            KRG_ASSERT( IsLoaded() );

            pComponent->Initialize();
            pComponent->m_status = EntityComponent::Status::Initialized;

            // Register with systems
            for ( auto pSystem : m_systems )
            {
                pSystem->RegisterComponent( pComponent );
            }
        }

        // Generate system update list
        //-------------------------------------------------------------------------

        for( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            for( auto& pSystem : m_systems )
            {
                if( pSystem->GetRequiredUpdatePriorities().IsUpdateStageEnabled( (UpdateStage) i ) )
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

        // Spatial Attachments
        //-------------------------------------------------------------------------

        if ( m_pParentSpatialEntity != nullptr )
        {
            AttachToParent();
        }

        //-------------------------------------------------------------------------

        RefreshEntityAttachments();

        m_status = Status::Initialized;
    }

    void Entity::Shutdown()
    {
        KRG_ASSERT( m_status == Status::Initialized );

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

        // Shutdown all components
        //-------------------------------------------------------------------------
        // An entity can only be shutdown once all loading for its components has completed (i.e. fully loaded or failed loading)

        for ( auto pComponent : m_components )
        {
            if ( pComponent->HasLoadingFailed() )
            {
                continue;
            }

            KRG_ASSERT( pComponent->IsInitialized() );

            for ( auto pSystem : m_systems )
            {
                pSystem->UnregisterComponent( pComponent );
            }

            pComponent->Shutdown();
            pComponent->m_status = EntityComponent::Status::Loaded;
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
        SpatialEntityComponent* pComponentToAttachTo = pParentEntity->m_pRootSpatialComponent.GetRawPtr();
        if ( m_parentAttachmentSocketID.IsValid() )
        {
            if ( auto pFoundComponent = pParentEntity->FindSocketAttachmentComponent( m_parentAttachmentSocketID ) )
            {
                pComponentToAttachTo = pFoundComponent;
            }
            else
            {
                KRG_LOG_WARNING( "Entity", "Could not find attachment socket '%s' on entity '%s' (%s)", m_parentAttachmentSocketID.c_str(), pParentEntity->GetName().c_str(), pParentEntity->GetID().ToString().c_str() );
            }
        }

        // Perform attachment
        //-------------------------------------------------------------------------

        KRG_ASSERT( pComponentToAttachTo != nullptr );

        // Set component hierarchy values
        m_pRootSpatialComponent->m_pSpatialParent = pComponentToAttachTo;
        m_pRootSpatialComponent->m_parentAttachmentSocketID = m_parentAttachmentSocketID;
        m_pRootSpatialComponent->CalculateWorldTransform();

        // Add to the list of child components on the component to attach to
        pComponentToAttachTo->m_spatialChildren.emplace_back( m_pRootSpatialComponent.GetRawPtr() );

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
        auto foundIter = VectorFind( pParentComponent->m_spatialChildren, m_pRootSpatialComponent.GetRawPtr() );
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
}