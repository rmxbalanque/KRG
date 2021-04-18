#include "MeshEditor_MeshInfoTool.h"
#include "System/Resource/ResourceSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    MeshInfo::~MeshInfo()
    {
        KRG_ASSERT( m_pSkeletonTreeRoot == nullptr );
    }

    void MeshInfo::Initialize( UpdateContext const& context )
    {
        TEditorTool<Model>::Initialize( context );

        // Create preview entity
        //-------------------------------------------------------------------------
        // We dont own the entity as soon as we add it to the map

        auto pWorld = context.GetSystem<EntityWorld>();
        auto pPersistentMap = pWorld->GetPersistentMap();
        
        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );
    }

    void MeshInfo::Shutdown( UpdateContext const& context )
    {
        // We dont own the entity or the components, the entity world will destroy them correctly
        m_pPreviewEntity = nullptr;
        m_pStaticMeshComponent = nullptr;
        m_pSkeletalMeshComponent = nullptr;

        //-------------------------------------------------------------------------

        UnloadAnyLoadedResources( context );
        TEditorTool<Model>::Shutdown( context );
    }

    void MeshInfo::UnloadAnyLoadedResources( UpdateContext const& context )
    {
        // Destroy any created preview components
        //-------------------------------------------------------------------------

        if ( m_pStaticMeshComponent != nullptr )
        {
            m_pPreviewEntity->DestroyComponent( m_pStaticMeshComponent->GetID() );
            m_pStaticMeshComponent = nullptr;
        }

        if ( m_pSkeletalMeshComponent != nullptr )
        {
            m_pPreviewEntity->DestroyComponent( m_pSkeletalMeshComponent->GetID() );
            m_pSkeletalMeshComponent = nullptr;
        }

        // Unload any loaded resources
        //-------------------------------------------------------------------------

        auto pResourceSystem = context.GetSystem<Resource::ResourceSystem>();

        if ( m_pStaticMesh.IsValid() )
        {
            pResourceSystem->UnloadResource( m_pStaticMesh );
            m_pStaticMesh = nullptr;
        }

        if ( m_pSkeletalMesh.IsValid() )
        {
            pResourceSystem->UnloadResource( m_pSkeletalMesh );
            m_pSkeletalMesh = nullptr;
        }

        // Destroy skeleton tree
        //-------------------------------------------------------------------------

        DestroySkeletonTree();
        m_isLoadingMesh = false;
    }

    //-------------------------------------------------------------------------

    void MeshInfo::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        // Update preview mesh requests
        //-------------------------------------------------------------------------

        DataPath const& previewedMesh = m_model.GetPreviewedMeshPath();
        if ( previewedMesh.IsValid() && previewedMesh != m_currentlyPreviewedMesh )
        {
            // Unload any previously loaded meshes
            //-------------------------------------------------------------------------

            if ( m_currentlyPreviewedMesh.IsValid() )
            {
                UnloadAnyLoadedResources( context );
            }

            m_currentlyPreviewedMesh = previewedMesh;

            // Load mesh
            //-------------------------------------------------------------------------

            auto pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
            ResourceID const resourceID( previewedMesh );
            if ( resourceID.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
            {
                m_pStaticMesh = resourceID;
                pResourceSystem->LoadResource( m_pStaticMesh );
            }
            else
            {
                m_pSkeletalMesh = resourceID;
                pResourceSystem->LoadResource( m_pSkeletalMesh );
            }

            m_isLoadingMesh = true;
        }
        else
        {
            // Clear previewed mesh and unload any loaded meshes
            if ( !previewedMesh.IsValid() && m_currentlyPreviewedMesh.IsValid() )
            {
                m_currentlyPreviewedMesh = DataPath();
                UnloadAnyLoadedResources( context );
            }
        }

        // Handle loading
        //-------------------------------------------------------------------------

        if ( m_isLoadingMesh )
        {
            if ( !IsLoadingMesh() )
            {
                UpdatePreviewEntity();

                if ( IsPreviewingSkeletalMesh() )
                {
                    CreateSkeletonTree();
                }

                m_isLoadingMesh = false;
            }
        }

        // Draw Info
        //-------------------------------------------------------------------------

        DrawMeshInfoWindow( context );
    }

    void MeshInfo::UpdatePreviewEntity()
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );
        KRG_ASSERT( m_pSkeletalMeshComponent == nullptr && m_pStaticMeshComponent == nullptr );

        if ( IsPreviewingStaticMesh() )
        {
            m_pStaticMeshComponent = KRG::New<StaticMeshComponent>( StringID( "Static Mesh Component" ) );
            m_pStaticMeshComponent->SetMesh( m_pStaticMesh.GetResourceID() );
            m_pPreviewEntity->AddComponent( m_pStaticMeshComponent );
        }
        else // Skeletal Mesh
        {
            m_pSkeletalMeshComponent = KRG::New<SkeletalMeshComponent>( StringID( "Skeletal Mesh Component" ) );
            m_pSkeletalMeshComponent->SetMesh( m_pSkeletalMesh.GetResourceID() );
            m_pPreviewEntity->AddComponent( m_pSkeletalMeshComponent );
        }
    }

    void MeshInfo::DrawMeshInfoWindow( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Mesh Info" ) )
        {
            if ( !m_currentlyPreviewedMesh.IsValid() )
            {
                ImGui::Text( "No Mesh Previewed" );
            }
            else if ( m_isLoadingMesh )
            {
                ImGui::Text( "Loading:" );
                ImGui::SameLine();
                ImGuiX::DrawSpinner( "Loading" );
            }
            else // Draw mesh info
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
                    ImGui::Text( m_currentlyPreviewedMesh.c_str() );

                    //-------------------------------------------------------------------------

                    // Loading indicator
                    if ( IsLoadingMesh() )
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text( "Loading:" );

                        ImGui::TableNextColumn();
                        ImGuiX::DrawSpinner( "Loading" );
                    }
                    else // Draw basic mesh info
                    {
                        Mesh const* pMesh = IsPreviewingStaticMesh() ? static_cast<Mesh const*>( m_pStaticMesh.GetPtr() ) : static_cast<Mesh const*>( m_pSkeletalMesh.GetPtr() );

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

                        if ( IsPreviewingSkeletalMesh() )
                        {
                            ImGui::TableNextColumn();
                            ImGui::Text( "Num Bones:" );

                            ImGui::TableNextColumn();
                            ImGui::Text( "%d", m_pSkeletalMesh->GetNumBones() );
                        }
                    }

                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();

                //-------------------------------------------------------------------------

                if ( IsPreviewingSkeletalMesh())
                {
                    DrawSkeletalMeshInfo( context );
                }
            }
        }
        ImGui::End();
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
        KRG_ASSERT( IsPreviewingSkeletalMesh() );

        TVector<BoneInfo*> boneInfos;

        // Create all infos
        int32 const numBones = m_pSkeletalMesh->GetNumBones();
        for ( auto i = 0; i < numBones; i++ )
        {
            auto& pBoneInfo = boneInfos.emplace_back( KRG::New<BoneInfo>() );
            pBoneInfo->m_boneIdx = i;
        }

        // Create hierarchy
        for ( auto i = 1; i < numBones; i++ )
        {
            int32 const parentBoneIdx = m_pSkeletalMesh->GetBoneParentIndex( i );
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
        ImGui::SetNextItemOpen( pBone->m_isExpanded );
        int32 const treeNodeFlags = pBone->m_children.empty() ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_None;
        pBone->m_isExpanded = ImGui::TreeNodeEx( m_pSkeletalMesh->GetBoneID( pBone->m_boneIdx ).c_str(), treeNodeFlags );
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