#include "MeshResourceFiles.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    static void DrawCommonMeshInfo( Mesh const* pMesh, bool bIsSkeletal = false )
    {
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

            if ( bIsSkeletal )
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
    }
}

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    void StaticMeshResourceFile::SetActive( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        // We dont own the entity as soon as we add it to the map
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );

        auto pStaticMeshComponent = KRG::New<StaticMeshComponent>( StringID( "Static Mesh Component" ) );
        pStaticMeshComponent->SetMesh( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( pStaticMeshComponent );
    }

    void StaticMeshResourceFile::SetInactive( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );
        m_pPreviewEntity = nullptr;
    }

    void StaticMeshResourceFile::DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager )
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

        DrawCommonMeshInfo( m_pResource.GetPtr() );
    }
}

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
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

    SkeletalMeshResourceFile::~SkeletalMeshResourceFile()
    {
        DestroySkeletonTree();
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void SkeletalMeshResourceFile::SetActive( EntityWorld* pPreviewWorld )
    {
        // We dont own the entity as soon as we add it to the map
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );

        auto pStaticMeshComponent = KRG::New<SkeletalMeshComponent>( StringID( "Skeletal Mesh Component" ) );
        pStaticMeshComponent->SetMesh( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( pStaticMeshComponent );
    }

    void SkeletalMeshResourceFile::SetInactive( EntityWorld* pPreviewWorld )
    {
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );
        m_pPreviewEntity = nullptr;
    }

    void SkeletalMeshResourceFile::DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager )
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

        if ( m_pSkeletonTreeRoot == nullptr )
        {
            CreateSkeletonTree();
        }

        //-------------------------------------------------------------------------

        DrawCommonMeshInfo( m_pResource.GetPtr(), true );

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

    void SkeletalMeshResourceFile::CreateSkeletonTree()
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

    void SkeletalMeshResourceFile::DestroySkeletonTree()
    {
        if ( m_pSkeletonTreeRoot != nullptr )
        {
            m_pSkeletonTreeRoot->DestroyChildren();
            KRG::Delete( m_pSkeletonTreeRoot );
        }
    }

    ImRect SkeletalMeshResourceFile::RenderSkeletonTree( BoneInfo* pBone ) const
    {
        ImGui::SetNextItemOpen( pBone->m_isExpanded );
        int32 const treeNodeFlags = pBone->m_children.empty() ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_None;
        pBone->m_isExpanded = ImGui::TreeNodeEx( m_pResource->GetBoneID( pBone->m_boneIdx ).c_str(), treeNodeFlags );
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