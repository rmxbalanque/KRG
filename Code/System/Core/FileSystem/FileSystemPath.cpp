#include "FileSystemPath.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    static bool IsValidPath( String const& pathString )
    {
        // TODO: path validation
        return !pathString.empty();
    }

    //-------------------------------------------------------------------------

    static size_t FindExtensionStartIdx( String const& path )
    {
        // Try to find the last period in the path
        //-------------------------------------------------------------------------

        size_t extensionIdx = path.rfind( '.' );
        if ( extensionIdx == String::npos )
        {
            return extensionIdx;
        }

        // If we have the path delimiter and it is after our last period, then there's no extension
        //-------------------------------------------------------------------------

        size_t const pathDelimiterIdx = path.find_last_of( Path::PathDelimiter );
        if ( pathDelimiterIdx > extensionIdx )
        {
            extensionIdx = String::npos;
            return extensionIdx;
        }

        // Try to handle multi-extensions like ".tar.gzip"
        //-------------------------------------------------------------------------

        size_t prevIdx = extensionIdx;
        while ( extensionIdx != String::npos && extensionIdx > pathDelimiterIdx )
        {
            prevIdx = extensionIdx;
            extensionIdx = path.rfind( '.', extensionIdx - 1 );
        }

        KRG_ASSERT( prevIdx != String::npos );
        prevIdx++;

        //-------------------------------------------------------------------------

        extensionIdx = prevIdx;
        return extensionIdx;
    }

    bool Path::IsFilenameEqual( char const* pString ) const
    {
        KRG_ASSERT( pString != nullptr );
        return strcmp( GetFileNameSubstr(), pString ) == 0;
    }

    //-------------------------------------------------------------------------

    Path::Path( String&& path )
        : m_fullpath( GetFullPathString( path.c_str() ) )
        , m_hashCode( Hash::GetHash32( m_fullpath ) )
    {
        if ( !IsValidPath( m_fullpath ) )
        {
            m_fullpath.clear();
        }
    }

     Path::Path( String const& path )
         : m_fullpath( GetFullPathString( path.c_str() ) )
         , m_hashCode( Hash::GetHash32( m_fullpath ) )
     {
         if ( !IsValidPath( m_fullpath ) )
         {
             m_fullpath.clear();
         }
     }

    Path::Path( char const* pPath )
        : m_fullpath( GetFullPathString( pPath ) )
        , m_hashCode( Hash::GetHash32( m_fullpath ) )
    {
        if ( !IsValidPath( m_fullpath ) )
        {
            m_fullpath.clear();
        }
    }

    Path& Path::operator=( Path& rhs )
    {
        m_fullpath = rhs.m_fullpath;
        m_hashCode = rhs.m_hashCode;
        return *this;
    }

    Path& Path::operator=( Path const& rhs )
    {
        m_fullpath = rhs.m_fullpath;
        m_hashCode = rhs.m_hashCode;
        return *this;
    }

    Path& Path::operator=( Path&& rhs )
    {
        m_fullpath.swap( rhs.m_fullpath );
        m_hashCode = rhs.m_hashCode;
        return *this;
    }

    //-------------------------------------------------------------------------

    Path& Path::Append( char const* pPathString, bool asDirectory )
    {
        KRG_ASSERT( IsValid() );

        m_fullpath += pPathString;
        m_fullpath = GetFullPathString( m_fullpath.c_str() );

        if ( asDirectory && !IsDirectory() )
        {
            m_fullpath += PathDelimiter;
        }

        if ( !IsValidPath( m_fullpath ) )
        {
            m_fullpath.clear();
        }

        m_hashCode = Hash::GetHash32( m_fullpath );
        return *this;
    }

    TInlineVector<String, 10> Path::Split() const
    {
        TInlineVector<String, 10> split;

        size_t previousDelimiterIdx = 0;
        size_t currentDelimiterIdx = m_fullpath.find( PathDelimiter );
        while ( currentDelimiterIdx != String::npos )
        {
            KRG_ASSERT( currentDelimiterIdx > previousDelimiterIdx );
            split.emplace_back( m_fullpath.substr( previousDelimiterIdx, currentDelimiterIdx - previousDelimiterIdx ) );

            previousDelimiterIdx = currentDelimiterIdx + 1;
            currentDelimiterIdx = m_fullpath.find( PathDelimiter, previousDelimiterIdx );
        }

        return split;
    }

    TInlineVector<Path, 10> Path::GetDirectoryHierarchy() const
    {
        TInlineVector<Path, 10> directoryHierarchy;

        // Get all previous parent paths
        directoryHierarchy.emplace_back( GetParentDirectory() );
        while ( directoryHierarchy.back().IsValid() )
        {
            directoryHierarchy.emplace_back( directoryHierarchy.back().GetParentDirectory() );
        }

        // Remove lats invalid path
        if ( !directoryHierarchy.empty() )
        {
            directoryHierarchy.pop_back();
        }

        return directoryHierarchy;
    }

    int32 Path::GetPathDepth() const
    {
        int32 pathDepth = -1;

        if ( IsValid() )
        {
            size_t delimiterIdx = m_fullpath.find( PathDelimiter );
            while ( delimiterIdx != String::npos )
            {
                pathDepth++;
                delimiterIdx = m_fullpath.find( PathDelimiter, delimiterIdx + 1 );
            }
        }

        return pathDepth;
    }

    //-------------------------------------------------------------------------

    Path Path::GetParentDirectory() const
    {
        KRG_ASSERT( IsValid() );

        size_t lastDelimiterIdx = m_fullpath.rfind( PathDelimiter );

        // Handle directory paths
        if ( lastDelimiterIdx == m_fullpath.length() - 1 )
        {
            lastDelimiterIdx = m_fullpath.rfind( PathDelimiter, lastDelimiterIdx - 1 );
        }

        //-------------------------------------------------------------------------

        Path parentPath;

        // If we found a parent, create the substring for it
        if ( lastDelimiterIdx != String::npos )
        {
            parentPath = Path( m_fullpath.substr( 0, lastDelimiterIdx + 1 ) );;
        }

        return parentPath;
    }

    bool Path::IsUnderDirectory( Path const& parentDirectory ) const
    {
        return m_fullpath.find( parentDirectory.m_fullpath ) == 0;
    }

    void Path::MakeDirectory()
    {
        if ( !IsDirectory() )
        {
            m_fullpath.push_back( PathDelimiter );
            m_hashCode = Hash::GetHash32( m_fullpath );
        }
    }

    void Path::ReplaceParentDirectory( Path const& newParentDirectory )
    {
        KRG_ASSERT( IsValid() );

        size_t lastDelimiterIdx = m_fullpath.rfind( PathDelimiter );

        // Handle directory paths
        if ( lastDelimiterIdx == m_fullpath.length() - 1 )
        {
            lastDelimiterIdx = m_fullpath.rfind( PathDelimiter, lastDelimiterIdx - 1 );
        }

        m_fullpath.replace( 0, lastDelimiterIdx + 1, newParentDirectory.ToString() );
        m_hashCode = Hash::GetHash32( m_fullpath );
    }

    String Path::GetDirectoryName() const
    {
        KRG_ASSERT( IsValid() && IsDirectory() );

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

    //-------------------------------------------------------------------------

    bool Path::HasExtension() const
    {
        return FindExtensionStartIdx( m_fullpath ) != String::npos;
    }

    bool Path::MatchesExtension( char const* inExtension ) const
    {
        size_t const extensionIdx = FindExtensionStartIdx( m_fullpath );
        if ( extensionIdx == String::npos )
        {
            return false;
        }

        return _stricmp( inExtension, &m_fullpath.at( extensionIdx ) ) == 0;
    }

    char const* Path::GetExtension() const
    {
        KRG_ASSERT( IsValid() && IsFile() );
        size_t const extIdx = FindExtensionStartIdx( m_fullpath );
        KRG_ASSERT( extIdx != String::npos );
        return &m_fullpath[extIdx];
    }

    void Path::ReplaceExtension( const char* pExtension )
    {
        KRG_ASSERT( IsValid() && IsFile() && pExtension != nullptr && pExtension[0] != 0 );
        size_t const extIdx = FindExtensionStartIdx( m_fullpath );
        KRG_ASSERT( extIdx != String::npos );
        m_fullpath = m_fullpath.substr( 0, extIdx ) + pExtension;
    }

    char const* Path::GetFileNameSubstr() const
    {
        KRG_ASSERT( IsValid() && IsFile() );
        auto idx = m_fullpath.find_last_of( PathDelimiter );
        KRG_ASSERT( idx != String::npos );

        idx++;
        return &m_fullpath[idx];
    }

    String Path::GetFileNameWithoutExtension() const
    {
        KRG_ASSERT( IsValid() && IsFile() );
        auto filenameStartIdx = m_fullpath.find_last_of( PathDelimiter );
        KRG_ASSERT( filenameStartIdx != String::npos );
        filenameStartIdx++;

        //-------------------------------------------------------------------------

        size_t extStartIdx = FindExtensionStartIdx( m_fullpath );
        KRG_ASSERT( extStartIdx != String::npos );

        //-------------------------------------------------------------------------

        return m_fullpath.substr( filenameStartIdx, extStartIdx - filenameStartIdx - 1 );
    }
}