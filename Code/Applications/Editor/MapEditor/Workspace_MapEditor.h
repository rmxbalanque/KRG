#pragma once

#include "Tools/Core/Workspaces/EditorWorkspace.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Engine/Core/Entity/EntityMapDescriptor.h"
#include "Engine/Core/DevUI/Gizmo.h"

//-------------------------------------------------------------------------

namespace KRG 
{
    class EntityComponent;
    class SpatialEntityComponent; 
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMap;

    //-------------------------------------------------------------------------

    class EntityMapEditor final : public EditorWorkspace
    {
    public:

        EntityMapEditor( EditorContext const& context, EntityWorld* pWorld );
        ~EntityMapEditor();

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

        virtual bool ShouldDrawFileMenu() const { return false; }
        virtual bool HasViewportToolbar() const override { return true; }
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void DrawWorkspaceToolbar( UpdateContext const& context ) override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual void DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual bool IsDirty() const override{ return false; } // TODO
        virtual bool Save() override;

        void SelectEntity( Entity* pEntity );
        void SelectComponent( EntityComponent* pComponent );
        void ClearSelection();

        void DrawEntityOutliner( UpdateContext const& context );
        void DrawEntityView( UpdateContext const& context );
        void DrawPropertyGrid( UpdateContext const& context );

        void DrawComponentEntry( EntityComponent* pComponent );
        void DrawSpatialComponentTree( SpatialEntityComponent* pComponent );

        void PreEdit( PropertyEditInfo const& eventInfo );
        void PostEdit( PropertyEditInfo const& eventInfo );

    private:

        PropertyGrid                    m_propertyGrid;
        ResourceID                      m_loadedMap;
        Entity*                         m_pSelectedEntity = nullptr;
        EntityComponent*                m_pSelectedComponent = nullptr;

        Transform                       m_editedTransform;
        ImGuiX::Gizmo                   m_gizmo;

        EventBindingID                  m_preEditBindingID;
        EventBindingID                  m_postEditBindingID;

        bool                            m_isGamePreviewRunning = false;
    };
}