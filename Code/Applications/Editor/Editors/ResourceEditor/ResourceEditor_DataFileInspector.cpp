#include "ResourceEditor_DataFileInspector.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Core/Thirdparty/pfd/portable-file-dialogs.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"
#include "Tools/Core/TypeSystem/Serialization/TypeReader.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    DataFileInspector::DataFileInspector( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath )
        : m_typeRegistry( typeRegistry )
        , m_sourceDataPath( sourceDataPath )
        , m_propertyGrid( typeRegistry, sourceDataPath )
    {
        KRG_ASSERT( sourceDataPath.ExistsAndIsDirectory() );
    }

    DataFileInspector::~DataFileInspector()
    {
        ClearFileToInspect();
        KRG_ASSERT( m_pDescriptor == nullptr );
    }

    void DataFileInspector::SetFileToInspect( FileSystem::Path const& inFile )
    {
        KRG_ASSERT( inFile.IsValid() );

        // Clean up existing state
        //-------------------------------------------------------------------------

        ClearFileToInspect();

        // Check if the file is a raw file or a resource file
        //-------------------------------------------------------------------------

        m_inspectedFile = inFile;

        m_descriptorID = ResourceID( DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile ) );
        if ( m_descriptorID.IsValid() )
        {
            // Ensure the resource type ID is a registered resource type
            if ( !m_typeRegistry.IsRegisteredResourceType( m_descriptorID.GetResourceTypeID() ) )
            {
                m_descriptorID = ResourceID();
            }
        }

        // Start inspecting
        //-------------------------------------------------------------------------

        if ( m_descriptorID.IsValid() )
        {
            m_mode = Mode::InspectingResourceFile;
            m_isDirty = false;
            m_descriptorPath = m_descriptorID.GetDataPath().ToFileSystemPath( m_sourceDataPath );

            if ( LoadResourceDescriptor() )
            {
                m_propertyGrid.SetTypeToEdit( m_pDescriptor );
            }
        }
        else
        {
            m_mode = Mode::InspectingRawFile;
            OnStartInspectingRawFile();
        }
    }

    void DataFileInspector::ClearFileToInspect()
    {
        if ( m_mode == Mode::InspectingResourceFile )
        {
            if ( m_descriptorID.IsValid() )
            {
                m_propertyGrid.SetTypeToEdit( nullptr );
                m_descriptorID = ResourceID();
                m_descriptorPath = FileSystem::Path();
                m_isDirty = false;
                KRG::Delete( m_pDescriptor );
            }
        }
        else if ( m_mode == Mode::InspectingRawFile )
        {
            OnStopInspectingRawFile();
        }

        m_mode = Mode::None;
        m_inspectedFile = FileSystem::Path();
    }

    void DataFileInspector::Draw()
    {
        if ( !m_inspectedFile.IsValid() )
        {
            ImGui::Text( "No file inspected" );
            return;
        }

        //-------------------------------------------------------------------------

        switch ( m_mode )
        {
            case DataFileInspector::Mode::InspectingRawFile:
            {
                DrawRawFileInfo();
            }
            break;

            case DataFileInspector::Mode::InspectingResourceFile:
            {
                DrawResourceFileInfo();
            }
            break;
        }
    }

    //-------------------------------------------------------------------------

    void DataFileInspector::OnStartInspectingRawFile()
    {
        if ( RawAssets::ReadFileInfo( m_inspectedFile, m_assetInfo ) )
        {
            m_validAssetInfo = true;
        }
        else
        {
            m_validAssetInfo = false;
        }
    }

    void DataFileInspector::OnStopInspectingRawFile()
    {
        m_assetInfo.Reset();
        m_validAssetInfo = false;
    }

    void DataFileInspector::DrawRawFileInfo()
    {
        KRG_ASSERT( m_inspectedFile.IsFilePath() );

        ImGui::Text( "Raw File: %s", m_inspectedFile.c_str() );

        if ( m_validAssetInfo )
        {
            ImGui::Text( "Original Up Axis: %s", Math::ToString( m_assetInfo.m_upAxis ) );
            ImGui::Text( "Scale: %.2f", m_assetInfo.m_scale );
            ImGui::Separator();

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
                        ImGui::Text( "Skeletal Mesh:" );
                        hasSkeletalMeshes = true;
                    }
                    else // Static
                    {
                        ImGui::Text( "Static Mesh:" );
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
                            /*SkeletalMeshResourceDescriptor resourceDesc;
                            resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                            resourceDesc.m_meshName = mesh.m_name;
                            CreateNewDescriptor( SkeletalMesh::GetStaticResourceTypeID(), resourceDesc );*/
                        }
                    }

                    if ( canCreateSkeletalMesh )
                    {
                        ImGuiX::ItemTooltip( "Create skeletal mesh resource for this sub-mesh" );
                    }

                    ImGui::SameLine( 0, 4 );

                    if ( ImGui::Button( KRG_ICON_CUBE "##CreateStaticMesh", ImVec2( 20, 0 ) ) )
                    {
                        /*StaticMeshResourceDescriptor resourceDesc;
                        resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                        resourceDesc.m_meshName = mesh.m_name;
                        CreateNewDescriptor( StaticMesh::GetStaticResourceTypeID(), resourceDesc );*/
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
                    /*SkeletalMeshResourceDescriptor resourceDesc;
                    resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                    CreateNewDescriptor( SkeletalMesh::GetStaticResourceTypeID(), resourceDesc );*/
                }
            }

            ImGui::SameLine( 0, 4 );

            if ( ImGui::Button( KRG_ICON_CUBE " Create Static Mesh", ImVec2( buttonWidth, 24 ) ) )
            {
                /*StaticMeshResourceDescriptor resourceDesc;
                resourceDesc.m_meshDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                CreateNewDescriptor( StaticMesh::GetStaticResourceTypeID(), resourceDesc );*/
            }
        }
    }

    //-------------------------------------------------------------------------

    bool DataFileInspector::CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const& descriptor ) const
    {
        KRG_ASSERT( m_mode == Mode::InspectingRawFile );
        KRG_ASSERT( resourceTypeID.IsValid() );

        //-------------------------------------------------------------------------

        InlineString<5> extension = resourceTypeID.ToString();
        extension.make_lower();

        FileSystem::Path newDescriptorPath = m_inspectedFile;
        newDescriptorPath.ReplaceExtension( extension );

        InlineString<10> filter;
        filter.sprintf( "*.%s", extension.c_str() );

        //-------------------------------------------------------------------------

        pfd::save_file saveDialog( "Save Resource Descriptor", newDescriptorPath.c_str(), { "Descriptor", filter.c_str() } );
        newDescriptorPath = saveDialog.result().c_str();

        if ( !newDescriptorPath.IsValid() || !newDescriptorPath.IsFilePath() )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        TypeSystem::Serialization::TypeWriter typeWriter( m_typeRegistry );
        typeWriter << &descriptor;
        return typeWriter.WriteToFile( newDescriptorPath );
    }

    //-------------------------------------------------------------------------

    void DataFileInspector::DrawResourceFileInfo()
    {
        KRG_ASSERT( m_mode == Mode::InspectingResourceFile );

        if ( !m_descriptorID.IsValid() )
        {
            ImGui::Text( "No Descriptor open" );
            return;
        }

        if ( m_pDescriptor == nullptr )
        {
            ImGui::Text( "Failed to load descriptor" );
            return;
        }

        //-------------------------------------------------------------------------

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 80;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;
        float const buttonStartPosX = textAreaWidth + itemSpacing;

        ImGui::AlignTextToFramePadding();
        ImGui::Text( "Descriptor: %s", m_descriptorPath.GetFileName().c_str() );
        float const actualTextWidth = ImGui::GetItemRectSize().x;

        ImGui::SameLine( 0, textAreaWidth - actualTextWidth + itemSpacing );
        if ( m_propertyGrid.IsDirty() )
        {
            if ( ImGuiX::ButtonColored( Colors::LimeGreen.ToFloat4(), KRG_ICON_FLOPPY_O " Save", ImVec2( buttonAreaWidth, 0 ) ) )
            {
                SaveLoadedResourceDescriptor();
                m_propertyGrid.ClearDirty();
            }
        }
        else
        {
            ImGuiX::ButtonColored( ImGuiX::Theme::s_textColorDisabled, KRG_ICON_FLOPPY_O " Save", ImVec2( buttonAreaWidth, 0 ) );
        }

        //-------------------------------------------------------------------------

        m_propertyGrid.DrawGrid();
        m_isDirty = m_propertyGrid.IsDirty();
    }

    bool DataFileInspector::LoadResourceDescriptor()
    {
        KRG_ASSERT( m_descriptorID.IsValid() && m_descriptorPath.IsFilePath() );
        KRG_ASSERT( m_pDescriptor == nullptr );

        TypeSystem::Serialization::TypeReader typeReader( m_typeRegistry );
        if ( typeReader.ReadFromFile( m_descriptorPath ) )
        {
            TypeSystem::TypeDescriptor typeDesc;
            if ( typeReader.ReadType( typeDesc ) )
            {
                m_pDescriptor = TypeSystem::TypeCreator::CreateTypeFromDescriptor<Resource::ResourceDescriptor>( m_typeRegistry, typeDesc );
                return true;
            }
        }

        return false;
    }

    bool DataFileInspector::SaveLoadedResourceDescriptor()
    {
        KRG_ASSERT( m_descriptorID.IsValid() && m_descriptorPath.IsFilePath() );
        KRG_ASSERT( m_pDescriptor != nullptr );

        TypeSystem::Serialization::TypeWriter typeWriter( m_typeRegistry );
        typeWriter << m_pDescriptor;
        return typeWriter.WriteToFile( m_descriptorPath );
    }
}