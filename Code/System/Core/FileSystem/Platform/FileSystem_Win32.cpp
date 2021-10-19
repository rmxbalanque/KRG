#ifdef _WIN32
#include "../FileSystem.h"
#include "System/Core/Platform/PlatformHelpers_Win32.h"
#include "System/Core/Algorithm/Hash.h"
#include "System/Core/Math/Math.h"
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>


#include <fstream>
#include "../Time/Timers.h"
#include "../Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    char const Path::PathDelimiter = '\\';

    //-------------------------------------------------------------------------

    String Path::GetFullPathString( char const* pPath )
    {
        char fullpath[256] = { 0 };

        if ( pPath != nullptr && pPath[0] != 0 )
        {
            // Warning: this function is slow, so use sparingly
            DWORD length = GetFullPathNameA( pPath, 256, fullpath, nullptr );
            KRG_ASSERT( length != 0 && length != 255 );

            // Ensure directory paths have the final slash appended
            DWORD const result = GetFileAttributesA( fullpath );
            if ( result != INVALID_FILE_ATTRIBUTES && ( result & FILE_ATTRIBUTE_DIRECTORY ) && fullpath[length - 1] != Path::PathDelimiter )
            {
                fullpath[length] = Path::PathDelimiter;
                fullpath[length + 1] = 0;
            }
        }

        return String( fullpath );
    }

    //-------------------------------------------------------------------------

    Path GetCurrentProcessPath()
    {
        return Path( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();
    }

    //-------------------------------------------------------------------------

    bool LoadFile( Path const& path, TVector<Byte>& fileData )
    {
        KRG_ASSERT( path.IsFile() );

        // Open file handle
        HANDLE hFile = CreateFile( path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr );
        if ( hFile == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        // Get file size
        LARGE_INTEGER fileSizeLI;
        if ( !GetFileSizeEx( hFile, &fileSizeLI ) )
        {
            CloseHandle( hFile );
            return false;
        }

        // Allocate destination memory
        size_t const fileSize = (size_t) ( fileSizeLI.QuadPart );
        fileData.resize( fileSize );

        // Read file
        static constexpr DWORD const defaultReadBufferSize = 65536;
        DWORD bytesRead = 0;
        DWORD remainingBytesToRead = (DWORD) fileSize;

        Byte* pBuffer = fileData.data();
        while ( remainingBytesToRead != 0 )
        {
            DWORD const numBytesToRead = Math::Min( defaultReadBufferSize, remainingBytesToRead );
            ReadFile( hFile, pBuffer, numBytesToRead, &bytesRead, nullptr );
            pBuffer += bytesRead;
            remainingBytesToRead -= bytesRead;
        }

        CloseHandle( hFile );
        return true;
    }
}

#endif