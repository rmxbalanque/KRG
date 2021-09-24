#include "FbxFileInspector.h"
#include "System/Imgui/ImguiX.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "Tools/Render/ResourceCompilers/Mesh/StaticMeshCompiler.h"
#include "Tools/Render/ResourceCompilers/Mesh/SkeletalMeshCompiler.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "Tools/Animation/ResourceCompilers/AnimationSkeletonCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationClipCompiler.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_RAW_FILE_INSPECTOR_FACTORY( FbxInspectorFactory, "fbx", FbxFileInspector );

    //-------------------------------------------------------------------------

    FbxFileInspector::FbxFileInspector( EditorModel* pModel, FileSystem::Path const& filePath )
        : RawFileInspector( pModel, filePath )
    {
        KRG_ASSERT( filePath.IsExistingFile() );
        m_validAssetInfo = RawAssets::ReadFileInfo( filePath, m_assetInfo );
    }

    void FbxFileInspector::Draw()
    {
        ImGui::Text( "Raw File: %s", m_filePath.c_str() );

        if ( m_validAssetInfo )
        {
            ImGui::Text( "Original Up Axis: %s", Math::ToString( m_assetInfo.m_upAxis ) );
            ImGui::Text( "Scale: %.2f", m_assetInfo.m_scale );
            ImGui::Separator();

            // Asset Info
            //-------------------------------------------------------------------------

            ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 2, 4 ) );
            if ( ImGui::BeginTable( "Raw File Contents", 3, 0 ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 80 );
                ImGui::TableSetupColumn( "Mesh Name", ImGuiTableColumnFlags_NoHide );
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

            // Create new descriptor
            //-------------------------------------------------------------------------

            //float buttonWidth = ( ImGui::GetWindowContentRegionWidth() - 4 ) / 2;

            //ImColor const buttonColor = hasSkeletalMeshes ? ImGuiX::Theme::s_textColor : ImGuiX::Theme::s_textColorDisabled;
            //if ( ImGui::Button( KRG_ICON_MALE " Create Skeletal Mesh", ImVec2( buttonWidth, 24 ) ) )
            //{
            //    if ( hasSkeletalMeshes )
            //    {
            //        /*SkeletalMeshResourceDescriptor resourceDesc;
            //        resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
            //        CreateNewDescriptor( SkeletalMesh::GetStaticResourceTypeID(), resourceDesc );*/
            //    }
            //}

            //ImGui::SameLine( 0, 4 );

            //if ( ImGui::Button( KRG_ICON_CUBE " Create Static Mesh", ImVec2( buttonWidth, 24 ) ) )
            //{
            //    /*StaticMeshResourceDescriptor resourceDesc;
            //    resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
            //    CreateNewDescriptor( StaticMesh::GetStaticResourceTypeID(), resourceDesc );*/
            //}
        }
    }

    void FbxFileInspector::DrawStaticMeshControls( RawAssets::RawMeshInfo const& mesh )
    {
        if ( ImGui::Button( KRG_ICON_PLUS "##CreateStaticMesh", ImVec2( 19, 0 ) ) )
        {
            Render::StaticMeshResourceDescriptor resourceDesc;
            resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), m_filePath );
            resourceDesc.m_meshName = mesh.m_name;
            CreateNewDescriptor( Render::StaticMesh::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create static mesh resource for this sub-mesh" );
    }

    void FbxFileInspector::DrawSkeletalMeshControls( RawAssets::RawMeshInfo const& mesh )
    {
        KRG_ASSERT( mesh.m_isSkinned );

        if ( ImGui::Button( KRG_ICON_PLUS "##CreateSkeletalMesh", ImVec2( 20, 0 ) ) )
        {
            Render::SkeletalMeshResourceDescriptor resourceDesc;
            resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), m_filePath );
            resourceDesc.m_meshName = mesh.m_name;
            CreateNewDescriptor( Render::SkeletalMesh::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create skeletal mesh resource for this sub-mesh" );
    }

    void FbxFileInspector::DrawSkeletonControls( RawAssets::RawSkeletonInfo const& skeleton )
    {
        if ( ImGui::Button( KRG_ICON_PLUS "##CreateSkeleton", ImVec2( 19, 0 ) ) )
        {
            Animation::SkeletonResourceDescriptor resourceDesc;
            resourceDesc.m_skeletonDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), m_filePath );
            resourceDesc.m_skeletonRootBoneName = skeleton.m_name;
            CreateNewDescriptor( Animation::Skeleton::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create skeleton resource" );
    }

    void FbxFileInspector::DrawAnimationControls( RawAssets::RawAnimationInfo const& animation )
    {
        if ( ImGui::Button( KRG_ICON_PLUS "##CreateAnimation", ImVec2( 19, 0 ) ) )
        {
            Animation::AnimationClipResourceDescriptor resourceDesc;
            resourceDesc.m_animationDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), m_filePath );
            resourceDesc.m_animationName = animation.m_name;
            CreateNewDescriptor( Animation::AnimationClip::GetStaticResourceTypeID(), resourceDesc );
        }

        ImGuiX::ItemTooltip( "Create animation resource" );
    }
}