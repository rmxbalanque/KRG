#include "ResourceEditor_Skeleton.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/DevTools/CommonWidgets/NumericEditors.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( SkeletonWorkspaceFactory, Skeleton, SkeletonResourceEditor );

    char const* const SkeletonResourceEditor::s_infoWindowName = "Info##Skeleton";
    char const* const SkeletonResourceEditor::s_skeletonTreeWindowName = "Skeleton Tree##Skeleton";

    //-------------------------------------------------------------------------

    SkeletonResourceEditor::~SkeletonResourceEditor()
    {
        DestroySkeletonTree();
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void SkeletonResourceEditor::Activate( EntityWorld* pPreviewWorld )
    {
        m_selectedBoneID = StringID();
    }

    void SkeletonResourceEditor::Deactivate( EntityWorld* pPreviewWorld )
    {
        m_selectedBoneID = StringID();
    }

    void SkeletonResourceEditor::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID viewportDockID = 0;
        ImGuiID leftDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Left, 0.3f, nullptr, &viewportDockID );
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( viewportDockID, ImGuiDir_Down, 0.2f, nullptr, &viewportDockID );

        // Set Flags
        //-------------------------------------------------------------------------

        ImGuiDockNode* pViewportNode = ImGui::DockBuilderGetNode( viewportDockID );
        pViewportNode->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;

        ImGuiDockNode* pLeftNode = ImGui::DockBuilderGetNode( leftDockID );
        pLeftNode->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar;

        ImGuiDockNode* pBottomNode = ImGui::DockBuilderGetNode( bottomDockID );
        pBottomNode->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar;

        // Dock windows
        //-------------------------------------------------------------------------

        ImGui::DockBuilderDockWindow( GetViewportWindowName(), viewportDockID );
        ImGui::DockBuilderDockWindow( s_skeletonTreeWindowName, leftDockID );
        ImGui::DockBuilderDockWindow( s_infoWindowName, bottomDockID );
    }

    void SkeletonResourceEditor::Draw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass )
    {
        // Info
        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 8 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( s_infoWindowName ) )
        {
            if ( IsWaitingForResource() )
            {
                ImGui::Text( "Loading:" );
                ImGui::SameLine();
                ImGuiX::DrawSpinner( "Loading" );
            }
            else if ( HasLoadingFailed() )
            {
                ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
                return;
            }
            else
            {
                if ( m_selectedBoneID.IsValid() )
                {
                    int32 const boneIdx = m_pResource->GetBoneIndex( m_selectedBoneID );
                    KRG_ASSERT( boneIdx != InvalidIndex );
                    Transform const refTransform = m_pResource->GetBoneTransform( boneIdx );
                    Transform const refGlobalTransform = m_pResource->GetBoneGlobalTransform( boneIdx );

                    ImGui::Text( "Selected Bone: %s", m_selectedBoneID.c_str() );

                    ImGui::Separator();

                    EulerAngles const refRotation = refTransform.GetRotation().ToEulerAngles();
                    ImGui::Text( "Local Rotation: R: %.2f, %.2f, %.2f", Degrees( refRotation.m_x ).ToFloat(), Degrees( refRotation.m_y ).ToFloat(), Degrees( refRotation.m_z ).ToFloat() );
                    ImGui::Text( "Local Translation: %.2f %.2f %.2f", refTransform.GetTranslation().m_x, refTransform.GetTranslation().m_y, refTransform.GetTranslation().m_z );
                    ImGui::Text( "Local Scale: %.2f %.2f %.2f", refTransform.GetScale().m_x, refTransform.GetScale().m_y, refTransform.GetScale().m_z );

                    ImGui::Separator();

                    EulerAngles const refGlobalRotation = refGlobalTransform.GetRotation().ToEulerAngles();
                    ImGui::Text( "Global Rotation: R: %.2f, %.2f, %.2f", Degrees( refGlobalRotation.m_x ).ToFloat(), Degrees( refGlobalRotation.m_y ).ToFloat(), Degrees( refGlobalRotation.m_z ).ToFloat() );
                    ImGui::Text( "Global Translation: %.2f %.2f %.2f", refGlobalTransform.GetTranslation().m_x, refGlobalTransform.GetTranslation().m_y, refGlobalTransform.GetTranslation().m_z );
                    ImGui::Text( "Global Scale: %.2f %.2f %.2f", refGlobalTransform.GetScale().m_x, refGlobalTransform.GetScale().m_y, refGlobalTransform.GetScale().m_z );
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();

        // Skeleton Tree
        //-------------------------------------------------------------------------

        if ( ImGui::Begin( s_skeletonTreeWindowName ) )
        {
            if ( IsLoaded() )
            {
                if ( m_pSkeletonTreeRoot == nullptr )
                {
                    CreateSkeletonTree();
                }

                ImGui::Text( "Skeleton ID: %s", m_pResource->GetResourceID().c_str() );
                ImGui::Text( "Num Bones: %d", m_pResource->GetNumBones() );

                ImGui::Separator();

                KRG_ASSERT( m_pSkeletonTreeRoot != nullptr );
                RenderSkeletonTree( m_pSkeletonTreeRoot );
            }
        }
        ImGui::End();

        // Debug drawing in Viewport
        //-------------------------------------------------------------------------

        if ( IsLoaded() )
        {
            auto drawingCtx = context.GetDrawingContext();
            drawingCtx.Draw( *m_pResource.GetPtr(), Transform::Identity );

            if ( m_selectedBoneID.IsValid() )
            {
                int32 const boneIdx = m_pResource->GetBoneIndex( m_selectedBoneID );
                KRG_ASSERT( boneIdx != InvalidIndex );
                Transform const refGlobalTransform = m_pResource->GetBoneGlobalTransform( boneIdx );

                drawingCtx.DrawAxis( refGlobalTransform, 0.10f, 4.0f );

                Vector textLocation = refGlobalTransform.GetTranslation() + Vector( 0, 0, 0.1f );
                Vector const textLineLocation = textLocation - Vector( 0, 0, 0.01f );
                drawingCtx.DrawText3D( textLocation, m_selectedBoneID.c_str(), Colors::Yellow );
                drawingCtx.DrawLine( textLineLocation, refGlobalTransform.GetTranslation(), Colors::Yellow, 2.0f );
            }
        }
    }

    //-------------------------------------------------------------------------

    void SkeletonResourceEditor::CreateSkeletonTree()
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

    void SkeletonResourceEditor::DestroySkeletonTree()
    {
        if ( m_pSkeletonTreeRoot != nullptr )
        {
            m_pSkeletonTreeRoot->DestroyChildren();
            KRG::Delete( m_pSkeletonTreeRoot );
        }
    }

    ImRect SkeletonResourceEditor::RenderSkeletonTree( BoneInfo* pBone )
    {
        StringID const currentBoneID = m_pResource->GetBoneID( pBone->m_boneIdx );

        ImGui::SetNextItemOpen( pBone->m_isExpanded );
        int32 treeNodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if ( pBone->m_children.empty() )
        {
            treeNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        }

        if( currentBoneID == m_selectedBoneID )
        {
            treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        pBone->m_isExpanded = ImGui::TreeNodeEx( currentBoneID.c_str(), treeNodeFlags );

        // Handle bone selection
        if( ImGui::IsItemClicked() )
        {
            m_selectedBoneID = m_pResource->GetBoneID( pBone->m_boneIdx );
        }

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