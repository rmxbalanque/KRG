#include "FileSystem.h"
#include <filesystem>
#include <regex>

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    bool CreateDir( Path const& path )
    {
        KRG_ASSERT( path.IsDirectoryPath() );
        return std::filesystem::create_directory( path.c_str() );
    }

    bool EraseDir( Path const& path )
    {
        KRG_ASSERT( path.IsDirectoryPath() );
        
        std::error_code ec;
        auto const result = std::filesystem::remove_all( path.c_str(), ec );
        return ec.value() == 0;
    }

    void GetDirectoryContents( Path const& directoryPath, TVector<Path>& contents, const char* pFileFilter )
    {
        KRG_ASSERT( directoryPath.IsDirectoryPath() );

        HANDLE hFind;
        WIN32_FIND_DATAA fd;

        // Create search path
        size_t const filterLength = strlen( pFileFilter );
        KRG_ASSERT( filterLength + directoryPath.Length() < 256 );
        char searchParam[256] = { 0 };
        memcpy( searchParam, directoryPath, directoryPath.Length() );
        memcpy( &searchParam[directoryPath.Length()], pFileFilter, strlen( pFileFilter ) );

        // Search directory
        hFind = ::FindFirstFileA( searchParam, &fd );
        if ( hFind != INVALID_HANDLE_VALUE )
        {
            do
            {
                if ( !( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
                {
                    contents.push_back( directoryPath + fd.cFileName );
                }
            } while ( ::FindNextFileA( hFind, &fd ) );
            ::FindClose( hFind );
        }
    }

    //-------------------------------------------------------------------------

    bool GetDirectoryContents( Path const& directoryPath, TVector<Path>& contents, DirectoryReaderOutput output, DirectoryReaderMode mode, char const* const extensionfilter[] )
    {
        KRG_ASSERT( directoryPath.IsDirectoryPath() );

        contents.clear();

        if ( !directoryPath.Exists() )
        {
            return false;
        }

        // Count how many filters we have
        //-------------------------------------------------------------------------

        uint32 numExtensionFilters = 0;
        if ( output != DirectoryReaderOutput::OnlyDirectories )
        {
            if ( extensionfilter != nullptr )
            {
                while ( extensionfilter[numExtensionFilters] != 0 )
                {
                    numExtensionFilters++;
                }
            }
        }

        // Path processing
        //-------------------------------------------------------------------------

        auto ProcessPath = [&] ( std::filesystem::path const& path )
        {
            if ( std::filesystem::is_directory( path ) )
            {
                if ( output != DirectoryReaderOutput::OnlyFiles )
                {
                    contents.emplace_back( Path( path.string().c_str() ) );
                }
            }
            else if( std::filesystem::is_regular_file( path ) )
            {
                if ( output == DirectoryReaderOutput::OnlyDirectories )
                {
                    return;
                }

                bool shouldAddFile = ( numExtensionFilters == 0 );

                // Optional: filter by extensions
                if ( numExtensionFilters > 0 )
                {
                    auto const extension = path.extension();
                    for ( auto i = 0u; i < numExtensionFilters; i++ )
                    {
                        if ( extension == extensionfilter[i] )
                        {
                            shouldAddFile = true;
                            break;
                        }
                    }
                }

                if ( shouldAddFile )
                {
                    contents.emplace_back( Path( path.string().c_str() ) );
                }
            }
            else
            {
                // Do nothing
            }
        };

        // Directory iteration
        //-------------------------------------------------------------------------

        switch ( mode )
        {
            case DirectoryReaderMode::Expand:
            {
                for ( auto& directoryEntry : std::filesystem::recursive_directory_iterator( directoryPath.c_str() ) )
                {
                    ProcessPath( directoryEntry.path() );
                }
            }
            break;

            case DirectoryReaderMode::DontExpand:
            {
                for ( auto& directoryEntry : std::filesystem::directory_iterator( directoryPath.c_str() ) )
                {
                    ProcessPath( directoryEntry.path() );
                }
            }
            break;
        }

        //-------------------------------------------------------------------------

        return true;
    }

    bool GetDirectoryContents( Path const& directoryPath, char const* const pRegexExpression, TVector<Path>& contents, DirectoryReaderOutput output, DirectoryReaderMode mode )
    {
        KRG_ASSERT( directoryPath.IsDirectoryPath() );
        KRG_ASSERT( pRegexExpression != nullptr );

        contents.clear();

        if ( !directoryPath.Exists() )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        std::regex const patternToMatch( pRegexExpression, std::regex::icase );

        // Path processing
        //-------------------------------------------------------------------------

        auto ProcessPath = [&] ( std::filesystem::path const& path )
        {
            if ( std::filesystem::is_directory( path ) )
            {
                if ( output == DirectoryReaderOutput::OnlyFiles )
                {
                    return;
                }

                auto const pathStr = path.string();
                if ( std::regex_match( pathStr, patternToMatch ) )
                {
                    contents.emplace_back( Path( pathStr.c_str() ) );
                }
            }
            else if ( std::filesystem::is_regular_file( path ) )
            {
                if ( output == DirectoryReaderOutput::OnlyDirectories )
                {
                    return;
                }

                auto const pathStr = path.string();
                if ( std::regex_match( pathStr, patternToMatch ) )
                {
                    contents.emplace_back( Path( pathStr.c_str() ) );
                }
            }
            else
            {
                // Do nothing
            }
        };

        // Directory iteration
        //-------------------------------------------------------------------------

        switch ( mode )
        {
            case DirectoryReaderMode::Expand:
            {
                for ( auto& directoryEntry : std::filesystem::recursive_directory_iterator( directoryPath.c_str() ) )
                {
                    ProcessPath( directoryEntry.path() );
                }
            }
            break;

            case DirectoryReaderMode::DontExpand:
            {
                for ( auto& directoryEntry : std::filesystem::directory_iterator( directoryPath.c_str() ) )
                {
                    ProcessPath( directoryEntry.path() );
                }
            }
            break;
        }

        return true;
    }
}

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    bool EnsurePathExists( Path const& path )
    {
        if ( !path.Exists() )
        {
            return path.IsDirectoryPath() ? CreateDir( path ) : CreateDir( path.GetParentDirectory() );
        }

        return true;
    }

    bool FileExists( Path const& filePath )
    {
        return std::filesystem::exists( filePath.c_str() );
    }

    bool IsFileReadOnly( Path const& filePath )
    {
        std::filesystem::path const path( filePath.c_str() );
        KRG_ASSERT( std::filesystem::exists( path ) );

        std::filesystem::perms const permissions = std::filesystem::status( path ).permissions();
        std::filesystem::perms const allWrite = std::filesystem::perms::owner_write | std::filesystem::perms::group_write | std::filesystem::perms::others_write;
        bool const isReadOnly = ( permissions & allWrite ) == std::filesystem::perms::none;
        return isReadOnly;
    }

    bool EraseFile( Path const& filePath )
    {
        return std::filesystem::remove( filePath.c_str() );
    }

    uint64 GetFileModifiedTime( Path const& filePath )
    {
        KRG_ASSERT( std::filesystem::exists( filePath.c_str() ) );
        auto const timepoint = std::filesystem::last_write_time( filePath.c_str() );
        return timepoint.time_since_epoch().count();
    }
}