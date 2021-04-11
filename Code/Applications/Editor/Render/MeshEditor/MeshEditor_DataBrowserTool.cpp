#include "MeshEditor_DataBrowserTool.h"
#include "Tools/Core/TypeSystem/Serialization/TypeInstanceModelReader.h"
#include "Tools/Core/TypeSystem/Serialization/TypeInstanceModelWriter.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Logging/Log.h"
#include "Compilers/Render/Mesh/StaticMeshCompiler.h"
#include "Compilers/Render/Mesh/SkeletalMeshCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
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

        TypeSystem::NativeTypeWriter typeWriter( m_pModel->GetTypeRegistry() );
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

namespace KRG::Render::MeshEditor
{
    static Color const g_staticMeshColor = Colors::GreenYellow;
    static Color const g_skeletalMeshColor = Colors::HotPink;
    static Color const g_rawFileColor = Colors::LightGray;

    //-------------------------------------------------------------------------

    static void DisplayBrowserFile( DataBrowserModel& model, DataFileModel& file )
    {
        ImGui::TableNextRow();

        // Filename
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
        if ( ImGui::IsItemFocused() || ImGui::IsItemClicked( ImGuiMouseButton_Left ) || ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
        {
            ImGui::SetItemDefaultFocus();
            model.SetSelection( file.GetPath() );
        }

        // Context menu
        //-------------------------------------------------------------------------

        ImGui::PushID( &file );
        if ( ImGui::BeginPopupContextItem( "FileContextMenu" ) )
        {
            model.SetSelection( file.GetPath() );

            if ( ImGui::Selectable( "Open In Explorer" ) )
            {
                FileSystem::OpenFileInExplorer( file.GetPath() );
            }

            if ( ImGui::Selectable( "Copy File Path" ) )
            {
                ImGui::SetClipboardText( file.GetPath().c_str() );
            }

            if ( ImGui::Selectable( "Copy Data Path" ) )
            {
                DataPath dataPath = DataPath::FromFileSystemPath( model.GetSourceDataDirectoryPath(), file.GetPath() );
                ImGui::SetClipboardText( dataPath.c_str() );
            }

            ImGui::EndPopup();
        }
        ImGui::PopID();

        // File Type
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();

        if ( file.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
        {
            ImGui::TextColored( g_staticMeshColor.ToFloat4(), "Static Mesh" );
        }
        else if ( file.GetResourceTypeID() == SkeletalMesh::GetStaticResourceTypeID() )
        {
            ImGui::TextColored( g_skeletalMeshColor.ToFloat4(), "Skeletal Mesh" );
        }
        else
        {
            ImGui::TextColored( g_rawFileColor.ToFloat4(), "Raw" );
        }
    };

    static void DisplayBrowserDirectory( DataBrowserModel& model, DataDirectoryModel& dir )
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
        if ( ImGui::IsItemFocused() || ImGui::IsItemClicked() )
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
                    DisplayBrowserDirectory( model, subDir );
                }
            }

