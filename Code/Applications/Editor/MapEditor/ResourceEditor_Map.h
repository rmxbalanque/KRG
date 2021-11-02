#pragma once

#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class MapEditor final : public ResourceEditorWorkspace
    {
        static char const* const s_infoWindowName;

    public:

        MapEditor( ResourceEditorContext const& context );

        virtual char const* GetWorkspaceName() const override { return "Map Editor"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void OnNewMap();
        void OnLoadMap();
        void OnSaveMap();
        void OnSaveMapAs();

    private:

        virtual bool OnSave() override;

    private:

        EntityWorld*    m_pPreviewWorld = nullptr;
        ResourceID      m_loadedMap;
    };
}