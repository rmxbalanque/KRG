#include "EntityEditor_Context.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Engine/Core/Components/Component_Volumes.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    // Compound undoable action - handles all scene manipulation operations
    // All operations are at the granularity of an entity, we dont manage component actions individually
    class EntityEditorUndoableAction final : public IUndoableAction
    {
        struct SpatialState
        {
            StringID    m_entityName;
            StringID    m_componentName;
            Transform   m_transform;
        };

    public:

        enum Type
        {
            Invalid = 0,
            CreateEntities,
            DeleteEntities,
            ModifyEntities,
            MoveComponents,
        };

    public:

        EntityEditorUndoableAction( TypeSystem::TypeRegistry const& typeRegistry, EntityMap* pMap )
            : m_typeRegistry( typeRegistry )
            , m_pMap( pMap )
        {
            KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        }

        void RecordCreate( TVector<Entity*> createdEntities )
        {
            m_actionType = Type::CreateEntities;

            for ( auto pEntity : createdEntities )
            {
                KRG_ASSERT( pEntity != nullptr );
                bool const result = pEntity->CreateDescriptor( m_typeRegistry, m_createdEntities.emplace_back() );
                KRG_ASSERT( result );
            }
        }

        void RecordDelete( TVector<Entity*> entitiesToDelete )
        {
            m_actionType = Type::DeleteEntities;

            for ( auto pEntity : entitiesToDelete )
            {
                KRG_ASSERT( pEntity != nullptr && pEntity->GetMapID() == m_pMap->GetID() );
                bool const result = pEntity->CreateDescriptor( m_typeRegistry, m_deletedEntities.emplace_back() );
                KRG_ASSERT( result );
            }
        }

        void RecordBeginEdit( TVector<Entity*> entitiesToBeModified, bool wereEntitiesDuplicated = false )
        {
            KRG_ASSERT( entitiesToBeModified.size() > 0 );

            m_actionType = Type::ModifyEntities;
            m_editedEntities = entitiesToBeModified;
            m_entitiesWereDuplicated = wereEntitiesDuplicated;

            for ( auto pEntity : m_editedEntities )
            {
                KRG_ASSERT( pEntity != nullptr );
                KRG_ASSERT( wereEntitiesDuplicated ? true : pEntity->GetMapID() == m_pMap->GetID() );
                bool const result = pEntity->CreateDescriptor( m_typeRegistry, m_entityDescPreModification.emplace_back() );
                KRG_ASSERT( result );
            }
        }

        void RecordEndEdit()
        {
            KRG_ASSERT( m_actionType == ModifyEntities );
            KRG_ASSERT( m_editedEntities.size() > 0 );

            for ( auto pEntity : m_editedEntities )
            {
                KRG_ASSERT( pEntity != nullptr && pEntity->GetMapID() == m_pMap->GetID() );
                bool const result = pEntity->CreateDescriptor( m_typeRegistry, m_entityDescPostModification.emplace_back() );
                KRG_ASSERT( result );
            }

            m_editedEntities.clear();
        }

    private:

        virtual void Undo() override
        {
            switch ( m_actionType )
            {
                case EntityEditorUndoableAction::CreateEntities:
                {
                    for ( auto const& entityDesc : m_createdEntities )
                    {
                        auto pEntity = m_pMap->FindEntityByName( entityDesc.m_name );
                        m_pMap->DestroyEntity( pEntity );
                    }
                }
                break;

                case EntityEditorUndoableAction::DeleteEntities:
                {
                    for ( auto const& entityDesc : m_deletedEntities )
                    {
                        auto pEntity = Entity::CreateFromDescriptor( m_typeRegistry, entityDesc );
                        m_pMap->AddEntity( pEntity );
                    }
                }
                break;

                case EntityEditorUndoableAction::ModifyEntities:
                {
                    int32 const numEntities = (int32) m_entityDescPostModification.size();
                    KRG_ASSERT( m_entityDescPostModification.size() == m_entityDescPreModification.size() );

                    for ( int32 i = 0; i < numEntities; i++ )
                    {
                        // Remove current entities
                        auto pExistingEntity = m_pMap->FindEntityByName( m_entityDescPostModification[i].m_name );
                        KRG_ASSERT( pExistingEntity != nullptr );
                        m_pMap->DestroyEntity( pExistingEntity );

                        //-------------------------------------------------------------------------

                        // Only recreate the entities if they werent duplicated, if they were duplicate, then deleting them was enough to undo the action
                        if ( !m_entitiesWereDuplicated )
                        {
                            auto pNewEntity = Entity::CreateFromDescriptor( m_typeRegistry, m_entityDescPreModification[i] );
                            m_pMap->AddEntity( pNewEntity );
                        }
                    }
                }
                break;

                case EntityEditorUndoableAction::MoveComponents:
                {

                }
                break;

                default:
                KRG_UNREACHABLE_CODE();
                break;
            }
        }

        virtual void Redo() override
        {
            switch ( m_actionType )
            {
                case EntityEditorUndoableAction::CreateEntities:
                {
                    for ( auto const& entityDesc : m_createdEntities )
                    {
                        auto pEntity = Entity::CreateFromDescriptor( m_typeRegistry, entityDesc );
                        m_pMap->AddEntity( pEntity );
                    }
                }
                break;

                case EntityEditorUndoableAction::DeleteEntities:
                {
                    for ( auto const& entityDesc : m_deletedEntities )
                    {
                        auto pEntity = m_pMap->FindEntityByName( entityDesc.m_name );
                        m_pMap->DestroyEntity( pEntity );
                    }
                }
                break;

                case EntityEditorUndoableAction::ModifyEntities:
                {
                    int32 const numEntities = (int32) m_entityDescPreModification.size();
                    KRG_ASSERT( m_entityDescPostModification.size() == m_entityDescPreModification.size() );

                    for ( int32 i = 0; i < numEntities; i++ )
                    {
                        // Destroy the current entities (only if there are not duplicates since if they are they wont exist)
                        if ( !m_entitiesWereDuplicated )
                        {
                            auto pExistingEntity = m_pMap->FindEntityByName( m_entityDescPreModification[i].m_name );
                            KRG_ASSERT( pExistingEntity != nullptr );
                            m_pMap->DestroyEntity( pExistingEntity );
                        }

                        //-------------------------------------------------------------------------

                        auto pNewEntity = Entity::CreateFromDescriptor( m_typeRegistry, m_entityDescPostModification[i] );
                        m_pMap->AddEntity( pNewEntity );
                    }
                }
                break;

                case EntityEditorUndoableAction::MoveComponents:
                {

                }
                break;

                default:
                KRG_UNREACHABLE_CODE();
                break;
            }
        }

    private:

        TypeSystem::TypeRegistry const&     m_typeRegistry;
        EntityMap*                          m_pMap = nullptr;
        Type                                m_actionType = Invalid;

        // Data: Create
        TVector<EntityDescriptor>           m_createdEntities;

        // Data: Delete
        TVector<EntityDescriptor>           m_deletedEntities;

        // Data: Modify
        TVector<Entity*>                    m_editedEntities; // Temporary storage that is only valid between a Begin and End call
        TVector<EntityDescriptor>           m_entityDescPreModification;
        TVector<EntityDescriptor>           m_entityDescPostModification;
        bool                                m_entitiesWereDuplicated = false;

        // Data: Move
        TVector<SpatialState>               m_componentSpatialStatePreMove;
        TVector<SpatialState>               m_componentSpatialStatePostMove;
    };
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityEditorContext::EntityEditorContext( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDB, EntityWorld* pWorld, UndoStack& undoStack )
        : m_typeRegistry( typeRegistry )
        , m_resourceDB( resourceDB )
        , m_undoStack( undoStack )
        , m_pWorld( pWorld )
        , m_volumeTypes( m_typeRegistry.GetAllDerivedTypes( VolumeComponent::GetStaticTypeID(), false, false, true ) )
    {}

    void EntityEditorContext::SetMapToUse( ResourceID const& resourceID )
    {
        auto pMap = m_pWorld->GetMap( resourceID );
        KRG_ASSERT( pMap != nullptr );
        KRG_ASSERT( m_mapID != pMap->GetID() );

        ClearSelection();
        m_entityDeletionRequests.clear();
        m_mapID = pMap->GetID();
        m_pMap = nullptr;
    }

    void EntityEditorContext::SetMapToUse( EntityMapID const& mapID )
    {
        KRG_ASSERT( m_mapID != mapID );

        ClearSelection();
        m_entityDeletionRequests.clear();
        m_mapID = mapID;
        m_pMap = nullptr;
    }

    void EntityEditorContext::Update( UpdateContext const& context )
    {
        // Process all deletion requests
        //-------------------------------------------------------------------------

        if ( !m_entityDeletionRequests.empty() )
        {
            KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
            KRG_ASSERT( m_pActiveUndoableAction == nullptr );

            for ( auto pEntity : m_entityDeletionRequests )
            {
                m_pMap->DestroyEntity( pEntity );
            }

            m_entityDeletionRequests.clear();
            ClearSelection();
        }

        // Update map ptr
        //-------------------------------------------------------------------------

        if ( m_pMap == nullptr )
        {
            if ( m_mapID.IsValid() && m_pWorld->IsMapActive( m_mapID ) )
            {
                m_pMap = m_pWorld->GetMap( m_mapID );
                KRG_ASSERT( m_pMap != nullptr );
            }
            else
            {
                m_pMap = nullptr;
            }
        }
    }

    //-------------------------------------------------------------------------

    void EntityEditorContext::OnUndoRedo( UndoStack::Operation operation, IUndoableAction const* pAction )
    {
        auto pUndoableAction = static_cast<EntityEditorUndoableAction const*>( pAction );
        // TODO: record addition info in undo command to be able to restore selection
        ClearSelection();
    }

    void EntityEditorContext::BeginEditEntities( TVector<Entity*> const& entities )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );

        // Record undo action
        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordBeginEdit( entities );
    }

    void EntityEditorContext::EndEditEntities()
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction != nullptr );

        m_pActiveUndoableAction->RecordEndEdit();
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;

        CalculateSelectionSpatialInfo();
    }

    void EntityEditorContext::BeginEditComponent( EntityComponent* pComponent )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        KRG_ASSERT( pComponent != nullptr );

        auto pEntity = m_pMap->FindEntity( pComponent->GetEntityID() );
        KRG_ASSERT( pEntity != nullptr );

        BeginEditEntities( { pEntity } );

        //-------------------------------------------------------------------------

        m_pWorld->PrepareComponentForEditing( m_pMap->GetID(), pComponent->GetEntityID(), pComponent->GetID() );
    }

    //-------------------------------------------------------------------------

    bool EntityEditorContext::IsSelected( Entity const* pEntity ) const
    {
        for ( auto pSelectedEntity : m_selectedEntities )
        {
            if ( pSelectedEntity == pEntity )
            {
                return true;
            }
        }

        return false;
    }

    bool EntityEditorContext::IsSelected( EntityComponent const* pComponent ) const
    {
        for ( auto pSelectedComponent : m_selectedComponents )
        {
            if ( pSelectedComponent == pComponent )
            {
                return true;
            }
        }

        return false;
    }

    void EntityEditorContext::SelectEntity( Entity* pEntity )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );

        ClearSelection();
        m_selectedEntities.emplace_back( pEntity );
        OnSelectionModified();
    }

    void EntityEditorContext::SelectEntities( TVector<Entity*> const& entities )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );

        for ( auto pEntity : entities )
        {
            KRG_ASSERT( pEntity != nullptr );
        }

        m_selectedEntities = entities;
        m_selectedComponents.clear();
        m_pSelectedSystem = nullptr;

        OnSelectionModified();
    }

    void EntityEditorContext::AddToSelectedEntities( Entity* pEntity )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( pEntity->GetMapID() == m_pMap->GetID() );
        
        if ( !VectorContains( m_selectedEntities, pEntity ) )
        {
            m_pSelectedSystem = nullptr;
            m_selectedComponents.clear();

            m_selectedEntities.emplace_back( pEntity );
            OnSelectionModified();
        }
    }

    void EntityEditorContext::RemoveFromSelectedEntities( Entity* pEntity )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( pEntity->GetMapID() == m_pMap->GetID() );
        KRG_ASSERT( m_selectedComponents.empty() && m_pSelectedSystem == nullptr );

        if ( VectorContains( m_selectedEntities, pEntity ) )
        {
            m_selectedEntities.erase_first( pEntity );
            OnSelectionModified();
        }
    }

    void EntityEditorContext::SelectComponent( EntityComponent* pComponent )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        KRG_ASSERT( pComponent != nullptr );
        KRG_ASSERT( m_selectedEntities.size() == 1 );
        KRG_ASSERT( pComponent->GetEntityID() == m_selectedEntities[0]->GetID() );

        m_pSelectedSystem = nullptr;

        m_selectedComponents.clear();
        m_selectedComponents.emplace_back( pComponent );
        OnSelectionModified();
    }

    void EntityEditorContext::AddToSelectedComponents( EntityComponent* pComponent )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        KRG_ASSERT( pComponent != nullptr );
        KRG_ASSERT( m_selectedEntities.size() == 1 && m_pSelectedSystem == nullptr );
        KRG_ASSERT( pComponent->GetEntityID() == m_selectedEntities[0]->GetID() );
        KRG_ASSERT( !VectorContains( m_selectedComponents, pComponent ) );

        m_selectedComponents.emplace_back( pComponent );
        OnSelectionModified();
    }

    void EntityEditorContext::RemoveFromSelectedComponents( EntityComponent* pComponent )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        KRG_ASSERT( pComponent != nullptr );
        KRG_ASSERT( m_selectedEntities.size() == 1 && m_pSelectedSystem == nullptr );
        KRG_ASSERT( pComponent->GetEntityID() == m_selectedEntities[0]->GetID() );
        KRG_ASSERT( VectorContains( m_selectedComponents, pComponent ) );

        m_selectedComponents.erase_first( pComponent );
        OnSelectionModified();
    }

    void EntityEditorContext::SelectSystem( EntitySystem* pSystem )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        KRG_ASSERT( pSystem != nullptr );
        KRG_ASSERT( m_selectedEntities.size() == 1 );
        KRG_ASSERT( VectorContains( m_selectedEntities[0]->GetSystems(), pSystem ) );

        m_selectedComponents.clear();
        m_pSelectedSystem = pSystem;
        OnSelectionModified();
    }

    void EntityEditorContext::ClearSelection()
    {
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );

        m_selectedEntities.clear();
        m_selectedComponents.clear();
        m_pSelectedSystem = nullptr;
        OnSelectionModified();
    }

    void EntityEditorContext::OnSelectionModified()
    {
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        CalculateSelectionSpatialInfo();
    }

    void EntityEditorContext::CalculateSelectionSpatialInfo()
    {
        TVector<OBB> bounds;

        m_selectionTransform = Transform::Identity;
        m_selectionOffsetTransforms.clear();
        m_hasSpatialSelection = false;

        // Check if we have spatial selection
        //-------------------------------------------------------------------------
        // Record the the transforms of all found spatial elements in the offset array

        if ( m_pSelectedSystem == nullptr )
        {
            // Check all selected components
            if ( m_selectedComponents.size() > 0 )
            {
                for ( auto pComponent : m_selectedComponents )
                {
                    if ( auto pSC = TryCast<SpatialEntityComponent>( pComponent ) )
                    {
                        m_selectionOffsetTransforms.emplace_back( pSC->GetWorldTransform() );
                        bounds.emplace_back( pSC->GetWorldBounds() );
                        m_hasSpatialSelection = true;
                    }
                }
            }
            else // Check all selected entities
            {
                for ( auto pSelectedEntity : m_selectedEntities )
                {
                    if ( pSelectedEntity->IsSpatialEntity() )
                    {
                        m_selectionOffsetTransforms.emplace_back( pSelectedEntity->GetWorldTransform() );
                        bounds.emplace_back( pSelectedEntity->GetCombinedWorldBounds() ); // TODO: calculate combined bounds
                        m_hasSpatialSelection = true;
                    }
                }
            }
        }

        // Update selection transform
        //-------------------------------------------------------------------------

        if ( m_hasSpatialSelection )
        {
            if ( m_selectionOffsetTransforms.size() == 1 )
            {
                m_selectionTransform = m_selectionOffsetTransforms[0];
                m_selectionOffsetTransforms[0] = Transform::Identity;
            }
            else
            {
                // Calculate the average position of all transforms found
                Vector averagePosition = Vector::Zero;
                for ( auto const& t : m_selectionOffsetTransforms )
                {
                    averagePosition += t.GetTranslation();
                }
                averagePosition /= (float) m_selectionOffsetTransforms.size();

                // Set the average transform
                m_selectionTransform = Transform( Quaternion::Identity, averagePosition );

                // Calculate the offsets
                Transform const inverseSelectionTransform = m_selectionTransform.GetInverse();
                for ( auto& t : m_selectionOffsetTransforms )
                {
                    t = t * inverseSelectionTransform;
                }
            }
        }

        // Update selection bounds
        //-------------------------------------------------------------------------

        if ( m_hasSpatialSelection )
        {
            if ( bounds.size() == 1 )
            {
                m_selectionBounds = bounds[0];
            }
            else
            {
                TVector<Vector> points;

                for ( auto itemBounds : bounds )
                {
                    Vector corners[8];
                    itemBounds.GetCorners( corners );
                    for ( auto i = 0; i < 8; i++ )
                    {
                        points.emplace_back( corners[i] );
                    }
                }

                m_selectionBounds = OBB( points.data(), (uint32) points.size() );
            }
        }

        CalculateSelectionSpatialBounds();
    }

    void EntityEditorContext::CalculateSelectionSpatialBounds()
    {
        TVector<OBB> bounds;

        // Check if we have spatial selection
        //-------------------------------------------------------------------------
        // Record the the transforms of all found spatial elements in the offset array

        if ( m_pSelectedSystem == nullptr )
        {
            // Check all selected components
            if ( m_selectedComponents.size() > 0 )
            {
                for ( auto pComponent : m_selectedComponents )
                {
                    if ( auto pSC = TryCast<SpatialEntityComponent>( pComponent ) )
                    {
                        bounds.emplace_back( pSC->GetWorldBounds() );
                        m_hasSpatialSelection = true;
                    }
                }
            }
            else // Check all selected entities
            {
                for ( auto pSelectedEntity : m_selectedEntities )
                {
                    if ( pSelectedEntity->IsSpatialEntity() )
                    {
                        bounds.emplace_back( pSelectedEntity->GetCombinedWorldBounds() ); // TODO: calculate combined bounds
                        m_hasSpatialSelection = true;
                    }
                }
            }
        }

        // Update selection bounds
        //-------------------------------------------------------------------------

        if ( m_hasSpatialSelection )
        {
            if ( bounds.size() == 1 )
            {
                m_selectionBounds = bounds[0];
            }
            else
            {
                TVector<Vector> points;

                for ( auto itemBounds : bounds )
                {
                    Vector corners[8];
                    itemBounds.GetCorners( corners );
                    for ( auto i = 0; i < 8; i++ )
                    {
                        points.emplace_back( corners[i] );
                    }
                }

                m_selectionBounds = OBB( points.data(), (uint32) points.size() );
            }
        }
    }

    //-------------------------------------------------------------------------

    void EntityEditorContext::BeginTransformManipulation( Transform const& newTransform, bool duplicateSelection )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( !m_selectedEntities.empty() );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );

        // Should we duplicate the selection?
        if ( duplicateSelection )
        {
            DuplicateSelectedEntities();
        }

        // Create undo action
        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordBeginEdit( m_selectedEntities, duplicateSelection );

        // Apply transform modification
        ApplyTransformManipulation( newTransform );
    }

    void EntityEditorContext::ApplyTransformManipulation( Transform const& newTransform )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( !m_selectedEntities.empty() );
        KRG_ASSERT( m_pActiveUndoableAction != nullptr );

        // Update all selected components
        if ( m_selectedComponents.size() > 0 )
        {
            int32 offsetIdx = 0;
            for ( auto pComponent : m_selectedComponents )
            {
                if ( auto pSC = TryCast<SpatialEntityComponent>( pComponent ) )
                {
                    pSC->SetWorldTransform( m_selectionOffsetTransforms[offsetIdx] * newTransform );
                    offsetIdx++;
                }
            }
        }
        else // Update all selected entities
        {
            int32 offsetIdx = 0;
            for ( auto pSelectedEntity : m_selectedEntities )
            {
                if ( pSelectedEntity->IsSpatialEntity() )
                {
                    pSelectedEntity->SetWorldTransform( m_selectionOffsetTransforms[offsetIdx] * newTransform );
                    offsetIdx++;
                }
            }
        }

        m_selectionTransform = newTransform;
        CalculateSelectionSpatialBounds();
    }

    void EntityEditorContext::EndTransformManipulation( Transform const& newTransform )
    {
        ApplyTransformManipulation( newTransform );

        m_pActiveUndoableAction->RecordEndEdit();
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    //-------------------------------------------------------------------------

    Entity* EntityEditorContext::CreateEntity()
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        Entity* pEntity = KRG::New<Entity>( StringID( "Entity" ) );
        AddEntity( pEntity );
        return pEntity;
    }

    void EntityEditorContext::AddEntity( Entity* pEntity )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( pEntity != nullptr && !pEntity->IsAddedToMap() );

        // Add entity to map
        m_pMap->AddEntity( pEntity );
        SelectEntity( pEntity );

        // Record undo action
        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordCreate( { pEntity } );
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    void EntityEditorContext::DuplicateSelectedEntities()
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );

        if ( m_selectedEntities.empty() )
        {
            return;
        }

        TVector<Entity*> duplicatedEntities;
        for ( auto pEntity : m_selectedEntities )
        {
            EntityDescriptor entityDesc;
            if ( pEntity->CreateDescriptor( m_typeRegistry, entityDesc ) )
            {
                auto pDuplicatedEntity = Entity::CreateFromDescriptor( m_typeRegistry, entityDesc );
                duplicatedEntities.emplace_back( pDuplicatedEntity );
                m_pMap->AddEntity( pDuplicatedEntity );
            }
        }

        //-------------------------------------------------------------------------

        // Update selection
        SelectEntities( duplicatedEntities );
    }

    void EntityEditorContext::DestroyEntity( Entity* pEntity )
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );
        KRG_ASSERT( pEntity != nullptr && pEntity->GetMapID() == m_pMap->GetID() );
        m_entityDeletionRequests.emplace_back( pEntity );
        ClearSelection();

        // Record undo action
        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordDelete( { pEntity } );
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    void EntityEditorContext::DestroySelectedEntities()
    {
        KRG_ASSERT( m_pMap != nullptr && m_pMap->IsActivated() );

        if ( m_selectedEntities.empty() )
        {
            return;
        }

        for ( auto pEntity : m_selectedEntities )
        {
            m_entityDeletionRequests.emplace_back( pEntity );
        }
        ClearSelection();

        // Record undo action
        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordDelete( m_entityDeletionRequests );
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    void EntityEditorContext::CreateSystem( Entity* pEntity, TypeSystem::TypeInfo const* pSystemTypeInfo )
    {
        KRG_ASSERT( pSystemTypeInfo != nullptr );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( m_pMap->ContainsEntity( pEntity->GetID() ) );

        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordBeginEdit( { pEntity } );
        pEntity->CreateSystem( pSystemTypeInfo );
        m_pActiveUndoableAction->RecordEndEdit();
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    void EntityEditorContext::DestroySystem( Entity* pEntity, TypeSystem::TypeID systemTypeID )
    {
        KRG_ASSERT( systemTypeID.IsValid() );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( m_pMap->ContainsEntity( pEntity->GetID() ) );

        ClearSelectedSystem(); // Todo only clear if the system is selected

        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordBeginEdit( { pEntity } );
        pEntity->DestroySystem( systemTypeID );
        m_pActiveUndoableAction->RecordEndEdit();
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    void EntityEditorContext::DestroySystem( Entity* pEntity, EntitySystem* pSystem )
    {
        KRG_ASSERT( pSystem != nullptr );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( VectorContains( pEntity->GetSystems(), pSystem ) );
        KRG_ASSERT( m_pMap->ContainsEntity( pEntity->GetID() ) );

        if ( m_pSelectedSystem == pSystem )
        {
            m_pSelectedSystem = nullptr;
        }

        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordBeginEdit( { pEntity } );
        pEntity->DestroySystem( pSystem->GetTypeID() );
        m_pActiveUndoableAction->RecordEndEdit();
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    void EntityEditorContext::CreateComponent( Entity* pEntity, TypeSystem::TypeInfo const* pComponentTypeInfo, ComponentID const& parentSpatialComponentID )
    {
        KRG_ASSERT( pComponentTypeInfo != nullptr );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( m_pMap->ContainsEntity( pEntity->GetID() ) );

        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordBeginEdit( { pEntity } );
        pEntity->CreateComponent( pComponentTypeInfo, parentSpatialComponentID );
        m_pActiveUndoableAction->RecordEndEdit();
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    void EntityEditorContext::DestroyComponent( Entity* pEntity, EntityComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
        KRG_ASSERT( pEntity != nullptr );
        KRG_ASSERT( m_pMap->ContainsEntity( pEntity->GetID() ) );

        m_selectedComponents.erase_first_unsorted( pComponent );

        m_pActiveUndoableAction = KRG::New<EntityEditorUndoableAction>( m_typeRegistry, m_pMap );
        m_pActiveUndoableAction->RecordBeginEdit( { pEntity } );
        pEntity->DestroyComponent( pComponent->GetID() );
        m_pActiveUndoableAction->RecordEndEdit();
        m_undoStack.RegisterAction( m_pActiveUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }
}