#pragma once

#include "../ResourceWorkspace.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshWorkspace : public Resource::TResourceWorkspace<StaticMesh>
    {
    public:

        using TResourceWorkspace::TResourceWorkspace;
    
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Entity* m_pPreviewEntity = nullptr;
    };
}