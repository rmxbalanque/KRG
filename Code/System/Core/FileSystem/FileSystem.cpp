#include "FileSystem.h"
#include "System/Core/Logging/Log.h"
#include <filesystem>
#include <regex>

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    bool Exists( char const* pPath )
    {
        std::error_code ec;
        bool const result = std::filesystem::exists( pPath, ec );
        KRG_ASSERT( ec.value() == 0 );
        return result;
    }

    bool IsFile( char const* pPath )
    {
        std::error_code ec;
        bool const result = !std::filesystem::is_directory( pPath, ec );
        KRG_ASSERT( ec.value() == 0 );
        return result;
    }

    bool IsDirectory( char const* pPath )
    {
        std::error_code ec;
        bool const result = std::filesystem::is_directory( pPath, ec );
        KRG_ASSERT( ec.value() == 0 );
        return result;
    }

    //-------------------------------------------------------------------------

    bool Exists( Path const& filePath )
    {
        if ( !Exists( filePath.c_str() ) )
        {
            return false;
        }

        std::error_code ec;
        bool const isDirectory = std::filesystem::is_directory( filePath.c_str(), ec );
        KRG_ASSERT( ec.value() == 0 );

        return isDirectory ? filePath.IsDirectory() : filePath.IsFile();
    }

    bool EnsurePathExists( Path const& path )
    {
        if ( !Exists( path ) )
        {
            if ( path.IsDirectory() )
            {
                return CreateDir( path );
            }
            else
            {
                return CreateDir( path.GetParentDirectory() );
            }
        }

        return true;
    }

    bool IsFileReadOnly( Path const& filePath )
    {
        KRG_ASSERT( filePath.IsFile() );

        std::error_code ec;
        std::filesystem::path const path( filePath.c_str() );
        KRG_ASSERT( std::filesystem::exists( path, ec ) );

        std::filesystem::perms const permissions = std::filesystem::status( path, ec ).permissions();
        KRG_ASSERT( ec.value() == 0 );
        std::filesystem::perms const allWrite = std::filesystem::perms::owner_write | std::filesystem::perms::group_write | std::filesystem::perms::others_write;
        bool const isReadOnly = ( permissions & allWrite ) == std::filesystem::perms::none;
        return isReadOnly;
    }

    bool EraseFile( Path const& filePath )
    {
        KRG_ASSERT( filePath.IsFile() );

        std::error_code ec;
        bool const result = std::filesystem::remove( filePath.c_str(), ec );
        if ( ec.value() != 0 )
        {
            KRG_LOG_ERROR( "FileSystem", ec.message().c_str() );
        }

        return result;
    }

    uint64 GetFileModifiedTime( Path const& filePath )
    {
        KRG_ASSERT( filePath.IsFile() );

        std::error_code ec;
        auto const timepoint = std::filesystem::last_write_time( filePath.c_str(), ec );
        KRG_ASSERT( ec.value() == 0 );
        return timepoint.time_since_epoch().count();
    }

    void EnsureCorrectPathStringFormat( Path& filePath )
    {
        std::error_code ec;
        bool const isPathADirectory = std::filesystem::is_directory( filePath.c_str(), ec );
        if ( ec.value() != 0 )
        {
            return;
        }

        //-------------------------------------------------------------------------

        // Add trailing delimiter for directories
        if ( isPathADirectory && !filePath.IsDirectory() )
        {
            filePath.m_fullpath += Path::PathDelimiter;
        }

        // Remove trailing delimiter for files
        else if ( !isPathADirectory && filePath.IsDirectory() )
        {
            filePath.m_fullpath.pop_back();
        }
    }

    //-------------------------------------------------------------------------

    bool CreateDir( Path const& path )
    {
        KRG_ASSERT( path.IsDirectory() );

        std::error_code ec;
        std::filesystem::create_directories( path.c_str(), ec );
        return ec.value() == 0;
    }

    bool EraseDir( Path const& path )
    {
        KRG_ASSERT( path.IsDirectory() );

        std::error_code ec;
        auto const result = std::filesystem::remove_all( path.c_str(), ec );
        return ec.value() == 0;
    }

    void GetDirectoryContents( Path const& directoryPath, TVector<Path>& contents, const char* pFileFilter )
    {
        KRG_ASSERT( directoryPath.IsDirectory() );

        HANDLE hFind;
        WIN32_FIND_DATAA findFileData;

        // Create search path
        size_t const filterLength = strlen( pFileFilter );
        KRG_ASSERT( filterLength + directoryPath.Length() < 256 );
        char searchParam[256] = { 0 };
        memcpy( searchParam, directoryPath, directoryPath.Length() );
        memcpy( &searchParam[directoryPath.Length()], pFileFilter, strlen( pFileFilter ) );

        // Search directory - Case-insensitive
        hFind = ::FindFirstFileExA( searchParam, FindExInfoStandard, &findFileData, FindExSearchNameMatch, nullptr, 0 );
        if ( hFind != INVALID_HANDLE_VALUE )
        {
            do
            {
                if ( !( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
                {
                    contents.push_back( directoryPath + findFileData.cFileName );
                }
            } while ( ::FindNextFileA( hFind, &findFileData ) );
            ::FindClose( hFind );
        }
    }

    //-------------------------------------------------------------------------

    bool GetDirectoryContents( Path const& directoryPath, TVector<Path>& contents, DirectoryReaderOutput output, DirectoryReaderMode mode, TVector<String> const& extensionFilter )
    {
        KRG_ASSERT( directoryPath.IsDirectory() );

        contents.clear();

        if ( !Exists( directoryPath ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        uint32 const numExtensionFilters = (uint32) extensionFilter.size();
        TVector<String> lowercaseExtensionFilters = extensionFilter;
        for ( auto& extFilter : lowercaseExtensionFilters )
        {
            extFilter.make_lower();
        }

        // Path processing
        //-------------------------------------------------------------------------

        InlineString<15> fileLowercaseExtension;

        auto ProcessPath = [&] ( std::filesystem::path const& path )
        {
            if ( std::filesystem::is_directory( path ) )
            {
                if ( output != DirectoryReaderOutput::OnlyFiles )
                {
                    contents.emplace_back( Path( path.string().c_str() ) );
                }
            }
            else if ( std::filesystem::is_regular_file( path ) )
            {
                if ( output == DirectoryReaderOutput::OnlyDirectories )
                {
                    return;
                }

                bool shouldAddFile = ( numExtensionFilters == 0 );

                // Optional: filter by extensions
                if ( numExtensionFilters > 0 )
                {
                    // Todo: there's likely a more efficient way to do a case insensitive compare
                    fileLowercaseExtension = InlineString<15>( path.extension().u8string().c_str() );
                    fileLowercaseExtension.make_lower();

                    for ( auto i = 0u; i < numExtensionFilters; i++ )
                    {
                        if ( fileLowercaseExtension == lowercaseExtensionFilters[i].c_str() )
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
        KRG_ASSERT( directoryPath.IsDirectory() );
        KRG_ASSERT( pRegexExpression != nullptr );

        contents.clear();

        if ( !Exists( directoryPath ) )
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