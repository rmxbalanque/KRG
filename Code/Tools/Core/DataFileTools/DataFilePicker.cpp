#include "DataFilePicker.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "System/Imgui/ImguiX.h"
#include <sstream>

//-------------------------------------------------------------------------

namespace KRG
{
    bool DataFilePicker::DrawPickerControl( FileSystem::Path const& sourceDataPath, ResourceTypeID allowedResourceTypeID, ResourceID* pResourceID )
    {
        KRG_ASSERT( sourceDataPath.IsExistingDirectory() );
        KRG_ASSERT( allowedResourceTypeID.IsValid() && pResourceID != nullptr );

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const childWindowWidth = 50;

        //-------------------------------------------------------------------------

        char resourceTypeStr[5] = { 0 };
        allowedResourceTypeID.GetString( resourceTypeStr );

        bool valueUpdated = false;

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
        ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGui::TextColored( Colors::LightPink.ToFloat4(), resourceTypeStr );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, itemSpacing );
        ImGui::SetNextItemWidth( cellContentWidth - ( itemSpacing * 3 ) - ( buttonAreaWidth * 2 ) - childWindowWidth );
        ImGui::InputText( "##DataPath", const_cast<char*>( pResourceID->c_str() ), pResourceID->GetDataPath().GetString().length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
        {
            DataPath pickedDataPath;
            if ( DataFilePicker::PickResourceFile( sourceDataPath, allowedResourceTypeID, pickedDataPath ) )
            {
                *pResourceID = ResourceID( pickedDataPath );
                valueUpdated = true;
            }
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear" ) )
        {
            *pResourceID = ResourceID();
            valueUpdated = true;
        }

        return valueUpdated;
    }

    bool DataFilePicker::PickFile( FileSystem::Path const & sourceDataPath, DataPath& outPath )
    {
        auto const selectedFiles = pfd::open_file( "Choose Data File", sourceDataPath.c_str(), { "All Files", "*" }, pfd::opt::none ).result();
        if ( selectedFiles.empty() )
        {
            return false;
        }

        FileSystem::Path const SelectedPath( selectedFiles[0].c_str() );
        outPath = DataPath::FromFileSystemPath( sourceDataPath, SelectedPath );
        return true;
    }

    bool DataFilePicker::PickResourceFile( FileSystem::Path const& sourceDataPath, ResourceTypeID allowedResourceTypes, DataPath& outPath )
    {
        KRG_ASSERT( allowedResourceTypes.IsValid() );

        char buffer[8] = { 0 };
        buffer[0] = '*';
        buffer[1] = '.';
        allowedResourceTypes.GetString( &buffer[2] );

        //-------------------------------------------------------------------------

        auto const selectedFiles = pfd::open_file( "Choose Resource File", sourceDataPath.c_str(), { buffer, buffer }, pfd::opt::none ).result();
        if ( selectedFiles.empty() )
        {
            return false;
        }

        FileSystem::Path const SelectedPath( selectedFiles[0].c_str() );
        outPath = DataPath::FromFileSystemPath( sourceDataPath, SelectedPath );
        return true;
    }

    bool DataFilePicker::PickResourceFile( FileSystem::Path const& sourceDataPath, TVector<ResourceTypeID> const& allowedResourceTypes, DataPath& outPath )
    {
        KRG_ASSERT( !allowedResourceTypes.empty() );

        //-------------------------------------------------------------------------

        std::stringstream ss;
        std::vector<std::string> fileFilters;
        fileFilters.emplace_back( "Kruger Resources" );

        for ( auto const& resourceTypeID : allowedResourceTypes )
        {
            char buffer[9] = { 0 };
            buffer[0] = ' ';
            buffer[1] = '*';
            buffer[2] = '.';
            resourceTypeID.GetString( &buffer[3] );
            ss << buffer;
        }

        fileFilters.emplace_back( ss.str() );

        //-------------------------------------------------------------------------

        auto const selectedFiles = pfd::open_file( "Choose Resource File", sourceDataPath.c_str(), fileFilters, pfd::opt::none ).result();
        if ( selectedFiles.empty() )
        {
            return false;
        }

        FileSystem::Path const SelectedPath( selectedFiles[0].c_str() );
        outPath = DataPath::FromFileSystemPath( sourceDataPath, SelectedPath );
        return true;
    }
}