#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"
#include "FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    // Raw functions
    //-------------------------------------------------------------------------

    KRG_SYSTEM_CORE_API bool Exists( char const* pPath );
    KRG_SYSTEM_CORE_API bool IsFile( char const* pPath );
    KRG_SYSTEM_CORE_API bool IsDirectory( char const* pPath );

    inline bool Exists( String const& filePath ) { return Exists( filePath.c_str() ); }
    inline bool IsFile( String const& filePath ) { return IsFile( filePath.c_str() ); }
    inline bool IsDirectory( String const& filePath ) { return IsDirectory( filePath.c_str() ); }

    // Path functions
    //-------------------------------------------------------------------------
    // These function also perform additional validation based on the path type

    KRG_SYSTEM_CORE_API bool Exists( Path const& filePath );
    KRG_SYSTEM_CORE_API bool EnsurePathExists( Path const& path );

    KRG_SYSTEM_CORE_API bool IsFileReadOnly( Path const& filePath );
    KRG_SYSTEM_CORE_API uint64 GetFileModifiedTime( Path const& filePath );
    KRG_SYSTEM_CORE_API bool EraseFile( Path const& filePath );
    KRG_SYSTEM_CORE_API bool LoadFile( Path const& filePath, TVector<Byte>& fileData );
    
    KRG_SYSTEM_CORE_API bool CreateDir( Path const& path );
    KRG_SYSTEM_CORE_API bool EraseDir( Path const& path );

    // This function will check the actual filesystem and ensure that the path string has the correct format whether its a file or a directory
    // Note: this is not the cheapest function.
    KRG_SYSTEM_CORE_API void EnsureCorrectPathStringFormat( Path& filePath );
}

//-------------------------------------------------------------------------
// Directory functions
//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    // What should we return
    enum class DirectoryReaderOutput
    {
        All,
        OnlyFiles,
        OnlyDirectories
    };

    // Should we expand sub-directories when reading contents?
    enum class DirectoryReaderMode
    {
        Expand,
        DontExpand,
    };

    // Get the contents of a specified directory
    // The extension filter is a list of extensions including the period e.g. extensionfilter = { ".txt", ".exe" }
    KRG_SYSTEM_CORE_API bool GetDirectoryContents( Path const& directoryPath, TVector<Path>& contents, DirectoryReaderOutput output = DirectoryReaderOutput::All, DirectoryReaderMode mode = DirectoryReaderMode::Expand, TVector<String> const& extensionFilters = TVector<String>() );

    // Get the contents of a specified directory
    // All paths will be matched against the supplied regex expression
    // WARNING!!! THIS IS VERY SLOW
    KRG_SYSTEM_CORE_API bool GetDirectoryContents( Path const& directoryPath, char const* const pRegexExpression, TVector<Path>& contents, DirectoryReaderOutput output = DirectoryReaderOutput::All, DirectoryReaderMode mode = DirectoryReaderMode::Expand );
}

//-------------------------------------------------------------------------
// Miscellaneous functions
//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    KRG_SYSTEM_CORE_API Path GetCurrentProcessPath();
}