#include "DataPath.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG
{
    char const* DataPath::PathPrefix = "data://";

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

        KRG_ASSERT( prevIdx != String::npos );
        prevIdx++;

        return prevIdx;
    }

    //-------------------------------------------------------------------------

    // Extremely naive data path validation function, this is definitely not robust!
    bool DataPath::IsValidDataPath( char const* pPath )
    {
        KRG_ASSERT( pPath != nullptr );

        if ( strlen( pPath ) == 0 )
        {
            return false;
        }

        if ( strncmp( DataPath::PathPrefix, pPath, strlen( DataPath::PathPrefix ) ) != 0 )
        {
            return false;
        }

        if ( strchr( pPath, '\\' ) != nullptr )
        {
            return false;
        }

        return true;
    }

    DataPath DataPath::FromFileSystemPath( FileSystem::Path const& sourceDataPath, FileSystem::Path const& filePath )
    {
        KRG_ASSERT( sourceDataPath.IsValid() && sourceDataPath.IsDirectoryPath() && filePath.IsValid() && filePath.IsFilePath() );

        DataPath path;

        if( filePath.IsUnderDirectory( sourceDataPath ) )
        {
            String tempPath = DataPath::PathPrefix;
            tempPath.append( filePath.GetString().substr( sourceDataPath.Length() ) );
            eastl::replace( tempPath.begin(), tempPath.end(), FileSystem::Path::PathDelimiter, DataPath::PathDelimiter );
            path = DataPath( tempPath );
        }

        return path;
    }

    FileSystem::Path DataPath::ToFileSystemPath( FileSystem::Path const& sourceDataPath, DataPath const& dataPath )
    {
        KRG_ASSERT( sourceDataPath.IsValid() && sourceDataPath.IsDirectoryPath() && dataPath.IsValid() );

        // Replace slashes and remove prefix
        String tempPath = dataPath.m_path;
        eastl::replace( tempPath.begin(), tempPath.end(), DataPath::PathDelimiter, FileSystem::Path::PathDelimiter );
        tempPath = dataPath.m_path.substr( 7 );
        return FileSystem::Path( sourceDataPath + tempPath );
    }

    //-------------------------------------------------------------------------

    DataPath::DataPath( String&& path )
        : m_path( path )
    {
        KRG_ASSERT( IsValidDataPath( m_path ) );
        OnPathMemberChanged();
    }

    DataPath::DataPath( String const& path )
        : m_path( path )
    {
        KRG_ASSERT( IsValidDataPath( m_path ) );
        OnPathMemberChanged();
    }

    DataPath::DataPath( char const* pPath )
        : m_path( pPath )
    {
        KRG_ASSERT( IsValidDataPath( m_path ) );
        OnPathMemberChanged();
    }

    void DataPath::OnPathMemberChanged()
    {
        if ( !IsValidDataPath( m_path ) )
        {
            m_path.clear();
        }

        m_path.make_lower();
        m_ID = Hash::GetHash32( m_path );
    }

    String DataPath::GetExtension() const
    {
        KRG_ASSERT( IsValid() );
        size_t const extIdx = FindExtensionStartIdx( m_path.c_str() );
        return m_path.substr( extIdx, m_path.length() - extIdx );
    }

    void DataPath::ReplaceExtension( const char* pExtension )
    {
        KRG_ASSERT( IsValid() && pExtension != nullptr && pExtension[0] != 0 );
        size_t const extIdx = FindExtensionStartIdx( m_path.c_str() );
        m_path = m_path.substr( 0, extIdx ) + pExtension;
        OnPathMemberChanged();
    }
}