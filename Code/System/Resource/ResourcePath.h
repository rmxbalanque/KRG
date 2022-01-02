#pragma once

#include "_Module/API.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// Path relative to the currently set source data directory
//-------------------------------------------------------------------------
// Note: data paths are ALWAYS lowercase
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_RESOURCE_API ResourcePath
    {
        friend cereal::access;

        template <class Archive>
        void save( Archive& ar ) const
        {
            ar( m_path );
        }

        template <class Archive>
        void load( Archive& ar )
        {
            ar( m_path );
            OnPathMemberChanged();
        }

    public:

        constexpr static char const* s_pathPrefix = "data://";
        constexpr static int32 const s_pathPrefixLength = 7;
        constexpr static char const s_pathDelimiter = '/';

        static bool IsValidPath( char const* pPath );
        static bool IsValidPath( String const& path ) { return IsValidPath( path.c_str() ); }

        static ResourcePath FromFileSystemPath( FileSystem::Path const& rawResourceDirectoryPath, FileSystem::Path const& filePath );
        static FileSystem::Path ToFileSystemPath( FileSystem::Path const& rawResourceDirectoryPath, ResourcePath const& resourcePath );

    public:

        ResourcePath() = default;
        ResourcePath( ResourcePath&& path );
        ResourcePath( ResourcePath const& path );
        explicit ResourcePath( String const& path );
        explicit ResourcePath( char const* pPath );
        explicit ResourcePath( String&& path );

        //-------------------------------------------------------------------------

        inline bool IsValid() const { return !m_path.empty() && IsValidPath( m_path ); }
        inline void Clear() { m_path.clear(); m_ID = 0; }
        inline uint32 GetID() const { return m_ID; }

        // Path info
        //-------------------------------------------------------------------------

        // returns the filename with all the 'extensions' removed (i.e. file.final.png -> file )
        String GetFileNameWithoutExtension() const;

        // Get the containing directory path for this path
        String GetParentDirectory() const;

        // Get the directory depth for this path e.g. D:\Foo\Bar\Moo.txt = 2
        int32 GetDirectoryDepth() const;

        // Get the full path depth for this path e.g. D:\Foo\Bar\Moo.txt = 3
        int32 GetPathDepth() const;

        // Is this a file path
        inline bool IsFile() const { KRG_ASSERT( IsValid() ); return m_path[m_path.length() - 1] != s_pathDelimiter; }

        // Is this a directory path
        inline bool IsDirectory() const { KRG_ASSERT( IsValid() ); return ( m_path[m_path.length() - 1] == s_pathDelimiter ); }

        // Extension
        //-------------------------------------------------------------------------

        // Get the extension for this path (excluding the '.')
        String GetExtension() const;

        // Replaces the extension for this path. Supplied extension must not start with '.'
        void ReplaceExtension( const char* pExtension );

        // Replaces the extension for this path. Supplied extension must not start with '.'
        inline void ReplaceExtension( String const& extension ) { ReplaceExtension( extension.c_str() ); }

        // Replaces the extension for this path. Supplied extension must not start with '.'
        template<size_t S> void ReplaceExtension( TInlineString<S> const& extension ) { ReplaceExtension( extension.c_str() ); }

        // Conversion
        //-------------------------------------------------------------------------

        inline String const& GetString() const { return m_path; }
        inline char const* c_str() const { return m_path.c_str(); }
        inline FileSystem::Path ToFileSystemPath( FileSystem::Path const& rawResourceDirectoryPath ) const { return ToFileSystemPath( rawResourceDirectoryPath, *this ); }

        // Operators
        //-------------------------------------------------------------------------

        inline bool operator==( ResourcePath const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( ResourcePath const& rhs ) const { return m_ID != rhs.m_ID; }

        ResourcePath& operator=( ResourcePath&& path );
        ResourcePath& operator=( ResourcePath const& path );

    private:

        void OnPathMemberChanged();

    private:

        String              m_path;
        uint32              m_ID = 0;
    };
}

//-------------------------------------------------------------------------
// Support for THashMap

namespace eastl
{
    template <>
    struct hash<KRG::ResourcePath>
    {
        eastl_size_t operator()( KRG::ResourcePath const& ID ) const
        {
            return ID.GetID();
        }
    };
}