#include "RawResourceInspector.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Tools/Core/Resource/ResourceDatabase.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "imgui.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    RawResourceInspector::RawResourceInspector( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath )
        : m_typeRegistry( typeRegistry )
        , m_rawResourceDirectory( resourceDatabase.GetRawResourceDirectoryPath() )
        , m_filePath( filePath )
        , m_propertyGrid( typeRegistry, resourceDatabase )
    {
        KRG_ASSERT( m_rawResourceDirectory.IsDirectory() && FileSystem::Exists( m_rawResourceDirectory ) );
        KRG_ASSERT( filePath.IsFile() && FileSystem::Exists( filePath ) );
    }

    RawResourceInspector::~RawResourceInspector()
    {
        KRG::Delete( m_pDescriptor );
    }

    bool RawResourceInspector::DrawDialog()
    {
        if ( !ImGui::IsPopupOpen( GetInspectorTitle() ) )
        {
            ImGui::OpenPopup( GetInspectorTitle() );
        }

        //-------------------------------------------------------------------------

        bool isOpen = true;
        ImGui::SetNextWindowSize( ImVec2( 600, 800 ), ImGuiCond_FirstUseEver );
        if ( ImGui::BeginPopupModal( GetInspectorTitle(), &isOpen ) )
        {
            if ( ImGui::BeginTable( "DialogTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_PadOuterX | ImGuiTableFlags_NoSavedSettings, ImVec2( -1, -1 ) ) )
            {
                ImGui::TableSetupColumn( "Info", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Creator", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch );

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0);
                DrawFileInfoAndContents();

                ImGui::TableSetColumnIndex( 1);
                DrawResourceDescriptorCreator();

                ImGui::EndTable();
            }

            ImGui::EndPopup();
        }

        return isOpen && ImGui::IsPopupOpen( GetInspectorTitle() );
    }

    bool RawResourceInspector::CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const* pDescriptor ) const
    {
        KRG_ASSERT( resourceTypeID.IsValid() );

        //-------------------------------------------------------------------------

        InlineString<5> extension = resourceTypeID.ToString();
        extension.make_lower();

        FileSystem::Path newDescriptorPath = m_filePath;
        newDescriptorPath.ReplaceExtension( extension );

        //-------------------------------------------------------------------------

        InlineString<10> const filter( InlineString<10>::CtorSprintf(), "*.%s", extension.c_str() );
        pfd::save_file saveDialog( "Save Resource Descriptor", newDescriptorPath.c_str(), { "Descriptor", filter.c_str() } );
        newDescriptorPath = saveDialog.result().c_str();

        if ( !newDescriptorPath.IsValid() || !newDescriptorPath.IsFile() )
        {
            return false;
        }

        // Ensure correct extension
        if ( !newDescriptorPath.MatchesExtension( extension.c_str() ) )
        {
            newDescriptorPath.Append( "." );
            newDescriptorPath.Append( extension.c_str() );
        }

        //-------------------------------------------------------------------------

        TypeSystem::Serialization::TypeWriter typeWriter( m_typeRegistry );
        typeWriter << pDescriptor;
        return typeWriter.WriteToFile( newDescriptorPath );
    }

    //-------------------------------------------------------------------------

    KRG_DEFINE_GLOBAL_REGISTRY( RawResourceInspectorFactory );

    //-------------------------------------------------------------------------

    bool RawResourceInspectorFactory::CanCreateInspector( FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( pCurrentFactory->IsSupportedFile( filePath ) )
            {
                return true;
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return false;
    }

    RawResourceInspector* RawResourceInspectorFactory::TryCreateInspector( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( pCurrentFactory->IsSupportedFile( filePath ) )
            {
                return pCurrentFactory->CreateInspector( typeRegistry, resourceDatabase, filePath );
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return nullptr;
    }
}