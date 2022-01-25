#pragma once

#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "System/Animation/AnimationSkeleton.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class SkeletalMeshComponent;
}

namespace KRG::Animation
{
    class SkeletonWorkspace final : public TResourceWorkspace<Skeleton>
    {
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

        using TResourceWorkspace::TResourceWorkspace;
        virtual ~SkeletonWorkspace();

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded ) override;
        virtual void EndHotReload() override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        
        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;

        void DrawDetailsWindow( UpdateContext const& context );
        void DrawSkeletonHierarchyWindow( UpdateContext const& context );

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone );

        void CreatePreviewEntity();

    private:

        String                          m_skeletonTreeWindowName;
        String                          m_detailsWindowName;

        BoneInfo*                       m_pSkeletonTreeRoot = nullptr;
        StringID                        m_selectedBoneID;

        Entity*                         m_pPreviewEntity = nullptr;
        bool                            m_poseReset = true;
    };
}