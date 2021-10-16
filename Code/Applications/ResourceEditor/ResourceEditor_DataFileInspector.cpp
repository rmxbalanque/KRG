#include "ResourceEditor_DataFileInspector.h"
#include "RawFileInspectors/RawFileInspector.h"
#include "ResourceEditor_Model.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"
#include "Tools/Core/TypeSystem/Serialization/TypeReader.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    DataFileInspector::DataFileInspector( ResourceEditorModel* pModel )
        : m_pModel( pModel )
        , m_propertyGrid( *pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
    {
        KRG_ASSERT( m_pModel != nullptr );
    }

    DataFileInspector::~DataFileInspector()
    {
        ClearFileToInspect();
        KRG_ASSERT( m_pDescriptor == nullptr && m_pRawFileInspector == nullptr );
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

        m_descriptorID = ResourceID( ResourcePath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), m_inspectedFile ) );
        if ( m_descriptorID.IsValid() )
        {
            // Ensure the resource type ID is a registered resource type
            if ( !m_pModel->GetTypeRegistry()->IsRegisteredResourceType( m_descriptorID.GetResourceTypeID() ) )
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
            m_descriptorPath = m_descriptorID.GetPath().ToFileSystemPath( m_pModel->GetSourceDataDirectory() );

            if ( LoadResourceDescriptor() )
            {
                m_propertyGrid.SetTypeToEdit( m_pDescriptor );
            }
        }
        else
        {
            KRG_ASSERT( m_pRawFileInspector == nullptr );
            m_mode = Mode::InspectingRawFile;
            m_pRawFileInspector = RawFileInspectorFactory::TryCreateInspector( m_pModel, m_inspectedFile );
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
            KRG::Delete( m_pRawFileInspector );
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
                if ( m_pRawFileInspector != nullptr )
                {
                    m_pRawFileInspector->Draw();
                }
                else
                {
                    ImGui::Text( "Unknown Format" );
                }
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

        TypeSystem::Serialization::TypeReader typeReader( *m_pModel->GetTypeRegistry() );
        if ( typeReader.ReadFromFile( m_descriptorPath ) )
        {
            TypeSystem::TypeDescriptor typeDesc;
            if ( typeReader.ReadType( typeDesc ) )
            {
                m_pDescriptor = TypeSystem::TypeCreator::CreateTypeFromDescriptor<Resource::ResourceDescriptor>( *m_pModel->GetTypeRegistry(), typeDesc );
                return true;
            }
        }

        return false;
    }

    bool DataFileInspector::SaveLoadedResourceDescriptor()
    {
        KRG_ASSERT( m_descriptorID.IsValid() && m_descriptorPath.IsFilePath() );
        KRG_ASSERT( m_pDescriptor != nullptr );
        return WriteResourceDescriptorToFile( *m_pModel->GetTypeRegistry(), m_descriptorPath, m_pDescriptor );
    }
}