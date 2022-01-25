#pragma once
#include "Tools/Entity/_Module/API.h"
#include "Tools/Core/UndoStack.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "System/Resource/ResourceID.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------
// Entity Editor Context
//-------------------------------------------------------------------------
// Maintains the shared state for all entity editor tools

namespace KRG
{
    namespace Resource { class ResourceDatabase; }
}

namespace KRG::EntityModel
{
    class EntityEditorUndoableAction;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API EntityEditorContext
    {
    public:

        EntityEditorContext( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDB, EntityWorld* pWorld, UndoStack& undoStack );

        void Update( UpdateContext const& context );

        // Map/World
        //-------------------------------------------------------------------------

        void SetMapToUse( ResourceID const& resourceID );
        void SetMapToUse( EntityMapID const& mapID );

        inline EntityMap* GetMap() { return m_pMap; }
        inline EntityMap const* GetMap() const  { return m_pMap; }
        inline EntityWorld* GetWorld() { return m_pWorld; }
        inline EntityWorld const* GetWorld() const  { return m_pWorld; }

        // Selection
        //-------------------------------------------------------------------------

        inline int32 GetNumSelectedEntities() { return (int32) m_selectedEntities.size(); }
        inline bool HasSingleSelectedEntity() const { return m_selectedEntities.size() == 1; }
        inline TVector<Entity*> GetSelectedEntities() { return m_selectedEntities; }
        inline Entity* GetSelectedEntity() const { KRG_ASSERT( HasSingleSelectedEntity() ); return m_selectedEntities[0]; }

        inline int32 GetNumSelectedComponents() { return (int32) m_selectedComponents.size(); }
        inline bool HasSingleSelectedComponent() const { return m_selectedComponents.size() == 1; }
        inline TVector<EntityComponent*> GetSelectedComponents() { return m_selectedComponents; }
        inline EntityComponent* GetSelectedComponent() const { KRG_ASSERT( HasSingleSelectedComponent() ); return m_selectedComponents[0]; }

        inline EntitySystem* GetSelectedSystem() { return m_pSelectedSystem; }

        void SelectEntity( Entity* pEntity );
        void SelectEntities( TVector<Entity*> const& entities );
        void AddToSelectedEntities( Entity* pEntity );
        void RemoveFromSelectedEntities( Entity* pEntity );
        void SelectComponent( EntityComponent* pComponent );
        void AddToSelectedComponents( EntityComponent* pComponent );
        void RemoveFromSelectedComponents( EntityComponent* pComponent );
        void SelectSystem( EntitySystem* pSystem );
        void ClearSelection();
        void ClearSelectedComponents() { m_selectedComponents.clear(); }
        void ClearSelectedSystem() { m_pSelectedSystem = nullptr; }

        bool IsSelected( Entity const* pEntity ) const;
        bool IsSelected( EntityComponent const* pComponent ) const;
        bool IsSelected( EntitySystem const* pSystem ) const { return m_pSelectedSystem == pSystem; }

        // Entity/Map Operations
        //-------------------------------------------------------------------------

        Entity* CreateEntity();
        void AddEntity( Entity* pEntity );
        void DuplicateSelectedEntities();
        void DestroyEntity( Entity* pEntity );
        void DestroySelectedEntities();

        void CreateSystem( Entity* pEntity, TypeSystem::TypeInfo const* pSystemTypeInfo );
        void DestroySystem( Entity* pEntity, EntitySystem* pSystem );
        void DestroySystem( Entity* pEntity, TypeSystem::TypeID systemTypeID );

        void CreateComponent( Entity* pEntity, TypeSystem::TypeInfo const* pComponentTypeInfo, ComponentID const& parentSpatialComponentID = ComponentID() );
        void DestroyComponent( Entity* pEntity, EntityComponent* pComponent );

        // Transform Manipulation
        //-------------------------------------------------------------------------
        // This always operates on the current selection!

        bool HasSpatialSelection() const { return m_hasSpatialSelection; }
        Transform const& GetSelectionTransform() const { KRG_ASSERT( HasSpatialSelection() ); return m_selectionTransform; }
        OBB const& GetSelectionBounds() const { return m_selectionBounds; }

        void BeginTransformManipulation( Transform const& newTransform, bool duplicateSelection = false );
        void ApplyTransformManipulation( Transform const& newTransform );
        void EndTransformManipulation( Transform const& newTransform );

        // Undo/Redo
        //-------------------------------------------------------------------------

        void OnUndoRedo( UndoStack::Operation operation, IUndoableAction const* pAction );

        void BeginEditEntities( TVector<Entity*> const& entities );
        void EndEditEntities();

        void BeginEditComponent( EntityComponent* pComponent );
        inline void EndEditComponent() { EndEditEntities(); }

        inline void BeginEditEntity( Entity* pEntity ) { BeginEditEntities( { pEntity } ); }
        inline void EndEditEntity() { EndEditEntities(); }

        // Debug Helpers / Visualization
        //-------------------------------------------------------------------------

        inline Drawing::DrawContext GetDrawingContext() { return m_pWorld->GetDebugDrawingSystem()->GetDrawingContext(); }

    private:

        void OnSelectionModified();
        void CalculateSelectionSpatialInfo();
        void CalculateSelectionSpatialBounds();

    public:

        TypeSystem::TypeRegistry const&         m_typeRegistry;
        Resource::ResourceDatabase const&       m_resourceDB;

    private:

        EntityWorld* const                      m_pWorld = nullptr;
        EntityMap*                              m_pMap = nullptr;
        EntityMapID                             m_mapID;

        UndoStack&                              m_undoStack;
        EntityEditorUndoableAction*             m_pActiveUndoableAction = nullptr;

        // Selection: multi-tiered
        TVector<Entity*>                        m_selectedEntities;
        TVector<EntityComponent*>               m_selectedComponents;
        EntitySystem*                           m_pSelectedSystem = nullptr;

        // Transform manipulation
        OBB                                     m_selectionBounds;
        Transform                               m_selectionTransform;
        TVector<Transform>                      m_selectionOffsetTransforms;
        bool                                    m_hasSpatialSelection = false;

        // Visualization
        TVector<TypeSystem::TypeInfo const*>    m_volumeTypes;
        TVector<TypeSystem::TypeInfo const*>    m_visualizedVolumeTypes;

        // All the entities requested for deletion in a frame, treated as a single operation
        TVector<Entity*>                        m_entityDeletionRequests;
    };
}