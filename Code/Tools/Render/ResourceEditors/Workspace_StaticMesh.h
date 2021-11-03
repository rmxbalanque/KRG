#pragma once

#include "Tools/Core/Editor/ResourceEditorWorkspace.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshResourceEditor final : public TResourceEditorWorkspace<StaticMesh>
    {
        static char const* const s_infoWindowName;

    public:

        using TResourceEditorWorkspace::TResourceEditorWorkspace;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        virtual void Initialize() override;

    private:

        Entity* m_pPreviewEntity = nullptr;
    };
}