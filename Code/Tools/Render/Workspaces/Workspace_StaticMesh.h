#pragma once

#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshWorkspace final : public TResourceWorkspace<StaticMesh>
    {

    public:

        using TResourceWorkspace::TResourceWorkspace;

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;

    private:

        Entity*         m_pPreviewEntity = nullptr;
        String          m_infoWindowName;
        bool            m_showVertices = false;
        bool            m_showNormals = false;
    };
}