#pragma once

#include "../ResourceWorkspace.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct BoneInfo;

    class SkeletalMeshWorkspace : public Resource::TResourceWorkspace<SkeletalMesh>
    {
    public:

        using TResourceWorkspace::TResourceWorkspace;
        virtual ~SkeletalMeshWorkspace();

        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone ) const;

    private:

        Entity* m_pPreviewEntity = nullptr;
        BoneInfo* m_pSkeletonTreeRoot = nullptr;
    };
}