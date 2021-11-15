#pragma once

#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "System/Render/RenderTexture.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class TextureWorkspace final : public TResourceWorkspace<Texture>
    {

    public:

        using TResourceWorkspace::TResourceWorkspace;

    private:

        virtual bool HasViewportWindow() const { return false; }
        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

    private:

        String          m_previewWindowName;
        String          m_infoWindowName;
    };
}