#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API FileSystemPath
    {

    public:

        static char const PathDelimiter;

    public:

        FileSystemPath() {}
        FileSystemPath( String&& path );
        FileSystemPath( String const& path );
        FileSystemPath( char const* pPath );

        inline bool IsValid() const { return !m_fullpath.empty(); }
        bool Exists() const;
        size_t Length() const { return m_fullpath.length(); }
        String const& GetFullPath() const { return m_fullpath; }

        // Append
        FileSystemPath& Append( char const* pPathString );
        inline FileSystemPath& operator+=( char const* pPathString ) { return Append( pPathString ); }
        inline FileSystemPath operator+( char const* pPathString ) const { return FileSystemPath( m_fullpath ).Append( pPathString ); }

        inline FileSystemPath& Append( String const& pathString ) { return Append( pathString.c_str() ); }
        inline FileSystemPath& operator+=( String const& pathString ) { return Append( pathString.c_str() ); }
        inline FileSystemPath operator+( String const& pathString ) const { return FileSystemPath( m_fullpath ).Append( pathString.c_str() ); }

        // Extension
        String GetExtension() const;
        void ReplaceExtension( const char* pExtension );
        inline void ReplaceExtension( String const& extension ) { ReplaceExtension( extension.c_str() ); }

        // Files
        inline bool IsFilePath() const { return m_fullpath[m_fullpath.length() - 1] != PathDelimiter; }
        inline String GetFileName() const { return String( GetFileNameSubstr() ); }
        bool IsFilenameEqual( char const* pString ) const;
        inline bool IsFilenameEqual( String const& pString ) const { KRG_ASSERT( !pString.empty() ); return IsFilenameEqual( pString.c_str() ); }

        // Directories
        FileSystemPath GetParentDirectory() const;
        inline bool IsDirectoryPath() const { return m_fullpath[m_fullpath.length() - 1] == PathDelimiter; }
        String GetDirectoryName() const;
        bool IsUnderDirectory( FileSystemPath parentDirectory ) const;

        // Conversion
        inline operator char const*( ) const { return m_fullpath.c_str(); }
        inline String const& ToString() const { return m_fullpath; }
        inline String const& GetString() const { return m_fullpath; }
        inline char const* c_str() const { return m_fullpath.c_str(); }

        // Comparison
        inline bool operator==( FileSystemPath const& RHS ) const { return m_ID == RHS.m_ID; }
        inline bool operator!=( FileSystemPath const& RHS ) const { return m_ID != RHS.m_ID; }

    private:

        char const* GetFileNameSubstr() const;

    private:

        String  m_fullpath;
        U32     m_ID;
    };
}