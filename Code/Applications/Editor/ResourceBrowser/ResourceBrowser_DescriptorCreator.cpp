#include "ResourceBrowser_DescriptorCreator.h"
#include "Applications/Editor/Editor_Model.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"
#include "Tools/Core/TypeSystem/Serialization/TypeReader.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceDescriptorCreator::ResourceDescriptorCreator( EditorModel* pModel, TypeSystem::TypeID const descriptorTypeID, FileSystem::Path const& startingDir )
        : m_pModel( pModel )
        , m_propertyGrid( *pModel->GetTypeRegistry(), pModel->GetSourceResourceDirectory() )
        , m_startingPath( startingDir )
    {
        KRG_ASSERT( m_pModel != nullptr );
        KRG_ASSERT( m_pModel->GetTypeRegistry()->IsTypeDerivedFrom( descriptorTypeID, Resource::ResourceDescriptor::GetStaticTypeID() ) );
        auto pTypeInfo = m_pModel->GetTypeRegistry()->GetTypeInfo( descriptorTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        m_pDescriptor = Cast<Resource::ResourceDescriptor>( pTypeInfo->m_pTypeHelper->CreateType() );
        KRG_ASSERT( m_pDescriptor != nullptr );

        m_propertyGrid.SetTypeToEdit( m_pDescriptor );

        //-------------------------------------------------------------------------

        InlineString<10> filenameStr;
        filenameStr.sprintf( "NewResource.%s", m_pDescriptor->GetCompiledResourceTypeID().ToString().c_str() );
        m_startingPath += filenameStr.c_str();
    }

    ResourceDescriptorCreator::~ResourceDescriptorCreator()
    {
        KRG::Delete( m_pDescriptor );
    }

    bool ResourceDescriptorCreator::Draw()
    {
        if ( !ImGui::IsPopupOpen( s_title ) )
        {
            ImGui::OpenPopup( s_title );
        }

        //-------------------------------------------------------------------------

        bool isOpen = true;
        ImGui::SetNextWindowSize( ImVec2( 600, 800 ), ImGuiCond_FirstUseEver );
        if ( ImGui::BeginPopupModal( s_title, &isOpen ) )
        {
            m_propertyGrid.DrawGrid();

            //-------------------------------------------------------------------------

            if ( ImGui::Button( "Save", ImVec2( 120, 0 ) ) )
            {
                SaveDescriptor();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if ( ImGui::Button( "Cancel", ImVec2( 120, 0 ) ) )
            {
                ImGui::CloseCurrentPopup();
            }

            //-------------------------------------------------------------------------

            ImGui::EndPopup();
        }

        return isOpen && ImGui::IsPopupOpen( s_title );
    }

    bool ResourceDescriptorCreator::SaveDescriptor()
    {
        auto pTypeRegistry = m_pModel->GetTypeRegistry();

        //-------------------------------------------------------------------------

        ResourceTypeID const resourceTypeID = m_pDescriptor->GetCompiledResourceTypeID();
        InlineString<5> const resourceTypeIDString = resourceTypeID.ToString();
        TypeSystem::ResourceInfo const* pResourceInfo = pTypeRegistry->GetResourceInfoForResourceType( resourceTypeID );

        // Get filename
        //-------------------------------------------------------------------------

        InlineString<10> filterString;
        filterString.sprintf( "*.%s", resourceTypeIDString.c_str() );

        auto const selectedFilePath = pfd::save_file( "Save Descriptor", m_startingPath.c_str(), { pResourceInfo->m_friendlyName.c_str(), filterString.c_str() } ).result();
        if ( selectedFilePath.empty() )
        {
            return false;
        }

        FileSystem::Path const outPath( selectedFilePath.c_str() );

        // Validate filename
        //-------------------------------------------------------------------------

        ResourceID const resourceID = ResourcePath::FromFileSystemPath( m_pModel->GetSourceResourceDirectory(), outPath );
        if ( resourceID.GetResourceTypeID() != pResourceInfo->m_resourceTypeID )
        {
            InlineString<150> errorString;
            errorString.sprintf( "Invalid extension provided! You need to have the .%s extension!", resourceTypeIDString.c_str() );
            pfd::message( "Error", errorString.c_str(), pfd::choice::ok, pfd::icon::error ).result();
            return false;
        }

        // Save descriptor
        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pDescriptor != nullptr );
        return Resource::WriteResourceDescriptorToFile( *pTypeRegistry, outPath, m_pDescriptor );
    }
}