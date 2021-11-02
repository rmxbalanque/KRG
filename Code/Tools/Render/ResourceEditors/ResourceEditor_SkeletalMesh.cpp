#include "ResourceEditor_SkeletalMesh.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Math/MathStringHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    KRG_RESOURCE_WORKSPACE_FACTORY( SkeletalMeshWorkspaceFactory, SkeletalMesh, SkeletalMeshResourceEditor );

    //-------------------------------------------------------------------------

    char const* const SkeletalMeshResourceEditor::s_infoWindowName = "Info Window##SkeletalMesh";
    char const* const SkeletalMeshResourceEditor::s_skeletonWindowName = "Bone Hierarchy##SkeletalMesh";

    //-------------------------------------------------------------------------

    void SkeletalMeshResourceEditor::BoneInfo::DestroyChildren()
    {
        for ( auto& pChild : m_children )
        {
            pChild->DestroyChildren();
            KRG::Delete( pChild );
        }

        m_children.clear();
    }

    //-------------------------------------------------------------------------

    SkeletalMeshResourceEditor::~SkeletalMeshResourceEditor()
    {
        DestroySkeletonTree();
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void SkeletalMeshResourceEditor::Activate( EntityWorld* pPreviewWorld )
    {
        // We dont own the entity as soon as we add it to the map
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );

        m_pMeshComponent = KRG::New<SkeletalMeshComponent>( StringID( "Skeletal Mesh Component" ) );
        m_pMeshComponent->SetMesh( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( m_pMeshComponent );
    }

    void SkeletalMeshResourceEditor::Deactivate( EntityWorld* pPreviewWorld )
    {
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );
        m_pPreviewEntity = nullptr;
        m_pMeshComponent = nullptr;
    }

    void SkeletalMeshResourceEditor::InitializeDockingLayout( ImGuiID dockspaceID ) const
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
        ImGui::DockBuilderDockWindow( s_skeletonWindowName, leftDockID );
        ImGui::DockBuilderDockWindow( s_infoWindowName, bottomDockID );
    }

    void SkeletalMeshResourceEditor::UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        if ( IsLoaded() )
        {
            /*auto drawingCtx = context.GetDrawingContext();

            if ( m_showBounds )
            {
                drawingCtx.DrawWireBox( m_pResource->GetBounds(), Colors::Cyan );
            }

            if ( m_showBindPose )
            {
                m_pResource->DrawBindPose( drawingCtx, Transform::Identity );
            }

            if ( m_showBindPose )
            {
                if ( m_pMeshComponent != nullptr && m_pMeshComponent->IsInitialized() )
                {
                    m_pMeshComponent->DrawPose( drawingCtx );
                }
            }*/
        }

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        DrawInfoWindow( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawSkeletonTreeWindow( context );
    }

    //-------------------------------------------------------------------------

    void SkeletalMeshResourceEditor::DrawInfoWindow( UpdateContext const& context )
    {
        
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
            }
            else // Draw UI
            {
                auto pMesh = m_pResource.GetPtr();
                KRG_ASSERT( pMesh != nullptr );

                //-------------------------------------------------------------------------
                // Draw Mesh Data
                //-------------------------------------------------------------------------

                ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 2 ) );
                if ( ImGui::BeginTable( "MeshInfoTable", 2, ImGuiTableFlags_Borders ) )
                {
                    ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthFixed, 110 );
                    ImGui::TableSetupColumn( "Data", ImGuiTableColumnFlags_NoHide );

                    //-------------------------------------------------------------------------

                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text( "Data Path:" );

                    ImGui::TableNextColumn();
                    ImGui::Text( pMesh->GetResourceID().c_str() );

                    //-------------------------------------------------------------------------

                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text( "Num Vertices:" );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%d", pMesh->GetNumVertices() );

                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text( "Num Indices:" );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%d", pMesh->GetNumIndices() );

                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text( "Geometry Sections:" );

                    ImGui::TableNextColumn();
                    for ( auto const& section : pMesh->GetSections() )
                    {
                        ImGui::Text( section.m_ID.c_str() );
                    }

                    auto const pSkeletalMesh = static_cast<SkeletalMesh const*>( pMesh );

                    ImGui::TableNextColumn();
                    ImGui::Text( "Num Bones:" );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%d", pSkeletalMesh->GetNumBones() );

                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();

                //-------------------------------------------------------------------------
                // Draw Bind Pose Data
                //-------------------------------------------------------------------------

                if ( ImGui::BeginTable( "SkeletonDataTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg ) )
                {
                    ImGui::TableSetupColumn( "Bone", ImGuiTableColumnFlags_WidthStretch );
                    ImGui::TableSetupColumn( "Bind Transform", ImGuiTableColumnFlags_WidthStretch );

                    //-------------------------------------------------------------------------

                    ImGui::TableHeadersRow();

                    int32 const numBones = m_pResource->GetNumBones();

                    for ( auto i = 0; i < numBones; i++ )
                    {
                        Transform const& boneBindPoseTransform = m_pResource->GetBindPose()[i];

                        ImGui::TableNextColumn();
                        ImGui::Text( "%d. %s", i, m_pResource->GetBoneID( i ).c_str() );

                        ImGui::TableNextColumn();
                        ImGui::Text( "Rot: %s", Math::ToString( boneBindPoseTransform.GetRotation() ).c_str() );
                        ImGui::Text( "Tra: %s", Math::ToString( boneBindPoseTransform.GetTranslation() ).c_str() );
                        ImGui::Text( "Scl: %s", Math::ToString( boneBindPoseTransform.GetScale() ).c_str() );
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();
    }

    void SkeletalMeshResourceEditor::DrawSkeletonTreeWindow( UpdateContext const& context )
    {
        if ( ImGui::Begin( s_skeletonWindowName ) )
        {
            if ( IsLoaded() )
            {
                if ( m_pSkeletonTreeRoot == nullptr )
                {
                    CreateSkeletonTree();
                }

                KRG_ASSERT( m_pSkeletonTreeRoot != nullptr );
                RenderSkeletonTree( m_pSkeletonTreeRoot );
            }
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void SkeletalMeshResourceEditor::CreateSkeletonTree()
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
            int32 const parentBoneIdx = m_pResource->GetBoneParentIndex( i );
            KRG_ASSERT( parentBoneIdx != InvalidIndex );
            boneInfos[parentBoneIdx]->m_children.emplace_back( boneInfos[i] );
        }

        // Set root
        m_pSkeletonTreeRoot = boneInfos[0];
    }

    void SkeletalMeshResourceEditor::DestroySkeletonTree()
    {
        if ( m_pSkeletonTreeRoot != nullptr )
        {
            m_pSkeletonTreeRoot->DestroyChildren();
            KRG::Delete( m_pSkeletonTreeRoot );
        }
    }

    ImRect SkeletalMeshResourceEditor::RenderSkeletonTree( BoneInfo* pBone ) const
    {
        ImGui::SetNextItemOpen( pBone->m_isExpanded );

        int32 treeNodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if ( pBone->m_children.empty() )
        {
            treeNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        }

        InlineString<128> boneLabel;
        boneLabel.sprintf( "%d. %s", pBone->m_boneIdx, m_pResource->GetBoneID( pBone->m_boneIdx ).c_str() );
        pBone->m_isExpanded = ImGui::TreeNodeEx( boneLabel.c_str(), treeNodeFlags );

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

    void SkeletalMeshResourceEditor::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        if ( !IsLoaded() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::Checkbox( "Show Bind Pose", &m_showBindPose );

        ImGuiX::VerticalSeparator();

        ImGui::Checkbox( "Show Bounds", &m_showBounds );
    }
}