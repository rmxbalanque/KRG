#include "RawFileInspector_MeshFormats.h"
#include "System/Render/Imgui/ImguiX.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderMesh.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationClip.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryFbx, "fbx", FileInspectorMeshFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryGltf, "gltf", FileInspectorMeshFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryGlb, "glb", FileInspectorMeshFormats );

    //-------------------------------------------------------------------------

    FileInspectorMeshFormats::FileInspectorMeshFormats( EditorModel* pModel, FileSystem::Path const& filePath )
        : RawFileInspector( pModel, filePath )
    {
        KRG_ASSERT( FileSystem::Exists( filePath ) );
        m_validAssetInfo = RawAssets::ReadFileInfo( filePath, m_assetInfo );
    }

    void FileInspectorMeshFormats::Draw()
    {
        ImGui::Text( "Raw File: %s", m_filePath.c_str() );

        if ( m_validAssetInfo )
        {
            ImGui::Text( "Original Up Axis: %s", Math::ToString( m_assetInfo.m_upAxis ) );
            ImGui::Text( "Scale: %.2f", m_assetInfo.m_scale );

            if ( ImGui::Button( KRG_ICON_CUBE " Create Static Mesh", ImVec2( -1, 24 ) ) )
            {
                Render::StaticMeshResourceDescriptor resourceDesc;
                resourceDesc.m_meshPath = ResourcePath::FromFileSystemPath( m_pModel->GetSourceResourceDirectory(), m_filePath );
                CreateNewDescriptor( Render::StaticMesh::GetStaticResourceTypeID(), resourceDesc );
            }

            ImGui::Separator();

            // Asset Info
            //-------------------------------------------------------------------------

            ImGui::BeginChild( "FileContents" );

            ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 2, 4 ) );
            if ( ImGui::BeginTable( "Raw File Contents", 3, 0 ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 80 );
                ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_NoHide );
                ImGui::TableSetupColumn( "Controls", ImGuiTableColumnFlags_WidthFixed, 20 );

                // Static Meshes
                //-------------------------------------------------------------------------

                for ( auto const& mesh : m_assetInfo.m_meshes )
                {
                    ImGui::TableNextRow();
                    ImGui::PushID( &mesh );

                    // Type
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Static Mesh" );

                    // Name
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::PushID( "StaticMesh" );
                    ImGuiX::SelectableText( mesh.m_name, -1 );
                    ImGui::PopID();

                    // Options
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    DrawStaticMeshControls( mesh );

                    ImGui::PopID();
                }

                // Skeletal Meshes
                //-------------------------------------------------------------------------

                for ( auto const& mesh : m_assetInfo.m_meshes )
                {
                    if ( !mesh.m_isSkinned )
                    {
                        continue;
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableNextRow();
                    ImGui::PushID( &mesh );

                    // Type
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Skeletal Mesh" );

                    // Name
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::PushID( "SkeletalMesh" );
                    ImGuiX::SelectableText( mesh.m_name, -1 );
                    ImGui::PopID();

                    // Options
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    DrawSkeletalMeshControls( mesh );

                    ImGui::PopID();
                }

                // Skeletons
                //-------------------------------------------------------------------------

                for ( auto const& skeleton : m_assetInfo.m_skeletons )
                {
                    ImGui::TableNextRow();
                    ImGui::PushID( &skeleton );

                    // Type
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Skeleton" );

                    // Name
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGuiX::SelectableText( skeleton.m_name, -1 );

                    // Options
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    DrawSkeletonControls( skeleton );

                    ImGui::PopID();
                }

                // Animations
                //-------------------------------------------------------------------------

                for ( auto const& animation : m_assetInfo.m_animations )
                {
                    ImGui::TableNextRow();
                    ImGui::PushID( &animation );

                    // Type
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Animation" );

                    // Name
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGuiX::SelectableText( animation.m_name, -1 );

                    // Options
                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    DrawAnimationControls( animation );

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            ImGui::EndChild();
        }
    }

    void FileInspectorMeshFormats::DrawStaticMeshControls( RawAssets::RawMeshInfo const& mesh )
    {
        if ( ImGui::Button( KRG_ICON_PLUS "##CreateStaticMesh", ImVec2( 19, 0 ) ) )
        {
            Render::StaticMeshResourceDescriptor resourceDesc;
            resourceDesc.m_meshPath = ResourcePath::FromFileSystemPath( m_pModel->GetSourceResourceDirectory(), m_filePath );
            resourceDesc.m_meshName = mesh.m_name;
            CreateNewDescriptor( Render::StaticMesh::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create static mesh resource for this sub-mesh" );
    }

    void FileInspectorMeshFormats::DrawSkeletalMeshControls( RawAssets::RawMeshInfo const& mesh )
    {
        KRG_ASSERT( mesh.m_isSkinned );

        if ( ImGui::Button( KRG_ICON_PLUS "##CreateSkeletalMesh", ImVec2( 20, 0 ) ) )
        {
            Render::SkeletalMeshResourceDescriptor resourceDesc;
            resourceDesc.m_meshPath = ResourcePath::FromFileSystemPath( m_pModel->GetSourceResourceDirectory(), m_filePath );
            resourceDesc.m_meshName = mesh.m_name;
            CreateNewDescriptor( Render::SkeletalMesh::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create skeletal mesh resource for this sub-mesh" );
    }

    void FileInspectorMeshFormats::DrawSkeletonControls( RawAssets::RawSkeletonInfo const& skeleton )
    {
        if ( ImGui::Button( KRG_ICON_PLUS "##CreateSkeleton", ImVec2( 19, 0 ) ) )
        {
            Animation::SkeletonResourceDescriptor resourceDesc;
            resourceDesc.m_skeletonPath = ResourcePath::FromFileSystemPath( m_pModel->GetSourceResourceDirectory(), m_filePath );
            resourceDesc.m_skeletonRootBoneName = skeleton.m_name;
            CreateNewDescriptor( Animation::Skeleton::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create skeleton resource" );
    }

    void FileInspectorMeshFormats::DrawAnimationControls( RawAssets::RawAnimationInfo const& animation )
    {
        if ( ImGui::Button( KRG_ICON_PLUS "##CreateAnimation", ImVec2( 19, 0 ) ) )
        {
            Animation::AnimationClipResourceDescriptor resourceDesc;
            resourceDesc.m_animationPath = ResourcePath::FromFileSystemPath( m_pModel->GetSourceResourceDirectory(), m_filePath );
            resourceDesc.m_animationName = animation.m_name;
            CreateNewDescriptor( Animation::AnimationClip::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create animation resource" );
    }
}