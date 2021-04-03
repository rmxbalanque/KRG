#include "MeshEditor.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    static void DisplayFile( DataDirectoryModel& model, DataDirectoryModel::File const& file )
    {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::TreeNodeEx( file.m_name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth );
        if ( ImGui::IsItemClicked() )
        {
            model.SetSelection( file.m_path );
        }

        ImGui::TableNextColumn();

        if ( file.m_resourceTypeID == StaticMesh::GetStaticResourceTypeID() )
        {
            ImGui::TextColored( Colors::LimeGreen.ToFloat4(), "Static Mesh" );
        }
        else if ( file.m_resourceTypeID == SkeletalMesh::GetStaticResourceTypeID() )
        {
            ImGui::TextColored( Colors::HotPink.ToFloat4(), "Skeletal Mesh" );
        }
        else
        {
            ImGui::TextColored( Colors::Gray.ToFloat4(), "Raw" );
        }
    };

    static void DisplayDirectory( DataDirectoryModel& model, DataDirectoryModel::Directory const& dir )
    {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::SetNextItemOpen( dir.m_isExpanded );
        dir.m_isExpanded = ImGui::TreeNodeEx( dir.m_name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth );
        if ( ImGui::IsItemClicked() )
        {
            model.SetSelection( dir.m_path );
        }

        ImGui::TableNextColumn();
        ImGui::Text( "-" );

        //-------------------------------------------------------------------------

        if ( dir.m_isExpanded )
        {
            for ( auto const& subDir : dir.m_directories )
            {
                if ( !subDir.IsHidden() )
                {
                    DisplayDirectory( model, subDir );
                }
            }

            for ( auto const& file : dir.m_files )
            {
                if ( !file.IsHidden() )
                {
                    DisplayFile( model, file );
                }
            }
            ImGui::TreePop();
        }
    }

    static void DisplayDataDirectory( DataDirectoryModel& model )
    {
        for ( auto const& subDir : model.GetRootDirectory().m_directories )
        {
            if ( !subDir.IsHidden() )
            {
                DisplayDirectory( model, subDir );
            }
        }

        for ( auto const& file : model.GetRootDirectory().m_files )
        {
            if ( !file.IsHidden() )
            {
                DisplayFile( model, file );
            }
        }
    }

    //-------------------------------------------------------------------------

    void MeshEditor::DrawMeshBrowserWindow()
    {
        ImGui::ShowDemoWindow();

        if ( ImGui::Begin( "Data Browser" ) )
        {
            // Filter
            //-------------------------------------------------------------------------

            char filterBuffer[255] = { 0 };
            ImGui::SetNextItemWidth( -1 );
            ImGui::InputText( "##Filter", filterBuffer, 255 );

            if ( ImGui::Checkbox( "Static Meshes", &m_showStaticMeshes ) )
            {
                auto func = [this] ( DataDirectoryModel::File const& file )
                {
                    if ( file.m_resourceTypeID == StaticMesh::GetStaticResourceTypeID() )
                    {
                        file.m_isHidden = !m_showStaticMeshes;
                    }
                };

                m_dataDirectoryModel.ForEachFile( func );
            }
            ImGui::SameLine();

            if ( ImGui::Checkbox( "Skeletal Meshes", &m_showSkeletalMeshes ) )
            {
                auto func = [this] ( DataDirectoryModel::File const& file )
                {
                    if ( file.m_resourceTypeID == SkeletalMesh::GetStaticResourceTypeID() )
                    {
                        file.m_isHidden = !m_showSkeletalMeshes;
                    }
                };

                m_dataDirectoryModel.ForEachFile( func );
            }
            ImGui::SameLine();

            if ( ImGui::Checkbox( "Raw Files", &m_showRawFiles ) )
            {
                auto func = [this] ( DataDirectoryModel::File const& file )
                {
                    if ( file.m_resourceTypeID != StaticMesh::GetStaticResourceTypeID() && file.m_resourceTypeID != SkeletalMesh::GetStaticResourceTypeID() )
                    {
                        file.m_isHidden = !m_showRawFiles;
                    }
                };

                m_dataDirectoryModel.ForEachFile( func );
            }

            ImGui::Separator();

            // Browser
            //-------------------------------------------------------------------------

            auto const& rootDir = m_dataDirectoryModel.GetRootDirectory();

            static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

            if ( ImGui::BeginTable( "Data Browser", 2, flags ) )
            {
                ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_NoHide );
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 100 );
                ImGui::TableHeadersRow();
                DisplayDataDirectory( m_dataDirectoryModel );
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}