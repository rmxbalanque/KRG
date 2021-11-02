#include "ResourceEditor_Skeleton.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Animation/Components/AnimatedMeshComponent.h"
#include "Engine/Animation/AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( SkeletonWorkspaceFactory, Skeleton, SkeletonResourceEditor );

    char const* const SkeletonResourceEditor::s_infoWindowName = "Skeleton Info##Skeleton";
    char const* const SkeletonResourceEditor::s_skeletonTreeWindowName = "Skeleton Hierarchy##Skeleton";

    //-------------------------------------------------------------------------

    SkeletonResourceEditor::~SkeletonResourceEditor()
    {
        DestroySkeletonTree();
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void SkeletonResourceEditor::Activate( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        // Load resource descriptor for skeleton to get the preview mesh
        FileSystem::Path const resourceDescPath = m_pResource.GetResourceID().GetPath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );
        SkeletonResourceDescriptor resourceDesc;
        TryReadResourceDescriptorFromFile( *m_editorContext.m_pTypeRegistry, resourceDescPath, resourceDesc );

        // We dont own the entity as soon as we add it to the map
        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        m_pMeshComponent = KRG::New<AnimatedMeshComponent>( StringID( "Mesh Component" ) );
        m_pMeshComponent->SetSkeleton( m_pResource.GetResourceID() );
        m_pMeshComponent->SetMesh( resourceDesc.m_previewMesh.GetResourceID() );
        m_pPreviewEntity->AddComponent( m_pMeshComponent );

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->AddEntity( m_pPreviewEntity );

        //-------------------------------------------------------------------------

        m_selectedBoneID = StringID();
    }

    void SkeletonResourceEditor::Deactivate( EntityWorld* pPreviewWorld )
    {
        m_selectedBoneID = StringID();

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pPreviewEntity != nullptr );

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );
        m_pPreviewEntity = nullptr;
        m_pMeshComponent = nullptr;
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

    void SkeletonResourceEditor::UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        // Debug drawing in Viewport
        //-------------------------------------------------------------------------

        if ( IsLoaded() )
        {
            //auto drawingCtx = context.GetDrawingContext();
            //drawingCtx.Draw( *m_pResource.GetPtr(), Transform::Identity );

            //if ( m_selectedBoneID.IsValid() )
            //{
            //    int32 const boneIdx = m_pResource->GetBoneIndex( m_selectedBoneID );
            //    KRG_ASSERT( boneIdx != InvalidIndex );
            //    Transform const refGlobalTransform = m_pResource->GetBoneGlobalTransform( boneIdx );

            //    drawingCtx.DrawAxis( refGlobalTransform, 0.10f, 4.0f );

            //    Vector textLocation = refGlobalTransform.GetTranslation() + Vector( 0, 0, 0.1f );
            //    Vector const textLineLocation = textLocation - Vector( 0, 0, 0.01f );
            //    drawingCtx.DrawText3D( textLocation, m_selectedBoneID.c_str(), Colors::Yellow );
            //    drawingCtx.DrawLine( textLineLocation, refGlobalTransform.GetTranslation(), Colors::Yellow, 2.0f );
            //}

            ////-------------------------------------------------------------------------

            //if ( m_pMeshComponent != nullptr && m_pMeshComponent->IsInitialized() )
            //{
            //    if ( m_showPreviewMesh )
            //    {
            //        Pose referencePose( m_pResource.GetPtr() );
            //        referencePose.CalculateGlobalTransforms();

            //        m_pMeshComponent->SetPose( &referencePose );
            //        m_pMeshComponent->FinalizePose();
            //    }
            //    else
            //    {
            //        //m_pMeshComponent->SetHidden();
            //    }
            //}
        }

        // UI
        //-------------------------------------------------------------------------

        DrawInfoWindow( context, pWindowClass );
        DrawSkeletonHierarchyWindow( context, pWindowClass );
    }

    void SkeletonResourceEditor::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        ImGui::Checkbox( "Show Preview Mesh", &m_showPreviewMesh );
    }

    void SkeletonResourceEditor::DrawInfoWindow( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
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
                if ( ImGui::BeginTable( "SkeletonDataTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg ) )
                {
                    ImGui::TableSetupColumn( "Bone", ImGuiTableColumnFlags_WidthStretch );
                    ImGui::TableSetupColumn( "Local Transform", ImGuiTableColumnFlags_WidthStretch );
                    ImGui::TableSetupColumn( "Global Transform", ImGuiTableColumnFlags_WidthStretch );

                    //-------------------------------------------------------------------------

                    ImGui::TableHeadersRow();

                    int32 const numBones = m_pResource->GetNumBones();

                    for ( auto i = 0; i < numBones; i++ )
                    {
                        Transform const& localBoneTransform = m_pResource->GetLocalReferencePose()[i];
                        Transform const& globalBoneTransform = m_pResource->GetGlobalReferencePose()[i];

                        ImGui::TableNextColumn();
                        ImGui::Text( "%d. %s", i, m_pResource->GetBoneID( i ).c_str() );

                        ImGui::TableNextColumn();
                        ImGui::Text( "Rot: %s", Math::ToString( localBoneTransform.GetRotation() ).c_str() );
                        ImGui::Text( "Tra: %s", Math::ToString( localBoneTransform.GetTranslation() ).c_str() );
                        ImGui::Text( "Scl: %s", Math::ToString( localBoneTransform.GetScale() ).c_str() );

                        ImGui::TableNextColumn();
                        ImGui::Text( "Rot: %s", Math::ToString( globalBoneTransform.GetRotation() ).c_str() );
                        ImGui::Text( "Tra: %s", Math::ToString( globalBoneTransform.GetTranslation() ).c_str() );
                        ImGui::Text( "Scl: %s", Math::ToString( globalBoneTransform.GetScale() ).c_str() );
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void SkeletonResourceEditor::DrawSkeletonHierarchyWindow( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        ImGui::SetNextWindowClass( pWindowClass );
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

        InlineString<128> boneLabel;
        boneLabel.sprintf( "%d. %s", pBone->m_boneIdx, currentBoneID.c_str() );
        pBone->m_isExpanded = ImGui::TreeNodeEx( boneLabel.c_str(), treeNodeFlags );

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