#include "SkeletonWorkspace.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    SkeletonWorkspace::~SkeletonWorkspace()
    {
        DestroySkeletonTree();
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void SkeletonWorkspace::Activate( EntityWorld* pPreviewWorld )
    {
    }

    void SkeletonWorkspace::Deactivate( EntityWorld* pPreviewWorld )
    {
    }

    void SkeletonWorkspace::DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( IsLoading() )
        {
            ImGui::Text( "Loading:" );
            ImGui::SameLine();
            ImGuiX::DrawSpinner( "Loading" );
            return;
        }

        if ( HasLoadingFailed() )
        {
            ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
            return;
        }

        //-------------------------------------------------------------------------

        auto drawingCtx = context.GetDrawingContext();
        drawingCtx.Draw( *m_pResource.GetPtr(), Transform::Identity );

        //-------------------------------------------------------------------------

        if ( m_pSkeletonTreeRoot == nullptr )
        {
            CreateSkeletonTree();
        }

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 2, 4 ) );
        ImGui::BeginChild( "SkeletonView", ImVec2( 0, 0 ), true, ImGuiWindowFlags_AlwaysVerticalScrollbar );
        {
            KRG_ASSERT( m_pSkeletonTreeRoot != nullptr );
            RenderSkeletonTree( m_pSkeletonTreeRoot );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );
    }

    void SkeletonWorkspace::CreateSkeletonTree()
    {
        TVector<BoneInfo*> boneInfos;

        // Create all infos
        int32 const numBones = m_pResource->GetNumBones();
        for ( auto i = 0; i < numBones; i++ )
        {
            auto& pBoneInfo = boneInfos.emplace_back( KRG::New<BoneInfo>() );
            pBoneInfo->m_boneIdx = i;
        }

        // Create hierarchy
        for ( auto i = 1; i < numBones; i++ )
        {
            int32 const parentBoneIdx = m_pResource->GetParentBoneIndex( i );
            KRG_ASSERT( parentBoneIdx != InvalidIndex );
            boneInfos[parentBoneIdx]->m_children.emplace_back( boneInfos[i] );
        }

        // Set root
        m_pSkeletonTreeRoot = boneInfos[0];
    }

    void SkeletonWorkspace::DestroySkeletonTree()
    {
        if ( m_pSkeletonTreeRoot != nullptr )
        {
            m_pSkeletonTreeRoot->DestroyChildren();
            KRG::Delete( m_pSkeletonTreeRoot );
        }
    }

    ImRect SkeletonWorkspace::RenderSkeletonTree( BoneInfo* pBone ) const
    {
        ImGui::SetNextItemOpen( pBone->m_isExpanded );
        int32 const treeNodeFlags = pBone->m_children.empty() ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_None;
        
        Transform const refTransform = m_pResource->GetBoneTransform( pBone->m_boneIdx );
        EulerAngles const refRotation = refTransform.GetRotation().ToEulerAngles();

        String nodeString;
        nodeString.sprintf( "%s - R: %.2f, %.2f, %.2f - T: %.2f, %.2f, %.2f - S: %.2f, %.2f, %.2f", m_pResource->GetBoneID( pBone->m_boneIdx ).c_str(), Degrees( refRotation.m_x ).ToFloat(), Degrees( refRotation.m_y ).ToFloat(), Degrees( refRotation.m_z ).ToFloat(), refTransform.GetTranslation().m_x, refTransform.GetTranslation().m_y, refTransform.GetTranslation().m_z, refTransform.GetScale().m_x, refTransform.GetScale().m_y, refTransform.GetScale().m_z );

        pBone->m_isExpanded = ImGui::TreeNodeEx( nodeString.c_str(), treeNodeFlags );

        //-------------------------------------------------------------------------

        ImRect const nodeRect = ImRect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );

        if ( pBone->m_isExpanded )
        {
            ImColor const TreeLineColor = ImGui::GetColorU32( ImGuiCol_TextDisabled );
            float const SmallOffsetX = 2;
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
            verticalLineStart.x += SmallOffsetX;
            ImVec2 verticalLineEnd = verticalLineStart;

            for ( BoneInfo* pChild : pBone->m_children )
            {
                const float HorizontalTreeLineSize = 4.0f;
                const ImRect childRect = RenderSkeletonTree( pChild );
                const float midpoint = ( childRect.Min.y + childRect.Max.y ) / 2.0f;
                drawList->AddLine( ImVec2( verticalLineStart.x, midpoint ), ImVec2( verticalLineStart.x + HorizontalTreeLineSize, midpoint ), TreeLineColor );
                verticalLineEnd.y = midpoint;
            }

            drawList->AddLine( verticalLineStart, verticalLineEnd, TreeLineColor );
            ImGui::TreePop();
        }

        return nodeRect;
    }
}