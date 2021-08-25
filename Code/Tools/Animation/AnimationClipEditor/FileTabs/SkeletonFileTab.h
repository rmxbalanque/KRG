#pragma once

#include "Tools/Core/Editor/EditorFileTab.h"
#include "System/DevTools/ImguiX.h"
#include "Engine/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
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

    //-------------------------------------------------------------------------

    class SkeletonFileTab : public TResourceFileTab<Skeleton>
    {
    public:

        using TResourceFileTab::TResourceFileTab;
    
        ~SkeletonFileTab();

        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone ) const;

    private:

        BoneInfo* m_pSkeletonTreeRoot = nullptr;
    };
}