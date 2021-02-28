#ifdef _WIN32
#include "../FileSystem.h"
#include "System/Core/Platform/Platform_Win32.h"
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

namespace KRG
{
    char const FileSystemPath::PathDelimiter = '\\';

    //-------------------------------------------------------------------------

    static String GetFullPathString( char const* pPath )
    {
        KRG_ASSERT( pPath != nullptr && pPath[0] != 0 );

        // Warning: this function is slow, so use sparingly
        char fullpath[256] = { 0 };
        DWORD length = GetFullPathNameA( pPath, 256, fullpath, nullptr );
        KRG_ASSERT( length != 0 && length != 255 );

        // Ensure directory paths have the final slash appended
        DWORD const result = GetFileAttributesA( fullpath );
        if ( result != INVALID_FILE_ATTRIBUTES && ( result & FILE_ATTRIBUTE_DIRECTORY ) && fullpath[length - 1] != FileSystemPath::PathDelimiter )
        {
            fullpath[length] = FileSystemPath::PathDelimiter;
            fullpath[length + 1] = 0;
        }

        return String( fullpath );
    }

    static bool IsValidPath()
    {
        // TODO: path validation
        return true;
    }

    //-------------------------------------------------------------------------

    FileSystemPath::FileSystemPath( String&& path )
        : m_fullpath( GetFullPathString( path.c_str() ) )
        , m_ID( Hash::GetHash32( m_fullpath ) )
    {
        if ( !IsValidPath() )
        {
            m_fullpath.clear();
        }
    }

    FileSystemPath::FileSystemPath( String const& path )
        : m_fullpath( GetFullPathString( path.c_str() ) )
        , m_ID( Hash::GetHash32( m_fullpath ) )
    {
        if ( !IsValidPath() )
        {
            m_fullpath.clear();
        }
    }

    FileSystemPath::FileSystemPath( char const* pPath )
        : m_fullpath( GetFullPathString( pPath ) )
        , m_ID( Hash::GetHash32( m_fullpath ) )
    {
        if ( !IsValidPath() )
        {
            m_fullpath.clear();
        }
    }

    FileSystemPath& FileSystemPath::Append( char const* pPathString )
    {
        KRG_ASSERT( IsValid() );

        m_fullpath += pPathString;
        m_fullpath = GetFullPathString( m_fullpath.c_str() );

        if ( !IsValidPath() )
        {
            m_fullpath.clear();
        }

        m_ID = Hash::GetHash32( m_fullpath );
        return *this;
    }

    bool FileSystemPath::Exists() const
    {
        KRG_ASSERT( IsValid() );

        bool const isDirectoryPath = IsDirectoryPath();

        DWORD const attribs = GetFileAttributesA( m_fullpath.c_str() );
        if ( attribs == INVALID_FILE_ATTRIBUTES )
        {
            return false;
        }

        // Check if the path is a directory
        if ( isDirectoryPath )
        {
            return ( attribs & FILE_ATTRIBUTE_DIRECTORY ) > 0;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    namespace FileSystem
    {
        bool CreateDir( FileSystemPath const& path )
        {
            KRG_ASSERT( path.IsDirectoryPath() );

            // Creates intermediate directories as well!
            auto result = SHCreateDirectoryEx( nullptr, path, nullptr );
            return SUCCEEDED( result );
        }

        bool EraseDir( FileSystemPath const& path )
        {
            KRG_ASSERT( path.IsDirectoryPath() );

            if ( path.Exists() )
            {
                char dir[MAX_PATH + 1];  // +1 for the double null terminate
                strcpy_s( dir, MAX_PATH + 1, path );
                dir[path.Length() + 1] = 0; // double null terminate for SHFileOperation

                SHFILEOPSTRUCT fos = { 0 };
                fos.wFunc = FO_DELETE;
                fos.pFrom = dir;
                fos.fFlags = FOF_NO_UI;
                return SHFileOperation( &fos ) == 0;
            }
            else
            {
                return true;
            }
        }

        void GetDirectoryContents( FileSystemPath const& directoryPath, TVector<FileSystemPath>& contents, const char* pFileFilter )
        {
            KRG_ASSERT( directoryPath.IsDirectoryPath() );

            HANDLE hFind;
            WIN32_FIND_DATAA fd;

            // Create search path
            size_t const filterLength = strlen( pFileFilter );
            KRG_ASSERT( filterLength + directoryPath.Length() < 256 );
            char searchParam[256] = {0};
            memcpy( searchParam, directoryPath, directoryPath.Length() );
            memcpy( &searchParam[directoryPath.Length()], pFileFilter, strlen( pFileFilter ) );

            // Search folder
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

        bool FileExists( FileSystemPath const& filePath )
        {
            DWORD const attr = GetFileAttributes( filePath );
            return ( attr != INVALID_FILE_ATTRIBUTES && !( attr & FILE_ATTRIBUTE_DIRECTORY ) );
        }

        bool IsFileReadOnly( FileSystemPath const& filePath )
        {
            KRG_ASSERT( FileExists( filePath ) );
            DWORD const attr = GetFileAttributes( filePath );
            return attr & FILE_ATTRIBUTE_READONLY;
        }

        bool EraseFile( FileSystemPath const& filePath )
        {
            return SUCCEEDED( DeleteFileA( filePath ) );
        }

        U64 GetFileModifiedTime( FileSystemPath const& filePath )
        {
            auto file = CreateFileA( filePath, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
            if ( file != INVALID_HANDLE_VALUE )
            {
                FILETIME ft;
                auto result = GetFileTime( file, nullptr, nullptr, &ft );
                if ( SUCCEEDED( result ) )
                {
                    return *( (UINT64*) &ft );
                }
            }
            return 0;
        }

        FileSystemPath GetCurrentProcessPath()
        { 
            return FileSystemPath( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();
        }

        //-------------------------------------------------------------------------

        bool LoadFile( FileSystemPath const& path, TVector<Byte>& fileData )
        {
            KRG_ASSERT( path.IsFilePath() );

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
}

#endif