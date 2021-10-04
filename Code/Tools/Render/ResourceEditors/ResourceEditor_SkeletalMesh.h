#pragma once

#include "Tools/Core/Editors/ResourceEditorWorkspace.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct BoneInfo;

    class SkeletalMeshResourceEditor : public TResourceEditorWorkspace<SkeletalMesh>
    {
        static char const* const s_infoWindowName;

    public:

        using TResourceEditorWorkspace::TResourceEditorWorkspace;
        virtual ~SkeletalMeshResourceEditor();

        virtual char const* GetWorkspaceName() const override { return "Skeletal Mesh"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDraw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

    private:

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone ) const;

    private:

        Entity*     m_pPreviewEntity = nullptr;
        BoneInfo*   m_pSkeletonTreeRoot = nullptr;
    };
}