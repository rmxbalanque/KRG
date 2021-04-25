#include "AnimationToolkit_SkeletonTreeTool.h"
#include "System/Resource/ResourceSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    SkeletonTree::~SkeletonTree()
    {
        
    }

    void SkeletonTree::Initialize( UpdateContext const& context )
    {
        TEditorTool<Model>::Initialize( context );

        // Create preview entity
        //-------------------------------------------------------------------------
        // We dont own the entity as soon as we add it to the map

        auto pWorld = context.GetSystem<EntityWorld>();
        auto pPersistentMap = pWorld->GetPersistentMap();
        
        //m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        //pPersistentMap->AddEntity( m_pPreviewEntity );
    }

    void SkeletonTree::Shutdown( UpdateContext const& context )
    {
        // We dont own the entity or the components, the entity world will destroy them correctly
        //m_pPreviewEntity = nullptr;
        //m_pStaticAnimationComponent = nullptr;
        //m_pSkeletalAnimationComponent = nullptr;

        //-------------------------------------------------------------------------

       // UnloadAnyLoadedResources( context );
        TEditorTool<Model>::Shutdown( context );
    }

    //void AnimationInfo::UnloadAnyLoadedResources( UpdateContext const& context )
    //{
    //    // Destroy any created preview components
    //    //-------------------------------------------------------------------------

    //    if ( m_pStaticAnimationComponent != nullptr )
    //    {
    //        m_pPreviewEntity->DestroyComponent( m_pStaticAnimationComponent->GetID() );
    //        m_pStaticAnimationComponent = nullptr;
    //    }

    //    if ( m_pSkeletalAnimationComponent != nullptr )
    //    {
    //        m_pPreviewEntity->DestroyComponent( m_pSkeletalAnimationComponent->GetID() );
    //        m_pSkeletalAnimationComponent = nullptr;
    //    }

    //    // Unload any loaded resources
    //    //-------------------------------------------------------------------------

    //    auto pResourceSystem = context.GetSystem<Resource::ResourceSystem>();

    //    if ( m_pStaticAnimation.IsValid() )
    //    {
    //        pResourceSystem->UnloadResource( m_pStaticAnimation );
    //        m_pStaticAnimation = nullptr;
    //    }

    //    if ( m_pSkeletalAnimation.IsValid() )
    //    {
    //        pResourceSystem->UnloadResource( m_pSkeletalAnimation );
    //        m_pSkeletalAnimation = nullptr;
    //    }

    //    // Destroy skeleton tree
    //    //-------------------------------------------------------------------------

    //    DestroySkeletonTree();
    //    m_isLoadingAnimation = false;
    //}

    //-------------------------------------------------------------------------

    void SkeletonTree::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
    //    // Update preview mesh requests
    //    //-------------------------------------------------------------------------

    //    DataPath const& previewedAnimation = m_model.GetPreviewedAnimationPath();
    //    if ( previewedAnimation.IsValid() && previewedAnimation != m_currentlyPreviewedAnimation )
    //    {
    //        // Unload any previously loaded meshes
    //        //-------------------------------------------------------------------------

    //        if ( m_currentlyPreviewedAnimation.IsValid() )
    //        {
    //            UnloadAnyLoadedResources( context );
    //        }

    //        m_currentlyPreviewedAnimation = previewedAnimation;

    //        // Load mesh
    //        //-------------------------------------------------------------------------

    //        auto pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
    //        ResourceID const resourceID( previewedAnimation );
    //        if ( resourceID.GetResourceTypeID() == StaticAnimation::GetStaticResourceTypeID() )
    //        {
    //            m_pStaticAnimation = resourceID;
    //            pResourceSystem->LoadResource( m_pStaticAnimation );
    //        }
    //        else
    //        {
    //            m_pSkeletalAnimation = resourceID;
    //            pResourceSystem->LoadResource( m_pSkeletalAnimation );
    //        }

    //        m_isLoadingAnimation = true;
    //    }
    //    else
    //    {
    //        // Clear previewed mesh and unload any loaded meshes
    //        if ( !previewedAnimation.IsValid() && m_currentlyPreviewedAnimation.IsValid() )
    //        {
    //            m_currentlyPreviewedAnimation = DataPath();
    //            UnloadAnyLoadedResources( context );
    //        }
    //    }

    //    // Handle loading
    //    //-------------------------------------------------------------------------

    //    if ( m_isLoadingAnimation )
    //    {
    //        if ( !IsLoadingAnimation() )
    //        {
    //            UpdatePreviewEntity();

    //            if ( IsPreviewingSkeletalAnimation() )
    //            {
    //                CreateSkeletonTree();
    //            }

    //            m_isLoadingAnimation = false;
    //        }
    //    }

    //    // Draw Info
    //    //-------------------------------------------------------------------------

    //    DrawAnimationInfoWindow( context );
    }

    //void AnimationInfo::UpdatePreviewEntity()
    //{
    //    KRG_ASSERT( m_pPreviewEntity != nullptr );
    //    KRG_ASSERT( m_pSkeletalAnimationComponent == nullptr && m_pStaticAnimationComponent == nullptr );

    //    if ( IsPreviewingStaticAnimation() )
    //    {
    //        m_pStaticAnimationComponent = KRG::New<StaticAnimationComponent>( StringID( "Static Animation Component" ) );
    //        m_pStaticAnimationComponent->SetAnimation( m_pStaticAnimation.GetResourceID() );
    //        m_pPreviewEntity->AddComponent( m_pStaticAnimationComponent );
    //    }
    //    else // Skeletal Animation
    //    {
    //        m_pSkeletalAnimationComponent = KRG::New<SkeletalAnimationComponent>( StringID( "Skeletal Animation Component" ) );
    //        m_pSkeletalAnimationComponent->SetAnimation( m_pSkeletalAnimation.GetResourceID() );
    //        m_pPreviewEntity->AddComponent( m_pSkeletalAnimationComponent );
    //    }
    //}

    //void AnimationInfo::DrawAnimationInfoWindow( UpdateContext const& context )
    //{
    //    if ( ImGui::Begin( "Animation Info" ) )
    //    {
    //        if ( !m_currentlyPreviewedAnimation.IsValid() )
    //        {
    //            ImGui::Text( "No Animation Previewed" );
    //        }
    //        else if ( m_isLoadingAnimation )
    //        {
    //            ImGui::Text( "Loading:" );
    //            ImGui::SameLine();
    //            ImGuiX::DrawSpinner( "Loading" );
    //        }
    //        else // Draw mesh info
    //        {
    //            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 2 ) );
    //            if ( ImGui::BeginTable( "AnimationInfoTable", 2, ImGuiTableFlags_Borders ) )
    //            {
    //                ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthFixed, 110 );
    //                ImGui::TableSetupColumn( "Data", ImGuiTableColumnFlags_NoHide );

    //                //-------------------------------------------------------------------------

    //                ImGui::TableNextRow();

    //                ImGui::TableNextColumn();
    //                ImGui::Text( "Data Path:" );

    //                ImGui::TableNextColumn();
    //                ImGui::Text( m_currentlyPreviewedAnimation.c_str() );

    //                //-------------------------------------------------------------------------

    //                // Loading indicator
    //                if ( IsLoadingAnimation() )
    //                {
    //                    ImGui::TableNextColumn();
    //                    ImGui::Text( "Loading:" );

    //                    ImGui::TableNextColumn();
    //                    ImGuiX::DrawSpinner( "Loading" );
    //                }
    //                else // Draw basic mesh info
    //                {
    //                    Animation const* pAnimation = IsPreviewingStaticAnimation() ? static_cast<Animation const*>( m_pStaticAnimation.GetPtr() ) : static_cast<Animation const*>( m_pSkeletalAnimation.GetPtr() );

    //                    ImGui::TableNextRow();

    //                    ImGui::TableNextColumn();
    //                    ImGui::Text( "Num Vertices:" );

    //                    ImGui::TableNextColumn();
    //                    ImGui::Text( "%d", pAnimation->GetNumVertices() );

    //                    ImGui::TableNextRow();

    //                    ImGui::TableNextColumn();
    //                    ImGui::Text( "Num Indices:" );

    //                    ImGui::TableNextColumn();
    //                    ImGui::Text( "%d", pAnimation->GetNumIndices() );

    //                    ImGui::TableNextRow();

    //                    ImGui::TableNextColumn();
    //                    ImGui::Text( "Geometry Sections:" );

    //                    ImGui::TableNextColumn();
    //                    for ( auto const& section : pAnimation->GetSections() )
    //                    {
    //                        ImGui::Text( section.m_ID.c_str() );
    //                    }

    //                    if ( IsPreviewingSkeletalAnimation() )
    //                    {
    //                        ImGui::TableNextColumn();
    //                        ImGui::Text( "Num Bones:" );

    //                        ImGui::TableNextColumn();
    //                        ImGui::Text( "%d", m_pSkeletalAnimation->GetNumBones() );
    //                    }
    //                }

    //                ImGui::EndTable();
    //            }
    //            ImGui::PopStyleVar();

    //            //-------------------------------------------------------------------------

    //            if ( IsPreviewingSkeletalAnimation())
    //            {
    //                DrawSkeletalAnimationInfo( context );
    //            }
    //        }
    //    }
    //    ImGui::End();
    //}

    //// Skeletal Animation Info
    ////-------------------------------------------------------------------------

    //struct BoneInfo
    //{
    //    inline void DestroyChildren()
    //    {
    //        for ( auto& pChild : m_children )
    //        {
    //            pChild->DestroyChildren();
    //            KRG::Delete( pChild );
    //        }

    //        m_children.clear();
    //    }

    //public:

    //    int32                           m_boneIdx;
    //    TInlineVector<BoneInfo*, 5>     m_children;
    //    bool                            m_isExpanded = true;
    //};

    //void AnimationInfo::CreateSkeletonTree()
    //{
    //    KRG_ASSERT( IsPreviewingSkeletalAnimation() );

    //    TVector<BoneInfo*> boneInfos;

    //    // Create all infos
    //    int32 const numBones = m_pSkeletalAnimation->GetNumBones();
    //    for ( auto i = 0; i < numBones; i++ )
    //    {
    //        auto& pBoneInfo = boneInfos.emplace_back( KRG::New<BoneInfo>() );
    //        pBoneInfo->m_boneIdx = i;
    //    }

    //    // Create hierarchy
    //    for ( auto i = 1; i < numBones; i++ )
    //    {
    //        int32 const parentBoneIdx = m_pSkeletalAnimation->GetBoneParentIndex( i );
    //        KRG_ASSERT( parentBoneIdx != InvalidIndex );
    //        boneInfos[parentBoneIdx]->m_children.emplace_back( boneInfos[i] );
    //    }

    //    // Set root
    //    m_pSkeletonTreeRoot = boneInfos[0];
    //}

    //void AnimationInfo::DestroySkeletonTree()
    //{
    //    if ( m_pSkeletonTreeRoot != nullptr )
    //    {
    //        m_pSkeletonTreeRoot->DestroyChildren();
    //        KRG::Delete( m_pSkeletonTreeRoot );
    //    }
    //}

    //ImRect AnimationInfo::RenderSkeletonTree( BoneInfo* pBone ) const
    //{
    //    ImGui::SetNextItemOpen( pBone->m_isExpanded );
    //    int32 const treeNodeFlags = pBone->m_children.empty() ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_None;
    //    pBone->m_isExpanded = ImGui::TreeNodeEx( m_pSkeletalAnimation->GetBoneID( pBone->m_boneIdx ).c_str(), treeNodeFlags );
    //    ImRect const nodeRect = ImRect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );

    //    if ( pBone->m_isExpanded )
    //    {
    //        ImColor const TreeLineColor = ImGui::GetColorU32( ImGuiCol_TextDisabled );
    //        float const SmallOffsetX = 2;
    //        ImDrawList* drawList = ImGui::GetWindowDrawList();

    //        ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
    //        verticalLineStart.x += SmallOffsetX;
    //        ImVec2 verticalLineEnd = verticalLineStart;

    //        for ( BoneInfo* pChild : pBone->m_children )
    //        {
    //            const float HorizontalTreeLineSize = 4.0f;
    //            const ImRect childRect = RenderSkeletonTree( pChild );
    //            const float midpoint = ( childRect.Min.y + childRect.Max.y ) / 2.0f;
    //            drawList->AddLine( ImVec2( verticalLineStart.x, midpoint ), ImVec2( verticalLineStart.x + HorizontalTreeLineSize, midpoint ), TreeLineColor );
    //            verticalLineEnd.y = midpoint;
    //        }

    //        drawList->AddLine( verticalLineStart, verticalLineEnd, TreeLineColor );
    //        ImGui::TreePop();
    //    }

    //    return nodeRect;
    //}

    //void AnimationInfo::DrawSkeletalAnimationInfo( UpdateContext const& context )
    //{
    //    ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
    //    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 2, 4 ) );
    //    ImGui::BeginChild( "SkeletonView", ImVec2( 0, 0 ), true, ImGuiWindowFlags_AlwaysVerticalScrollbar );
    //    {
    //        KRG_ASSERT( m_pSkeletonTreeRoot != nullptr );
    //        RenderSkeletonTree( m_pSkeletonTreeRoot );
    //    }
    //    ImGui::EndChild();
    //    ImGui::PopStyleVar( 2 );
    //}
}