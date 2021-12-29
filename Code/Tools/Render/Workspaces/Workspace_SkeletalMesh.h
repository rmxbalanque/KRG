#pragma once

#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class SkeletalMeshComponent;

    //-------------------------------------------------------------------------

    class SkeletalMeshWorkspace : public TResourceWorkspace<SkeletalMesh>
    {
        struct BoneInfo
        {
            void DestroyChildren();

        public:

            int32                           m_boneIdx;
            TInlineVector<BoneInfo*, 5>     m_children;
            bool                            m_isExpanded = true;
        };

    public:

        using TResourceWorkspace::TResourceWorkspace;
        virtual ~SkeletalMeshWorkspace();

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void DrawUI( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone );

        void DrawSkeletonTreeWindow( UpdateContext const& context );
        void DrawMeshInfoWindow( UpdateContext const& context );
        void DrawDetailsWindow( UpdateContext const& context );

    private:

        Entity*                 m_pPreviewEntity = nullptr;
        SkeletalMeshComponent*  m_pMeshComponent = nullptr;
        BoneInfo*               m_pSkeletonTreeRoot = nullptr;
        StringID                m_selectedBoneID;

        String                  m_skeletonTreeWindowName;
        String                  m_meshInfoWindowName;
        String                  m_detailsWindowName;

        bool                    m_showNormals = false;
        bool                    m_showVertices = false;
        bool                    m_showBindPose = true;
        bool                    m_showBounds = true;
    };
}