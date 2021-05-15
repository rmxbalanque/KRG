#include "DataFilePicker.h"
#include "Tools/Core/Thirdparty/pfd/portable-file-dialogs.h"

//-------------------------------------------------------------------------

namespace KRG::Editor
{
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