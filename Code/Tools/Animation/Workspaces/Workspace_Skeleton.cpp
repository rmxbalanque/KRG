#include "Workspace_Skeleton.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "Engine/Animation/Components/Component_AnimatedMeshes.h"
#include "Engine/Animation/AnimationPose.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/DevUI/NumericUIHelpers.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Math/MathStringHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( SkeletonWorkspaceFactory, Skeleton, SkeletonWorkspace );

    //-------------------------------------------------------------------------

    SkeletonWorkspace::~SkeletonWorkspace()
    {
        DestroySkeletonTree();
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void SkeletonWorkspace::Initialize( UpdateContext const& context )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        TResourceWorkspace<Skeleton>::Initialize( context );

        m_skeletonTreeWindowName.sprintf( "Skeleton##%u", GetID() );
        m_detailsWindowName.sprintf( "Details##%u", GetID() );

        //-------------------------------------------------------------------------

        // We dont own the entity as soon as we add it to the map
        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        m_pWorld->GetPersistentMap()->AddEntity( m_pPreviewEntity );

        //-------------------------------------------------------------------------

        m_selectedBoneID = StringID();
    }

    void SkeletonWorkspace::Shutdown( UpdateContext const& context )
    {
        m_selectedBoneID = StringID();

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pPreviewEntity != nullptr );
        m_pPreviewEntity = nullptr;
        m_pMeshComponent = nullptr;

        TResourceWorkspace<Skeleton>::Shutdown( context );
    }

    void SkeletonWorkspace::BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded )
    {
        m_pPreviewEntity->DestroyComponent( m_pMeshComponent );
        m_pMeshComponent = nullptr;
    }

    void SkeletonWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID viewportDockID = 0;
        ImGuiID leftDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Left, 0.3f, nullptr, &viewportDockID );
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( viewportDockID, ImGuiDir_Down, 0.2f, nullptr, &viewportDockID );

        // Set Flags
        //-------------------------------------------------------------------------

        ImGuiDockNode* pViewportNode = ImGui::DockBuilderGetNode( viewportDockID );
        pViewportNode->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;

        // Dock windows
        //-------------------------------------------------------------------------

        ImGui::DockBuilderDockWindow( GetViewportWindowID(), viewportDockID );
        ImGui::DockBuilderDockWindow( m_skeletonTreeWindowName.c_str(), leftDockID );
        ImGui::DockBuilderDockWindow( m_descriptorWindowName.c_str(), bottomDockID );
        ImGui::DockBuilderDockWindow( m_detailsWindowName.c_str(), bottomDockID );
    }

    void SkeletonWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        // Debug drawing in Viewport
        //-------------------------------------------------------------------------

        if ( IsLoaded() )
        {
            auto drawingCtx = GetDrawingContext();

            // Draw skeleton
            drawingCtx.Draw( *m_pResource.GetPtr(), Transform::Identity );

            // Draw selected bone
            if ( m_selectedBoneID.IsValid() )
            {
                int32 const boneIdx = m_pResource->GetBoneIndex( m_selectedBoneID );
                if ( boneIdx != InvalidIndex )
                {
                    Transform const globalBoneTransform = m_pResource->GetBoneGlobalTransform( boneIdx );
                    drawingCtx.DrawAxis( globalBoneTransform, 0.25f, 3.0f );

                    Vector textLocation = globalBoneTransform.GetTranslation();
                    Vector const textLineLocation = textLocation - Vector( 0, 0, 0.01f );
                    drawingCtx.DrawText3D( textLocation, m_selectedBoneID.c_str(), Colors::Yellow );
                }
            }
        }

        // Preview Mesh
        //-------------------------------------------------------------------------

        if ( IsLoaded() )
        {
            // Initialize preview mesh
            if ( m_pMeshComponent == nullptr && m_pPreviewEntity->IsActivated() )
            {
                // Load resource descriptor for skeleton to get the preview mesh
                FileSystem::Path const resourceDescPath = m_pResource->GetResourcePath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );
                SkeletonResourceDescriptor resourceDesc;
                TryReadResourceDescriptorFromFile( *m_editorContext.m_pTypeRegistry, resourceDescPath, resourceDesc );

                // Create a preview mesh component
                m_pMeshComponent = KRG::New<AnimatedMeshComponent>( StringID( "Mesh Component" ) );
                m_pMeshComponent->SetSkeleton( m_pResource->GetResourceID() );
                if ( resourceDesc.m_previewMesh.IsValid() )
                {
                    m_pMeshComponent->SetMesh( resourceDesc.m_previewMesh.GetResourceID() );
                }
                m_pPreviewEntity->AddComponent( m_pMeshComponent );
            }
        }

        // UI
        //-------------------------------------------------------------------------

        DrawDescriptorWindow( context, pWindowClass );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawSkeletonHierarchyWindow( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawDetailsWindow( context );
    }

    void SkeletonWorkspace::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
    }

    void SkeletonWorkspace::DrawDetailsWindow( UpdateContext const& context )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 8 ) );
        if ( ImGui::Begin( m_detailsWindowName.c_str() ) )
        {
            if ( m_selectedBoneID.IsValid() )
            {
                int32 const selectedBoneIdx = m_pResource->GetBoneIndex( m_selectedBoneID );
                KRG_ASSERT( selectedBoneIdx != InvalidIndex );

                {
                    ImGuiX::ScopedFont sf( ImGuiX::Font::LargeBold );
                    ImGui::Text( "%d. %s", selectedBoneIdx, m_pResource->GetBoneID( selectedBoneIdx ).c_str() );
                }

                ImGui::NewLine();
                ImGui::Text( "Local Transform" );
                Transform const& localBoneTransform = m_pResource->GetLocalReferencePose()[selectedBoneIdx];
                ImGuiX::DisplayTransform( localBoneTransform );

                ImGui::NewLine();
                ImGui::Text( "Global Transform" );
                Transform const& globalBoneTransform = m_pResource->GetGlobalReferencePose()[selectedBoneIdx];
                ImGuiX::DisplayTransform( globalBoneTransform );
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void SkeletonWorkspace::DrawSkeletonHierarchyWindow( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_skeletonTreeWindowName.c_str() ) )
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

    ImRect SkeletonWorkspace::RenderSkeletonTree( BoneInfo* pBone )
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