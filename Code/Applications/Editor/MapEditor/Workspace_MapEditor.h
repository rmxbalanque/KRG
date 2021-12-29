#pragma once

#include "Tools/Core/Workspaces/EditorWorkspace.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Engine/Core/Entity/EntityMapDescriptor.h"
#include "Engine/Core/ToolsUI/Gizmo.h"

//-------------------------------------------------------------------------

namespace KRG 
{
    class EntityComponent;
    class SpatialEntityComponent; 
    class TreeListView;
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMap;
    class EntityInspectorTreeView;
    class TypeSelector;

    //-------------------------------------------------------------------------

    class EntityMapEditor final : public EditorWorkspace
    {
        constexpr static const char* const s_addSystemDialogTitle = "Add System";
        constexpr static const char* const s_addComponentDialogTitle = "Add Component";

    public:

        EntityMapEditor( WorkspaceInitializationContext const& context, EntityWorld* pWorld );
        ~EntityMapEditor();

        virtual void Initialize( UpdateContext const& context ) override;

        inline bool HasLoadedMap() const { return m_loadedMap.IsValid(); }
        inline ResourceID GetLoadedMap() const { return m_loadedMap; }

        void CreateNewMap();
        void SelectAndLoadMap();
        void LoadMap( TResourcePtr<EntityModel::EntityMapDescriptor> const& mapToLoad );
        void SaveMap();
        void SaveMapAs();

        void OnGamePreviewStarted();
        void OnGamePreviewEnded();

    private:

        virtual uint32 GetID() const override { return 0xFFFFFFFF; }

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void DrawWorkspaceToolbar( UpdateContext const& context ) override;
        virtual void DrawUI( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual void DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual bool IsDirty() const override{ return false; } // TODO
        virtual bool AlwaysAllowSaving() const override { return true; }
        virtual bool Save() override;
        virtual void OnMousePick( uint64 pickingID );

        void HandleInput( UpdateContext const& context );

        void SelectEntity( Entity* pEntity );
        void SelectComponent( EntityComponent* pComponent );
        void ClearSelection();

        // Map outliner
        void DrawMapOutliner( UpdateContext const& context );

        // Property Grid
        void DrawPropertyGrid( UpdateContext const& context );
        void PreEdit( PropertyEditInfo const& eventInfo );
        void PostEdit( PropertyEditInfo const& eventInfo );

        // Entity Inspector
        void DrawEntityInspector( UpdateContext const& context );
        void DrawAddSystemDialog();
        void DrawAddComponentDialog();
        void OnEntityStateChanged( Entity* pEntityChanged );
        void OnInspectorSelectionChanged();

    private:

        String                          m_outlinerWindowName;
        String                          m_entityViewWindowName;
        String                          m_propertyGridWindowName;

        ResourceID                      m_loadedMap;

        EntityInspectorTreeView*        m_pEntityInspectorTreeView = nullptr;
        TypeSelector*                   m_pTypeSelector = nullptr;
        PropertyGrid                    m_propertyGrid;

        Entity*                         m_pSelectedEntity = nullptr;
        EntityComponent*                m_pSelectedComponent = nullptr;
        Transform                       m_editedTransform;
        ImGuiX::Gizmo                   m_gizmo;
        bool                            m_syncOutlinerToSelectedItem = false;

        EventBindingID                  m_preEditBindingID;
        EventBindingID                  m_postEditBindingID;
        EventBindingID                  m_entityStateChangedBindingID;
        EventBindingID                  m_inspectorSelectionChangedBindingID;

        bool                            m_isGamePreviewRunning = false;
    };
}