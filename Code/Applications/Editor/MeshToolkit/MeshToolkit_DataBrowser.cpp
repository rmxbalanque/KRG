#include "MeshToolkit_DataBrowser.h"
#include "FileTypes/MeshResourceFiles.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Render/ResourceCompilers/Mesh/StaticMeshCompiler.h"
#include "Tools/Render/ResourceCompilers/Mesh/SkeletalMeshCompiler.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"
#include "Tools/Core/TypeSystem/Serialization/TypeReader.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    MeshResourceDescriptorCreator::MeshResourceDescriptorCreator( EditorModel* pModel )
        : ResourceDescriptorCreator( pModel )
    {
        m_decriptorTypes.push_back( { "Static Mesh", StaticMesh::GetStaticResourceTypeID() } );
        m_decriptorTypes.push_back( { "Skeletal Mesh", SkeletalMesh::GetStaticResourceTypeID() } );
    }

    void MeshResourceDescriptorCreator::CreateNewDescriptor( FileSystem::Path const& rawFile ) const
    {
        int32 const selectedTypeIdx = GetSelectedTypeIndex();
        KRG_ASSERT( selectedTypeIdx < m_decriptorTypes.size() );

        TypeSystem::Serialization::TypeWriter typeWriter( m_pModel->GetTypeRegistry() );
        if ( selectedTypeIdx == 0 )
        {
            StaticMeshResourceDescriptor desc;
            desc.m_meshDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), rawFile );
            typeWriter << desc;
        }
        else
        {
            SkeletalMeshResourceDescriptor desc;
            desc.m_meshDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), rawFile );
            typeWriter << desc;
        }
      
        typeWriter.WriteToFile( CreateDescriptorFilePath( rawFile ) );
    }
}

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    static Color const g_staticMeshColor = Colors::GreenYellow;
    static Color const g_skeletalMeshColor = Colors::HotPink;
    static Color const g_rawFileColor = Colors::LightGray;

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
        , m_propertyGrid( pModel )
        , m_descriptorCreator( pModel )
        , m_dataBrowserTreeView( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory(), { ".msh", ".smsh", ".fbx", ".gltf" } )
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
            if ( pSelectedItem->GetPath() != m_inspectedFile )
            {
                m_inspectedFile = pSelectedItem->GetPath();

                if ( pSelectedItem->IsRawFile() )
                {
                    if ( RawAssets::ReadFileInfo( m_inspectedFile, m_assetInfo ) )
                    {
                        m_validAssetInfo = true;
                    }
                    else
                    {
                        m_validAssetInfo = false;
                        KRG_LOG_ERROR( "DataBrowser", "Failed to read raw resource file: %s", m_inspectedFile.c_str() );
                    }
                }
                else // Resource File
                {
                    /*if ( OpenInspectedResourceFile( context ) )
                    {
                        m_propertyGrid.SetTypeToEdit( &m_typeInstance );
                    }
                    else
                    {
                        m_propertyGrid.SetTypeToEdit( nullptr );
                        KRG_LOG_ERROR( "DataBrowser", "Failed to deserialize resource file: %s", m_inspectedFile.c_str() );
                    }*/
                }
            }
        }
        else // Clear inspected file
        {
            m_inspectedFile = FileSystem::Path();
            m_assetInfo.Reset();
        }

        //-------------------------------------------------------------------------

        DrawInfoPanel( context );
    }

    //-------------------------------------------------------------------------
    // Browser
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
            m_model.OpenResourceFile<StaticMeshResourceFile>( resourceID );
        }
        else if ( pDataFileItem->IsResourceOfType<SkeletalMesh>() )
        {
            ResourceID const resourceID( pDataFileItem->GetDataPath() );
            m_model.OpenResourceFile<SkeletalMeshResourceFile>( resourceID );
        }
    }

    bool DataBrowser::OpenInspectedResourceFile( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedFile.ExistsAndIsFile() );

        TypeSystem::Serialization::TypeReader typeReader( *context.GetSystem<TypeSystem::TypeRegistry>() );
        if ( typeReader.ReadFromFile( m_inspectedFile ) )
        {
            return typeReader.ReadType( m_typeInstance );
        }

        return false;
    }

    bool DataBrowser::SaveInspectedResourceFile( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedFile.ExistsAndIsFile() && m_typeInstance.IsValid() );

        TypeSystem::Serialization::TypeWriter typeWriter( *context.GetSystem<TypeSystem::TypeRegistry>() );
        typeWriter << m_typeInstance;
        return typeWriter.WriteToFile( m_inspectedFile );
    }

    //-------------------------------------------------------------------------
    // Info Panel
    //-------------------------------------------------------------------------

    void DataBrowser::DrawInfoPanel( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Data File Info" ) )
        {
            if ( m_inspectedFile.IsValid() )
            {
                auto pSelectedItem = m_dataBrowserTreeView.GetSelectedItem<MeshBrowserTreeItem>();
                KRG_ASSERT( pSelectedItem != nullptr && pSelectedItem->IsFile() );
                if ( pSelectedItem->IsRawFile() )
                {
                    DrawRawFileInfo( context );
                }
                else if ( m_typeInstance.IsValid() )
                {
                    DrawResourceFileInfo( context );
                }
                else
                {
                    // Do Nothing
                }
            }
        }
        ImGui::End();
    }

    void DataBrowser::DrawResourceFileInfo( UpdateContext const& context )
    {
        KRG_ASSERT( m_typeInstance.IsValid() && m_inspectedFile.IsFilePath() );

        //-------------------------------------------------------------------------

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 80;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;
        float const buttonStartPosX = textAreaWidth + itemSpacing;

        ImGui::AlignTextToFramePadding();
        ImGui::Text( m_inspectedFile.GetFileName().c_str() );
        float const actualTextWidth = ImGui::GetItemRectSize().x;

        ImGui::SameLine( 0, textAreaWidth - actualTextWidth + itemSpacing );
        if ( m_propertyGrid.IsDirty() )
        {
            if ( ImGuiX::ButtonColored( Colors::LimeGreen.ToFloat4(), KRG_ICON_FLOPPY_O " Save", ImVec2( buttonAreaWidth, 0 ) ) )
            {
                SaveInspectedResourceFile( context );
                m_propertyGrid.ClearDirty();
            }
        }
        else
        {
            ImGuiX::ButtonColored( ImGuiX::Theme::s_textColorDisabled, KRG_ICON_FLOPPY_O " Save", ImVec2( buttonAreaWidth, 0 ) );
        }

        //-------------------------------------------------------------------------

        m_propertyGrid.DrawGrid();
    }

    void DataBrowser::DrawRawFileInfo( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedFile.IsFilePath() );

        ImGui::Text( "Raw File: %s", m_inspectedFile.c_str() );
        ImGui::Separator();

        if ( m_validAssetInfo )
        {
            // Asset Info
            //-------------------------------------------------------------------------

            ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 4 ) );
            if ( ImGui::BeginTable( "Mesh Info", 2, 0 ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 80 );
                ImGui::TableSetupColumn( "Mesh Name", ImGuiTableColumnFlags_NoHide );

                for ( auto const& mesh : m_assetInfo.m_meshes )
                {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    if ( mesh.m_isSkinned )
                    {
                        ImGui::TextColored( g_skeletalMeshColor.ToFloat4(), "Skeletal Mesh:" );

                    }
                    else // Static
                    {
                        ImGui::TextColored( g_staticMeshColor.ToFloat4(), "Static Mesh:" );
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGuiX::SelectableText( mesh.m_name, -1 );
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            // Create new descriptor
            //-------------------------------------------------------------------------

            if ( ImGui::Button( "Create New Descriptor", ImVec2( -1, 0 ) ) )
            {
                ImGui::OpenPopup( "Create New Descriptor##Modal" );
            }

            if ( ImGui::BeginPopupModal( "Create New Descriptor##Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                if ( m_descriptorCreator.ShowCreatorDialog( m_inspectedFile ) )
                {
                    ImGui::CloseCurrentPopup();
                }

                //-------------------------------------------------------------------------

                ImGui::EndPopup();
            }
        }
    }
}