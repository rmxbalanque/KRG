#pragma once

#include "../ResourceWorkspace.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshWorkspace : public Resource::TResourceWorkspace<StaticMesh>
    {
        static char const* const s_infoWindowName;

    public:

        using TResourceWorkspace::TResourceWorkspace;

        virtual char const* GetWorkspaceName() const override { return "Static Mesh"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

    private:

        Entity* m_pPreviewEntity = nullptr;
    };
}