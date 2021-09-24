#pragma once

#include "Tools/Core/Editors/ResourceEditorWorkspace.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "System/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshResourceEditor : public TResourceEditorWorkspace<StaticMesh>
    {
        static char const* const s_infoWindowName;

    public:

        using TResourceEditorWorkspace::TResourceEditorWorkspace;

        virtual char const* GetWorkspaceName() const override { return "Static Mesh"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

    private:

        Entity* m_pPreviewEntity = nullptr;
    };
}