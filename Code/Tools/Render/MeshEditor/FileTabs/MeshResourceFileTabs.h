#pragma once

#include "Tools/Core/Editor/EditorFileTab.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class StaticMeshFileTab : public TResourceFileTab<StaticMesh>
    {
    public:

        using TResourceFileTab::TResourceFileTab;
    
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Entity* m_pPreviewEntity = nullptr;
    };

    //-------------------------------------------------------------------------

    struct BoneInfo;

    class SkeletalMeshFileTab : public TResourceFileTab<SkeletalMesh>
    {
    public:

        using TResourceFileTab::TResourceFileTab;
        virtual ~SkeletalMeshFileTab();

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