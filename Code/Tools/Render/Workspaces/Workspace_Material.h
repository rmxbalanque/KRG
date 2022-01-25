#pragma once

#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Engine/Render/Material/RenderMaterial.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshComponent;

    //-------------------------------------------------------------------------

    class MaterialWorkspace final : public TResourceWorkspace<Material>
    {
    public:

        using TResourceWorkspace::TResourceWorkspace;

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual bool HasViewportToolbar() const { return true; }

    private:

        StaticMeshComponent*    m_pPreviewComponent = nullptr;
        String                  m_materialDetailsWindowName;
    };
}