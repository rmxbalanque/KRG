#include "MeshToolkit_MeshInfoTool.h"
#include "System/Resource/ResourceSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    MeshInfo::~MeshInfo()
    {
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void MeshInfo::Shutdown( UpdateContext const& context )
    {
        DestroySkeletonTree();
        TEditorTool<Model>::Shutdown( context );
    }

    //-------------------------------------------------------------------------

    void MeshInfo::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        // Manage skeleton tree state
        //-------------------------------------------------------------------------

        if ( m_skeletonTreeResource != m_model.GetActiveResource() )
        {
            DestroySkeletonTree();
        }

        if ( m_model.HasActiveResource() && !m_model.IsActiveResourceLoading() )
        {
            if ( !m_model.GetActiveResourcePtr().HasLoadingFailed() )
            {
                if ( m_model.IsPreviewingSkeletalMesh() )
                {
                    CreateSkeletonTree();
                }
            }
        }

        // Draw Window
        //-------------------------------------------------------------------------

        // Prevent anything docking into this window, and remove the tab-bar
        ImGuiWindowClass windowClass;
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingSplitOther | ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass( &windowClass );

        // Create timeline window
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        int32 const windowFlags = ImGuiWindowFlags_NoTitleBar;
        if ( ImGui::Begin( "Mesh Info", nullptr, windowFlags ) )
        {
            auto const& openResources = m_model.GetLoadedResources();

            if ( openResources.empty() )
            {
                ImGui::Text( "No previewed items" );
            }
            else // Draw tab bar and timeline hosts
            {
                TInlineVector<ResourceID, 4> resourcesToClose;
                int32 const tabBarFlags = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
                if ( ImGui::BeginTabBar( "OpenResourcesTabBar", tabBarFlags ) )
                {
                    for ( auto& resourcePtr : openResources )
                    {
                        bool isOpened = true;
                        if ( ImGui::BeginTabItem( resourcePtr.GetResourceID().c_str(), &isOpened, ImGuiTabItemFlags_None ) )
                        {
                            m_model.SetActiveResource( resourcePtr.GetResourceID() );

                            DrawMeshInfo( context );

                            ImGui::EndTabItem();
                        }

                        if ( !isOpened )
                        {
                            resourcesToClose.emplace_back( resourcePtr.GetResourceID() );
                        }
                    }
                    ImGui::EndTabBar();
                }

                for ( auto const& resourceID : resourcesToClose )
                {
                    m_model.UnloadResource( resourceID );
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void MeshInfo::DrawMeshInfo( UpdateContext const& context )
    {
        if ( !m_model.HasActiveResource() )
        {
            ImGui::Text( "No Mesh Previewed" );
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_model.IsActiveResourceLoading() )
        {
            ImGui::Text( "Loading:" );
            ImGui::SameLine();
            ImGuiX::DrawSpinner( "Loading" );
            return;
        }

        //-------------------------------------------------------------------------

        auto const& activeResourcePtr = m_model.GetActiveResourcePtr();
        if ( activeResourcePtr.HasLoadingFailed() )
        {
            ImGui::Text( "Loading Failed: %s", m_model.GetActiveResource().c_str() );
            return;
        }

        // Draw mesh info
        //-------------------------------------------------------------------------

        Mesh const* pMesh = nullptr;

        if ( m_model.IsPreviewingStaticMesh() )
        {
            TResourcePtr<StaticMesh> const pStaticMesh( activeResourcePtr );
            pMesh = pStaticMesh.GetPtr();
        }
        else
        {
            TResourcePtr<SkeletalMesh> const pSkeletalMesh( activeResourcePtr );
            pMesh = pSkeletalMesh.GetPtr();
        }

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
            ImGui::Text( m_model.GetActiveResource().c_str() );

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

            if ( m_model.IsPreviewingSkeletalMesh() )
            {
                auto const pSkeletalMesh = static_cast<SkeletalMesh const*>( pMesh );

                ImGui::TableNextColumn();
                ImGui::Text( "Num Bones:" );

                ImGui::TableNextColumn();
                ImGui::Text( "%d", pSkeletalMesh->GetNumBones() );
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        if ( m_model.IsPreviewingSkeletalMesh() && m_pSkeletonTreeRoot != nullptr )
        {
            DrawSkeletalMeshInfo( context );
        }
    }

    // Skeletal Mesh Info
    //-------------------------------------------------------------------------

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

    void MeshInfo::CreateSkeletonTree()
    {
        KRG_ASSERT( m_model.IsPreviewingSkeletalMesh() );
        TResourcePtr<SkeletalMesh> const pSkeletalMesh( m_model.GetActiveResourcePtr() );

        //-------------------------------------------------------------------------

        TVector<BoneInfo*> boneInfos;

        // Create all infos
        int32 const numBones = pSkeletalMesh->GetNumBones();
        for ( auto i = 0; i < numBones; i++ )
        {
            auto& pBoneInfo = boneInfos.emplace_back( KRG::New<BoneInfo>() );
            pBoneInfo->m_boneIdx = i;
        }

        // Create hierarchy
        for ( auto i = 1; i < numBones; i++ )
        {
            int32 const parentBoneIdx = pSkeletalMesh->GetBoneParentIndex( i );
            KRG_ASSERT( parentBoneIdx != InvalidIndex );
            boneInfos[parentBoneIdx]->m_children.emplace_back( boneInfos[i] );
        }

        // Set root
        m_pSkeletonTreeRoot = boneInfos[0];
    }

    void MeshInfo::DestroySkeletonTree()
    {
        if ( m_pSkeletonTreeRoot != nullptr )
        {
            m_pSkeletonTreeRoot->DestroyChildren();
            KRG::Delete( m_pSkeletonTreeRoot );
        }
    }

    ImRect MeshInfo::RenderSkeletonTree( BoneInfo* pBone ) const
    {
        KRG_ASSERT( m_model.IsPreviewingSkeletalMesh() );
        TResourcePtr<SkeletalMesh> const pSkeletalMesh( m_model.GetActiveResourcePtr() );

        //-------------------------------------------------------------------------

        ImGui::SetNextItemOpen( pBone->m_isExpanded );
        int32 const treeNodeFlags = pBone->m_children.empty() ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_None;
        pBone->m_isExpanded = ImGui::TreeNodeEx( pSkeletalMesh->GetBoneID( pBone->m_boneIdx ).c_str(), treeNodeFlags );
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

    void MeshInfo::DrawSkeletalMeshInfo( UpdateContext const& context )
    {
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
}