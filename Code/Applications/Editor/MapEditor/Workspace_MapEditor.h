#pragma once

#include "Tools/Core/Editor/EditorWorkspace.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Engine/Core/Entity/Map/EntityMapDescriptor.h"
#include "System/Render/Imgui/ImguiX.h"
#include "Engine/Core/Imgui/Gizmo.h"

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

        void CreateNewMap();
        void SelectAndLoadMap();
        void LoadMap( TResourcePtr<EntityModel::EntityMapDescriptor> const& mapToLoad );
        void SaveMap();
        void SaveMapAs();

    private:

        virtual char const* GetTitle() const override { return "Map Editor"; }
        virtual uint32 GetID() const override { return 0xFFFFFFFF; }

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual void DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual bool Save() override;

        void ClearSelection();

        void DrawEntityOutliner( UpdateContext const& context );
        void DrawComponentsView( UpdateContext const& context );
        void DrawPropertyGrid( UpdateContext const& context );

        void DrawComponentEntry( EntityComponent* pComponent );
        void DrawSpatialComponentTree( SpatialEntityComponent* pComponent );

    private:

        PropertyGrid                    m_propertyGrid;
        ResourceID                      m_loadedMap;
        Entity*                         m_pSelectedEntity = nullptr;
        EntityComponent*                m_pSelectedComponent = nullptr;

        Transform                       m_editedTransform;
        ImGuiX::Gizmo                   m_gizmo;
    };
}