#pragma once

#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimatedMeshComponent;

    //-------------------------------------------------------------------------

    class SkeletonResourceEditor : public TResourceEditorWorkspace<Skeleton>
    {
        static char const* const s_infoWindowName;
        static char const* const s_skeletonTreeWindowName;

        struct BoneInfo
        {
            inline void DestroyChildren()
            {
                for ( auto& pChild : m_children )
                {
                    pChild->DestroyChildren();
                    KRG::Delete( pChild );
                }

                m_children.clear();
            }

        public:

            int32                           m_boneIdx;
            TInlineVector<BoneInfo*, 5>     m_children;
            bool                            m_isExpanded = true;
        };

    public:

        using TResourceEditorWorkspace::TResourceEditorWorkspace;
        virtual ~SkeletonResourceEditor();

        virtual char const* GetWorkspaceName() const override { return "Skeleton"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDraw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void DrawInfoWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass );
        void DrawSkeletonHierarchyWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass );

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone );

    private:

        BoneInfo*                       m_pSkeletonTreeRoot = nullptr;
        StringID                        m_selectedBoneID;

        Entity*                         m_pPreviewEntity = nullptr;
        AnimatedMeshComponent*          m_pMeshComponent = nullptr;
        bool                            m_showPreviewMesh = true;
    };
}