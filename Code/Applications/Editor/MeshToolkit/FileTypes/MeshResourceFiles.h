#pragma once

#include "Applications/Editor/Editor/EditorFileTab.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    class StaticMeshResourceFile : public TResourceFileTab<StaticMesh>
    {
    public:

        using TResourceFileTab::TResourceFileTab;
    
        virtual void SetActive( EntityWorld* pPreviewWorld ) override;
        virtual void SetInactive( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Entity* m_pPreviewEntity = nullptr;
    };

    //-------------------------------------------------------------------------

    struct BoneInfo;

    class SkeletalMeshResourceFile : public TResourceFileTab<SkeletalMesh>
    {
    public:

        using TResourceFileTab::TResourceFileTab;
        virtual ~SkeletalMeshResourceFile();

        virtual void SetActive( EntityWorld* pPreviewWorld ) override;
        virtual void SetInactive( EntityWorld* pPreviewWorld ) override;
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