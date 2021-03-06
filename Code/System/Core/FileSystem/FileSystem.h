#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"
#include "FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace FileSystem
    {
        enum class FileType
        {
            Binary,
            ASCII,
        };

        //-------------------------------------------------------------------------

        // Directory functions
        KRG_SYSTEM_CORE_API bool CreateDir( FileSystemPath const& path );
        KRG_SYSTEM_CORE_API bool EraseDir( FileSystemPath const& path );
        KRG_SYSTEM_CORE_API void GetDirectoryContents( FileSystemPath const& directoryPath, TVector<FileSystemPath>& contents, const char* pFileFilter = "*" );
        KRG_SYSTEM_CORE_API bool EnsurePathExists( FileSystemPath const& path );

        // File functions
        KRG_SYSTEM_CORE_API bool IsFileReadOnly( FileSystemPath const& filePath );
        KRG_SYSTEM_CORE_API bool FileExists( FileSystemPath const& filePath );
        KRG_SYSTEM_CORE_API uint64 GetFileModifiedTime( FileSystemPath const& filePath );
        KRG_SYSTEM_CORE_API bool LoadFile( FileSystemPath const& filePath, TVector<Byte>& fileData );
        KRG_SYSTEM_CORE_API bool EraseFile( FileSystemPath const& filePath );

        // Misc
        KRG_SYSTEM_CORE_API FileSystemPath GetCurrentProcessPath();
    }
}