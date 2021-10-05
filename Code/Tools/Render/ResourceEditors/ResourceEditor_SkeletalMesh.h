#pragma once

#include "Tools/Core/Editors/ResourceEditorWorkspace.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class SkeletalMeshComponent;

    //-------------------------------------------------------------------------

    class SkeletalMeshResourceEditor : public TResourceEditorWorkspace<SkeletalMesh>
    {
        static char const* const s_infoWindowName;
        static char const* const s_skeletonWindowName;

        struct BoneInfo
        {
            void DestroyChildren();

        public:

            int32                           m_boneIdx;
            TInlineVector<BoneInfo*, 5>     m_children;
            bool                            m_isExpanded = true;
        };

    public:

        using TResourceEditorWorkspace::TResourceEditorWorkspace;
        virtual ~SkeletalMeshResourceEditor();

        virtual char const* GetWorkspaceName() const override { return "Skeletal Mesh"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDraw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone ) const;

        void DrawInfoWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass );
        void DrawSkeletonTreeWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass );

    private:

        Entity*                 m_pPreviewEntity = nullptr;
        SkeletalMeshComponent*  m_pMeshComponent = nullptr;
        BoneInfo*               m_pSkeletonTreeRoot = nullptr;

        bool                    m_showBindPose = true;
        bool                    m_showBounds = true;
    };
}