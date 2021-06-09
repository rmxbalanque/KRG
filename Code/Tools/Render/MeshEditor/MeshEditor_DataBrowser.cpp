#include "MeshEditor_DataBrowser.h"
#include "FileTabs/MeshResourceFileTabs.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Render/ResourceCompilers/Mesh/StaticMeshCompiler.h"
#include "Tools/Render/ResourceCompilers/Mesh/SkeletalMeshCompiler.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    static Color const g_staticMeshColor = Colors::GreenYellow;
    static Color const g_skeletalMeshColor = Colors::HotPink;
    static Color const g_rawFileColor = Colors::LightGray;

    //-------------------------------------------------------------------------

    void MeshDataFileInspector::OnStartInspectingRawFile()
    {
        if ( RawAssets::ReadFileInfo( m_inspectedFile, m_assetInfo ) )
        {
            m_validAssetInfo = true;
        }
        else
        {
            m_validAssetInfo = false;
            KRG_LOG_ERROR( "Resource", "Failed to read raw resource file: %s", m_inspectedFile.c_str() );
        }
    }

    void MeshDataFileInspector::OnStopInspectingRawFile()
    {
        m_assetInfo.Reset();
        m_validAssetInfo = false;
    }

    void MeshDataFileInspector::DrawRawFileInfo()
    {
        KRG_ASSERT( m_inspectedFile.IsFilePath() );

        ImGui::Text( "Raw File: %s", m_inspectedFile.c_str() );
        ImGui::Separator();

        if ( m_validAssetInfo )
        {
            // Asset Info
            //-------------------------------------------------------------------------

            bool hasSkeletalMeshes = false;

            ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 2, 4 ) );
            if ( ImGui::BeginTable( "Raw File Contents", 3, 0 ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 80 );
                ImGui::TableSetupColumn( "Mesh Name", ImGuiTableColumnFlags_NoHide );
                ImGui::TableSetupColumn( "Controls", ImGuiTableColumnFlags_WidthFixed, 46 );

                for ( auto const& mesh : m_assetInfo.m_meshes )
                {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    if ( mesh.m_isSkinned )
                    {
                        ImGui::TextColored( g_skeletalMeshColor.ToFloat4(), "Skeletal Mesh:" );
                        hasSkeletalMeshes = true;
                    }
                    else // Static
                    {
                        ImGui::TextColored( g_staticMeshColor.ToFloat4(), "Static Mesh:" );
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGuiX::SelectableText( mesh.m_name, -1 );

                    ImGui::TableNextColumn();

                    ImGui::PushID( &mesh );

                    bool const canCreateSkeletalMesh = mesh.m_isSkinned;
                    ImColor const buttonColor = canCreateSkeletalMesh ? ImGuiX::Theme::s_textColor : ImGuiX::Theme::s_textColorDisabled;
                    if ( ImGuiX::ButtonColored( buttonColor, KRG_ICON_MALE "##CreateSkeletalMesh", ImVec2( 20, 0 ) ) )
                    {
                        if ( canCreateSkeletalMesh )
                        {
                            SkeletalMeshResourceDescriptor resourceDesc;
                            resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                            resourceDesc.m_meshName = mesh.m_name;
                            CreateNewDescriptor( SkeletalMesh::GetStaticResourceTypeID(), resourceDesc );
                        }
                    }

                    if( canCreateSkeletalMesh )
                    {
                        ImGuiX::ItemTooltip( "Create skeletal mesh resource for this sub-mesh" );
                    }

                    ImGui::SameLine( 0, 4 );
   
                    if ( ImGui::Button( KRG_ICON_CUBE "##CreateStaticMesh", ImVec2( 20, 0 ) ) )
                    {
                        StaticMeshResourceDescriptor resourceDesc;
                        resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                        resourceDesc.m_meshName = mesh.m_name;
                        CreateNewDescriptor( StaticMesh::GetStaticResourceTypeID(), resourceDesc );
                    }

                    ImGuiX::ItemTooltip( "Create static mesh resource for this sub-mesh" );

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            // Create new descriptor
            //-------------------------------------------------------------------------

            float buttonWidth = ( ImGui::GetWindowContentRegionWidth() - 4 ) / 2;

            ImColor const buttonColor = hasSkeletalMeshes ? ImGuiX::Theme::s_textColor : ImGuiX::Theme::s_textColorDisabled;
            if ( ImGui::Button( KRG_ICON_MALE " Create Skeletal Mesh", ImVec2( buttonWidth, 24 ) ) )
            {
                if ( hasSkeletalMeshes )
                {
                    SkeletalMeshResourceDescriptor resourceDesc;
                    resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                    CreateNewDescriptor( SkeletalMesh::GetStaticResourceTypeID(), resourceDesc );
                }
            }

            ImGui::SameLine( 0, 4 );

            if ( ImGui::Button( KRG_ICON_CUBE " Create Static Mesh", ImVec2( buttonWidth, 24 ) ) )
            {
                StaticMeshResourceDescriptor resourceDesc;
                resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                CreateNewDescriptor( StaticMesh::GetStaticResourceTypeID(), resourceDesc );
            }
        }
    }

    //-------------------------------------------------------------------------

    void DataBrowser::MeshBrowserTreeItem::DrawControls()
    {
        if ( IsDirectory() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( IsRawFile() )
        {
            ImGui::TextColored( g_rawFileColor.ToFloat4(), "Raw" );
        }
        else if ( IsResourceOfType<SkeletalMesh>() )
        {
            ImGui::TextColored( g_skeletalMeshColor.ToFloat4(), "Skeletal Mesh" );
        }
        else if ( IsResourceOfType<StaticMesh>() )
        {
            ImGui::TextColored( g_staticMeshColor.ToFloat4(), "Static Mesh" );
        }
    }

    //-------------------------------------------------------------------------

    DataBrowser::DataBrowser( EditorModel* pModel )
        : TEditorTool<EditorModel>( pModel )
        , m_dataBrowserTreeView( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory(), { ".msh", ".smsh", ".fbx", ".gltf" } )
        , m_dataFileInspector( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
    {
        m_onDoubleClickEventID = m_dataBrowserTreeView.OnItemDoubleClicked().Bind( [this] ( TreeViewItem* pItem ) { OnBrowserItemDoubleClicked( pItem ); } );
        m_dataBrowserTreeView.RebuildBrowserTree();
        UpdateVisibility();
    }

    DataBrowser::~DataBrowser()
    {
        m_dataBrowserTreeView.OnItemDoubleClicked().Unbind( m_onDoubleClickEventID );
    }

    //-------------------------------------------------------------------------

    void DataBrowser::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_dataBrowserTreeView.Update( context );

        //-------------------------------------------------------------------------

        DrawBrowser( context );

        //-------------------------------------------------------------------------

        auto pSelectedItem = m_dataBrowserTreeView.GetSelectedItem<MeshBrowserTreeItem>();
        if ( pSelectedItem != nullptr && pSelectedItem->IsFile() )
        {
            if ( pSelectedItem->GetPath() != m_dataFileInspector.GetInspectedFilePath() )
            {
                m_dataFileInspector.SetFileToInspect( pSelectedItem->GetPath() );
            }
        }
        else // Clear inspected file
        {
            m_dataFileInspector.ClearFileToInspect();
        }

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "Data File Info" ) )
        {
            m_dataFileInspector.Draw();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void DataBrowser::UpdateVisibility()
    {
        auto VisibilityFunc = [this] ( TreeViewItem const* pItem )
        {
            bool isVisible = true;

            // Type filter
            //-------------------------------------------------------------------------

            auto pDataFileItem = static_cast<DataBrowserTreeItem const*>( pItem );
            if ( pDataFileItem->IsFile() )
            {
                if ( pDataFileItem->IsResourceOfType<StaticMesh>() )
                {
                    isVisible = m_showStaticMeshes;
                }
                else if ( pDataFileItem->IsResourceOfType<SkeletalMesh>() )
                {
                    isVisible = m_showSkeletalMeshes;
                }
                else
                {
                    isVisible = m_showRawFiles;
                }
            }

            // Text filter
            //-------------------------------------------------------------------------

            if ( isVisible && m_filterBuffer[0] != 0 )
            {
                String lowercaseLabel = pItem->GetLabel();
                lowercaseLabel.make_lower();

                char tempBuffer[256];
                strcpy( tempBuffer, m_filterBuffer );

                char* token = strtok( tempBuffer, " " );
                while ( token )
                {
                    if ( lowercaseLabel.find( token ) == String::npos )
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

        m_dataBrowserTreeView.UpdateItemVisibility( VisibilityFunc );
    }

    void DataBrowser::DrawBrowser( UpdateContext const& context )
    {
        KRG_PROFILE_FUNCTION();

        if ( ImGui::Begin( "Data Browser", nullptr ) )
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

                auto const SetExpansion = []( TreeViewItem* pItem )
                {
                    if ( pItem->IsVisible() )
                    {
                        pItem->SetExpanded( true );
                    }
                };

                m_dataBrowserTreeView.ForEachItem( SetExpansion );
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
            if ( ImGui::Button( KRG_ICON_ERASER "##Clear Filter" ) )
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
                m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( true ); } );
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
            if ( ImGui::Button( KRG_ICON_MINUS "##Collapse ALL" ) )
            {
                m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( false ); } );
            }

            //-------------------------------------------------------------------------

            ImGui::Separator();

            // Browser
            //-------------------------------------------------------------------------

            m_dataBrowserTreeView.Draw();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void DataBrowser::OnBrowserItemDoubleClicked( TreeViewItem* pItem )
    {
        auto pDataFileItem = static_cast<DataBrowserTreeItem const*>( pItem );
        if ( pDataFileItem->IsDirectory() || pDataFileItem->IsRawFile() )
        {
            return;
        }

        if ( pDataFileItem->IsResourceOfType<StaticMesh>() )
        {
            ResourceID const resourceID( pDataFileItem->GetDataPath() );
            m_model.OpenResourceFile<StaticMeshFileTab>( resourceID );
        }
        else if ( pDataFileItem->IsResourceOfType<SkeletalMesh>() )
        {
            ResourceID const resourceID( pDataFileItem->GetDataPath() );
            m_model.OpenResourceFile<SkeletalMeshFileTab>( resourceID );
        }
    }
}