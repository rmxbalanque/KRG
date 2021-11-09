#include "ResourcePath.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG
{
    char const* ResourcePath::PathPrefix = "data://";

    //-------------------------------------------------------------------------

    static size_t FindExtensionStartIdx( String const& path )
    {
        size_t const pathDelimiterIdx = path.find_last_of( FileSystem::Path::PathDelimiter );

        size_t idx = path.rfind( '.' );
        size_t prevIdx = idx;
        while ( idx != String::npos && idx > pathDelimiterIdx )
        {
            prevIdx = idx;
            idx = path.rfind( '.', idx - 1 );
        }

        if ( prevIdx != String::npos )
        {
            prevIdx++;
        }

        return prevIdx;
    }

    //-------------------------------------------------------------------------

    // Extremely naive data path validation function, this is definitely not robust!
    bool ResourcePath::IsValidPath( char const* pPath )
    {
        KRG_ASSERT( pPath != nullptr );

        if ( strlen( pPath ) == 0 )
        {
            return false;
        }

        if ( strncmp( ResourcePath::PathPrefix, pPath, strlen( ResourcePath::PathPrefix ) ) != 0 )
        {
            return false;
        }

        if ( strchr( pPath, '\\' ) != nullptr )
        {
            return false;
        }

        return true;
    }

    ResourcePath ResourcePath::FromFileSystemPath( FileSystem::Path const& rawResourceDirectoryPath, FileSystem::Path const& filePath )
    {
        KRG_ASSERT( rawResourceDirectoryPath.IsValid() && rawResourceDirectoryPath.IsDirectory() && filePath.IsValid() );

        ResourcePath path;

        if( filePath.IsUnderDirectory( rawResourceDirectoryPath ) )
        {
            String tempPath = ResourcePath::PathPrefix;
            tempPath.append( filePath.GetString().substr( rawResourceDirectoryPath.Length() ) );
            eastl::replace( tempPath.begin(), tempPath.end(), FileSystem::Path::PathDelimiter, ResourcePath::PathDelimiter );
            path = ResourcePath( tempPath );
        }

        return path;
    }

    FileSystem::Path ResourcePath::ToFileSystemPath( FileSystem::Path const& rawResourceDirectoryPath, ResourcePath const& resourcePath )
    {
        KRG_ASSERT( rawResourceDirectoryPath.IsValid() && rawResourceDirectoryPath.IsDirectory() && resourcePath.IsValid() );

        // Replace slashes and remove prefix
        String tempPath = resourcePath.m_path;
        eastl::replace( tempPath.begin(), tempPath.end(), ResourcePath::PathDelimiter, FileSystem::Path::PathDelimiter );
        tempPath = resourcePath.m_path.substr( 7 );
        return FileSystem::Path( rawResourceDirectoryPath + tempPath );
    }

    //-------------------------------------------------------------------------

    ResourcePath::ResourcePath( String const& path )
        : m_path( path )
    {
        KRG_ASSERT( m_path.empty() || IsValidPath( m_path ) );
        OnPathMemberChanged();
    }

    ResourcePath::ResourcePath( char const* pPath )
        : m_path( pPath )
    {
        KRG_ASSERT( m_path.empty() || IsValidPath( m_path ) );
        OnPathMemberChanged();
    }

    ResourcePath::ResourcePath( String&& path )
    {
        m_path.swap( path );
        KRG_ASSERT( m_path.empty() || IsValidPath( m_path ) );
        OnPathMemberChanged();
    }

    ResourcePath::ResourcePath( ResourcePath const& path )
        : m_path( path.m_path )
        , m_ID( path.m_ID )
    {}

    ResourcePath::ResourcePath( ResourcePath&& path )
        : m_ID( path.m_ID )
    {
        m_path.swap( path.m_path );
    }

    ResourcePath& ResourcePath::operator=( ResourcePath const& path )
    {
        m_path = path.m_path;
        m_ID = path.m_ID;
        return *this;
    }

    ResourcePath& ResourcePath::operator=( ResourcePath&& path )
    {
        m_path.swap( path.m_path );
        m_ID = path.m_ID;
        return *this;
    }

    //-------------------------------------------------------------------------

    void ResourcePath::OnPathMemberChanged()
    {
        if ( !IsValidPath( m_path ) )
        {
            m_path.clear();
        }

        m_path.make_lower();
        m_ID = Hash::GetHash32( m_path );
    }

    String ResourcePath::GetExtension() const
    {
        KRG_ASSERT( IsValid() );
        String ext;

        size_t const extIdx = FindExtensionStartIdx( m_path.c_str() );
        if ( extIdx != String::npos )
        {
            ext = m_path.substr( extIdx, m_path.length() - extIdx );
        }

        return ext;
    }

    void ResourcePath::ReplaceExtension( const char* pExtension )
    {
        KRG_ASSERT( IsValid() && pExtension != nullptr );
        KRG_ASSERT( pExtension[0] != 0 && pExtension[0] != '.' );

        size_t const extIdx = FindExtensionStartIdx( m_path.c_str() );
        if ( extIdx != String::npos )
        {
            m_path = m_path.substr( 0, extIdx ) + pExtension;
        }
        else // No extension, so just append
        {
            m_path.append( "." );
            m_path.append( pExtension );
        }

        OnPathMemberChanged();
    }
}