            for ( auto& file : dir.GetFiles() )
            {
                if ( file.IsVisible() )
                {
                    DisplayBrowserFile( model, file );
                }
            }
            ImGui::TreePop();
        }
    }

    static void DisplayBrowserDataDirectory( DataBrowserModel& model )
    {
        for ( auto& subDir : model.GetRootDirectory().GetDirectories() )
        {
            if ( subDir.IsVisible() )
            {
                DisplayBrowserDirectory( model, subDir );
            }
        }

        for ( auto& file : model.GetRootDirectory().GetFiles() )
        {
            if ( file.IsVisible() )
            {
                DisplayBrowserFile( model, file );
            }
        }
    }

    //-------------------------------------------------------------------------

    DataBrowser::DataBrowser( EditorModel* pModel )
        : TEditorTool<Model>( pModel )
        , m_propertyGrid( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
        , m_descriptorCreator( pModel )
    {
        UpdateVisibility();
    }

    //-------------------------------------------------------------------------

    void DataBrowser::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_model.GetDataBrowser().GetSelection() != m_inspectedPath )
        {
            m_inspectedPath = m_model.GetDataBrowser().GetSelection();
            if ( m_inspectedPath.IsValid() && m_inspectedPath.IsFilePath() )
            {
                if ( IsInspectedFileARawFile() )
                {
                    if ( RawAssets::ReadFileInfo( m_inspectedPath, m_assetInfo ) )
                    {
                        m_validAssetInfo = true;
                    }
                    else
                    {
                        m_validAssetInfo = false;
                        KRG_LOG_ERROR( "DataBrowser", "Failed to read raw resource file: %s", m_inspectedPath.c_str() );
                    }
                }
                else // Resource File
                { 
                    if ( OpenInspectedResourceFile( context ) )
                    {
                        m_propertyGrid.SetTypeToEdit( &m_typeInstance );
                    }
                    else
                    {
                        m_propertyGrid.SetTypeToEdit( nullptr );
                        KRG_LOG_ERROR( "DataBrowser", "Failed to deserialize resource file: %s", m_inspectedPath.c_str() );
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        ImGui::ShowDemoWindow();
        DrawBrowser( context );
        DrawInfoPanel( context );
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

    void DataBrowser::DrawBrowser( UpdateContext const& context )
    {
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

            ImGui::BeginChild( "BrowserWindow", ImVec2( 0, 0 ), false, ImGuiWindowFlags_AlwaysVerticalScrollbar );
            {
                auto const& rootDir = m_model.GetDataBrowser().GetRootDirectory();

                ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Theme::s_accentColorDark );
                ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Theme::s_accentColorDark );
                static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
                if ( ImGui::BeginTable( "Data Browser", 2, flags, ImVec2( ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x / 2, 0 ) ) )
                {
                    ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_NoHide );
                    ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 100 );
                    ImGui::TableHeadersRow();
                    DisplayBrowserDataDirectory( m_model.GetDataBrowser() );
                    ImGui::EndTable();
                }
                ImGui::PopStyleColor( 2 );
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    bool DataBrowser::IsInspectedFileARawFile() const
    {
        auto const extension = m_inspectedPath.GetExtensionAsString();
        if ( extension == "gltf" || extension == "fbx" )
        {
            return true;
        }
        
        return false;
    }

    bool DataBrowser::OpenInspectedResourceFile( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedPath.ExistsAndIsFile() );

        TypeSystem::TypeInstanceModelReader typeReader( *context.GetSystem<TypeSystem::TypeRegistry>() );
        if ( typeReader.ReadFromFile( m_inspectedPath ) )
        {
            return typeReader.DeserializeType( m_typeInstance );
        }

        return false;
    }

    bool DataBrowser::SaveInspectedResourceFile( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedPath.ExistsAndIsFile() && m_typeInstance.IsValid() );

        TypeSystem::TypeInstanceModelWriter typeWriter( *context.GetSystem<TypeSystem::TypeRegistry>() );
        typeWriter.SerializeType( m_typeInstance );
        return typeWriter.WriteToFile( m_inspectedPath );
    }

    //-------------------------------------------------------------------------

    void DataBrowser::DrawInfoPanel( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Data File Info" ) )
        {
            if ( !m_inspectedPath.IsValid() )
            {
                // Do Nothing
            }
            else if ( m_inspectedPath.IsDirectoryPath() )
            {
                ImGui::Text( "Directory: %s", m_inspectedPath.c_str() );
            }
            else // File
            {
                if ( IsInspectedFileARawFile() )
                {
                    DrawRawFileInfo( context );
                }
                else if( m_typeInstance.IsValid() )
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
        KRG_ASSERT( m_typeInstance.IsValid() && m_inspectedPath.IsFilePath() );

        //-------------------------------------------------------------------------

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 80;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;
        float const buttonStartPosX = textAreaWidth + itemSpacing;

        ImGui::AlignTextToFramePadding();
        ImGui::Text( m_inspectedPath.GetFileName().c_str() );
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
        ImGui::Text( "Raw File: %s", m_inspectedPath.c_str() );
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
                if ( m_descriptorCreator.ShowCreatorDialog( m_inspectedPath ) )
                {
                    ImGui::CloseCurrentPopup();
                }

                //-------------------------------------------------------------------------

                ImGui::EndPopup();
            }
        }
    }
}