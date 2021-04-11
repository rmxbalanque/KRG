#include "MeshEditor_DataBrowserTool.h"
#include "Tools/Core/TypeSystem/Serialization/TypeInstanceModelReader.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    static void DisplayFile( DataBrowserModel& model, DataFileModel& file )
    {
        ImGui::TableNextRow();

        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();

        // Set node flags
        uint32 treeNodeflags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
        if ( model.GetSelection() == file.GetPath() )
        {
            treeNodeflags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx( file.GetName().c_str(), treeNodeflags );

        // Handle clicks on the tree node
        if ( ImGui::IsItemClicked() )
        {
            model.SetSelection( file.GetPath() );
        }

        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();

        if ( file.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
        {
            ImGui::TextColored( Colors::GreenYellow.ToFloat4(), "Static Mesh" );
        }
        else if ( file.GetResourceTypeID() == SkeletalMesh::GetStaticResourceTypeID() )
        {
            ImGui::TextColored( Colors::HotPink.ToFloat4(), "Skeletal Mesh" );
        }
        else
        {
            ImGui::TextColored( Colors::LightGray.ToFloat4(), "Raw" );
        }
    };

    static void DisplayDirectory( DataBrowserModel& model, DataDirectoryModel& dir )
    {
        ImGui::TableNextRow();

        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        ImGui::SetNextItemOpen( dir.IsExpanded() );

        // Set node flags
        uint32 treeNodeflags = ImGuiTreeNodeFlags_SpanFullWidth;
        if ( model.GetSelection() == dir.GetPath() )
        {
            treeNodeflags |= ImGuiTreeNodeFlags_Selected;
        }

        dir.SetExpanded( ImGui::TreeNodeEx( dir.GetName().c_str(), treeNodeflags ) );
        if ( ImGui::IsItemClicked() )
        {
            model.SetSelection( dir.GetPath() );
        }

        ImGui::TableNextColumn();
        ImGui::Text( "-" );

        //-------------------------------------------------------------------------

        if ( dir.IsExpanded() )
        {
            for ( auto& subDir : dir.GetDirectories() )
            {
                if ( subDir.IsVisible() )
                {
                    DisplayDirectory( model, subDir );
                }
            }

            for ( auto& file : dir.GetFiles() )
            {
                if ( file.IsVisible() )
                {
                    DisplayFile( model, file );
                }
            }
            ImGui::TreePop();
        }
    }

    static void DisplayDataDirectory( DataBrowserModel& model )
    {
        for ( auto& subDir : model.GetRootDirectory().GetDirectories() )
        {
            if ( subDir.IsVisible() )
            {
                DisplayDirectory( model, subDir );
            }
        }

        for ( auto& file : model.GetRootDirectory().GetFiles() )
        {
            if ( file.IsVisible() )
            {
                DisplayFile( model, file );
            }
        }
    }

    //-------------------------------------------------------------------------

    DataBrowser::DataBrowser( EditorModel* pModel )
        : TEditorTool<Model>( pModel )
        , m_propertyGrid( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
    {
        UpdateVisibility();
    }

    //-------------------------------------------------------------------------

    void DataBrowser::UpdateVisibility()
    {
        auto VisibilityFunc = [this] ( DataFileModel const& file )
        {
            bool isVisible = false;

            // Type filter
            //-------------------------------------------------------------------------

            if ( file.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
            {
                isVisible = m_showStaticMeshes;
            }
            else if ( file.GetResourceTypeID() == SkeletalMesh::GetStaticResourceTypeID() )
            {
                isVisible = m_showSkeletalMeshes;
            }
            else
            {
                isVisible = m_showRawFiles;
            }

            // Text filter
            //-------------------------------------------------------------------------

            if ( isVisible && m_filterBuffer[0] != 0 )
            {
                String lowerName = file.GetName();
                lowerName.make_lower();

                char tempBuffer[256];
                strcpy( tempBuffer, m_filterBuffer );

                char* token = strtok( tempBuffer, " " );
                while ( token )
                {
                    if ( lowerName.find( token ) == String::npos )
                    {
                        isVisible = false;
                        break;
                    }

                    token = strtok( NULL, " " );
                }
            }

            //-------------------------------------------------------------------------

            return isVisible;
        };

        //-------------------------------------------------------------------------

        m_model.GetDataBrowser().UpdateFileVisibility( VisibilityFunc );
    }

    //-------------------------------------------------------------------------

    void DataBrowser::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::ShowDemoWindow();
        DrawBrowser( context );
        DrawInfoPanel( context );
    }

    void DataBrowser::DrawBrowser( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Data Browser", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar ) )
        {
            // Text Filter
            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 31 );
            if ( ImGui::InputText( "##Filter", m_filterBuffer, 256 ) )
            {
                // Convert buffer to lower case
                int32 i = 0;
                while ( i < 256 && m_filterBuffer[i] != 0 )
                {
                    m_filterBuffer[i] = eastl::CharToLower( m_filterBuffer[i] );
                    i++;
                }

                UpdateVisibility();
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
            if ( ImGui::Button( KRG_ICON_TRASH "##Clear Filter" ) )
            {
                m_filterBuffer[0] = 0;
                UpdateVisibility();
            }

            // Type Filter + Controls
            //-------------------------------------------------------------------------

            if ( ImGui::Checkbox( "Static Meshes", &m_showStaticMeshes ) )
            {
                UpdateVisibility();
            }

            ImGui::SameLine();
            if ( ImGui::Checkbox( "Skeletal Meshes", &m_showSkeletalMeshes ) )
            {
                UpdateVisibility();
            }

            ImGui::SameLine();
            if ( ImGui::Checkbox( "Raw Files", &m_showRawFiles ) )
            {
                UpdateVisibility();
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 42 );
            if ( ImGui::Button( KRG_ICON_PLUS "##Expand All" ) )
            {
                m_model.GetDataBrowser().ForEachDirectory( [] ( DataDirectoryModel& dir ) { dir.SetExpanded( true ); } );
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
            if ( ImGui::Button( KRG_ICON_MINUS "##Collapse ALL" ) )
            {
                m_model.GetDataBrowser().ForEachDirectory( [] ( DataDirectoryModel& dir ) { dir.SetExpanded( false ); } );
            }

            //-------------------------------------------------------------------------

            ImGui::Separator();

            // Browser
            //-------------------------------------------------------------------------

            auto const& rootDir = m_model.GetDataBrowser().GetRootDirectory();

            static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Theme::s_accentColorDark );
            ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Theme::s_accentColorDark );
            if ( ImGui::BeginTable( "Data Browser", 2, flags ) )
            {
                ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_NoHide );
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 100 );
                ImGui::TableHeadersRow();
                DisplayDataDirectory( m_model.GetDataBrowser() );
                ImGui::EndTable();
            }
            ImGui::PopStyleColor( 2 );
        }
        ImGui::End();
    }

    void DataBrowser::DrawInfoPanel( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Data File Info" ) )
        {
            if ( m_model.GetDataBrowser().GetSelection() != m_inspectedFile )
            {
                m_inspectedFile = m_model.GetDataBrowser().GetSelection();
                if ( m_inspectedFile.IsValid() && m_inspectedFile.IsFilePath() )
                {
                    TypeSystem::TypeInstanceModelReader typeReader( *context.GetSystem<TypeSystem::TypeRegistry>() );
                    if ( typeReader.ReadFromFile( m_inspectedFile ) )
                    {
                        if ( typeReader.DeserializeType( m_typeInstance ) )
                        {
                            m_propertyGrid.SetTypeToEdit( &m_typeInstance );
                        }
                        else
                        {
                            m_propertyGrid.SetTypeToEdit( nullptr );
                        }
                    }
                    else
                    {
                        ImGui::Text( "ERROR" );
                    }
                }
            }

            //-------------------------------------------------------------------------

            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }
}