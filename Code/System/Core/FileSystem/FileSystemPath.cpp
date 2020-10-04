#include "FileSystemPath.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG
{
    static size_t FindExtensionStartIdx( String const& path )
    {
        size_t const pathDelimiterIdx = path.find_last_of( FileSystemPath::PathDelimiter );

        size_t idx = path.rfind( '.' );
        size_t prevIdx = idx;
        while ( idx != String::npos && idx > pathDelimiterIdx )
        {
            prevIdx = idx;
            idx = path.rfind( '.', idx - 1 );
        }

        KRG_ASSERT( prevIdx != String::npos );
        prevIdx++;

        return prevIdx;
    }

    bool FileSystemPath::IsFilenameEqual( char const* pString ) const
    {
        KRG_ASSERT( pString != nullptr );
        return strcmp( GetFileNameSubstr(), pString ) == 0;
    }

    //-------------------------------------------------------------------------

    FileSystemPath FileSystemPath::GetParentDirectory() const
    {
        KRG_ASSERT( IsValid() );

        String dirPath;
        size_t const lastSlashIdx = m_fullpath.rfind( PathDelimiter );
        if ( lastSlashIdx != String::npos )
        {
            dirPath = m_fullpath.substr( 0, lastSlashIdx + 1 );
        }

        return FileSystemPath( dirPath );
    }

    bool FileSystemPath::IsUnderDirectory( FileSystemPath parentDirectory ) const
    {
        return m_fullpath.find( parentDirectory.m_fullpath ) == 0;
    }

    String FileSystemPath::GetExtension() const
    {
        KRG_ASSERT( IsValid() && IsFilePath() );
        size_t const extIdx = FindExtensionStartIdx( m_fullpath.c_str() );
        return m_fullpath.substr( extIdx, m_fullpath.length() - extIdx );
    }

    void FileSystemPath::ReplaceExtension( const char* pExtension )
    {
        KRG_ASSERT( IsValid() && IsFilePath() && pExtension != nullptr && pExtension[0] != 0 );
        size_t const extIdx = FindExtensionStartIdx( m_fullpath.c_str() );
        m_fullpath = m_fullpath.substr( 0, extIdx ) + pExtension;
    }

    char const* FileSystemPath::GetFileNameSubstr() const
    {
        KRG_ASSERT( IsValid() && IsFilePath() );
        auto idx = m_fullpath.find_last_of( PathDelimiter );
        KRG_ASSERT( idx != String::npos );

        idx++;
        return &m_fullpath[idx];
    }

    String FileSystemPath::GetDirectoryName() const
    {
        KRG_ASSERT( IsValid() && IsDirectoryPath() );

        size_t idx = m_fullpath.rfind( PathDelimiter );
        KRG_ASSERT( idx != String::npos );

        idx = m_fullpath.rfind( PathDelimiter, Math::Max( size_t( 0 ), idx - 1 ) );
        if ( idx == String::npos )
        {
            return String();
        }

        idx++;
        return m_fullpath.substr( idx, m_fullpath.length() - idx - 1 );
    }
}