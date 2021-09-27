#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// File System Path
//-------------------------------------------------------------------------
// A helper to abstract and perform some file system path operations
// Not the smartest thing in the world, and has some limitations made for performance
// 
// For directory paths, the most robust methods to specify a directory path is to end with a path delimiter, 
// else we cant distinguish between a directory with a period in the name and a file. So we assume any path that 
// has an extension at the end of it is a file.

namespace KRG::FileSystem
{
    class KRG_SYSTEM_CORE_API Path
    {

    public:

        // Platform specific path delimiter
        static char const PathDelimiter;

        // Platform specific path conversion
        static String GetFullPathString( char const* pPath );

    public:

        Path() = default;
        Path( Path&& path ) { *this = std::move( path ); }
        Path( Path const& path ) : m_fullpath( path.m_fullpath ), m_hashCode( path.m_hashCode ) {}
        Path( String&& path );
        Path( String const& path );
        Path( char const* pPath );

        Path& operator=( Path& rhs );
        Path& operator=( Path const& rhs );
        Path& operator=( Path&& rhs );

        inline bool IsValid() const { return !m_fullpath.empty(); }
        inline size_t Length() const { return m_fullpath.length(); }
        inline String const& GetFullPath() const { return m_fullpath; }

        // Basic Operations
        //-------------------------------------------------------------------------

        Path& Append( char const* pPathString );
        inline Path& operator+=( char const* pPathString ) { return Append( pPathString ); }
        inline Path operator+( char const* pPathString ) const { return Path( m_fullpath ).Append( pPathString ); }

        inline Path& Append( String const& pathString ) { return Append( pathString.c_str() ); }
        inline Path& operator+=( String const& pathString ) { return Append( pathString.c_str() ); }
        inline Path operator+( String const& pathString ) const { return Path( m_fullpath ).Append( pathString.c_str() ); }

        // Get the directory depth for this path e.g. D:\Foo\Bar\Moo.txt -> 2
        int32 GetPathDepth() const;

        // Splits the path into multiple strings e.g. D:\Foo\Bar\Moo.txt -> { D:, Foo, Bar, Moo.txt }
        TInlineVector<String, 10> Split() const;

        // Splits the path into multiple paths starting from the parent and going to the root e.g. D:\Foo\Bar\Moo.txt -> { D:\Foo\Bar, D:\Foo, D:\ }
        TInlineVector<Path, 10> GetDirectoryHierarchy() const;

        // File System Functions
        //-------------------------------------------------------------------------
        // These functions actually check the filesystem

        bool Exists() const;
        bool IsExistingDirectory() const;
        bool IsExistingFile() const;

        // Files
        //-------------------------------------------------------------------------
        // These functions just operate on the string path and infer information from it, they dont check the actual file system

        inline String GetFileName() const { return String( GetFileNameSubstr() ); }
        bool IsFilenameEqual( char const* pString ) const;
        inline bool IsFilenameEqual( String const& pString ) const { KRG_ASSERT( !pString.empty() ); return IsFilenameEqual( pString.c_str() ); }

        String GetFileNameWithoutExtension() const;

        // This will return true for all paths not ending in the delimiter and with an extension
        inline bool IsFilePath() const { KRG_ASSERT( IsValid() ); return m_fullpath[m_fullpath.length() - 1] != PathDelimiter; }

        // Extensions
        //-------------------------------------------------------------------------
        // Extensions dont include the "."

        bool HasExtension() const;

        bool MatchesExtension( char const* inExtension ) const;
        inline bool MatchesExtension( String const& inExtension ) const { return MatchesExtension( inExtension.c_str() ); }

        char const* GetExtension() const;
        inline InlineString<6> GetExtensionAsString() const { return InlineString<6>( GetExtension() ); }
        
        inline InlineString<6> GetLowercaseExtensionAsString() const
        {
            auto ext = InlineString<6>( GetExtension() );
            ext.make_lower();
            return ext;
        }

        void ReplaceExtension( const char* pExtension );
        inline void ReplaceExtension( String const& extension ) { ReplaceExtension( extension.c_str() ); }
        template<size_t S> void ReplaceExtension( InlineString<S> const& extension ) { ReplaceExtension( extension.c_str() ); }

        // Directories
        //-------------------------------------------------------------------------
        // These functions just operate on the string path and infer information from it, they dont check the actual file system

        Path GetParentDirectory() const;
        String GetDirectoryName() const;
        bool IsUnderDirectory( Path parentDirectory ) const;

        // This will return true for all paths that either end with the delimiter or have no extension
        bool IsDirectoryPath() const;

        // Conversion
        //-------------------------------------------------------------------------

        inline operator char const*( ) const { return m_fullpath.c_str(); }
        inline String const& ToString() const { return m_fullpath; }
        inline String const& GetString() const { return m_fullpath; }
        inline char const* c_str() const { return m_fullpath.c_str(); }

        // Comparison
        //-------------------------------------------------------------------------

        inline uint32 GetHashCode() const { return m_hashCode; }
        inline bool operator==( Path const& RHS ) const { return m_hashCode == RHS.m_hashCode; }
        inline bool operator!=( Path const& RHS ) const { return m_hashCode != RHS.m_hashCode; }

    private:

        char const* GetFileNameSubstr() const;

    private:

        String      m_fullpath;
        uint32      m_hashCode;
    };
}

//-------------------------------------------------------------------------

namespace eastl
{
    template <>
    struct hash<KRG::FileSystem::Path>
    {
        eastl_size_t operator()( KRG::FileSystem::Path const& path ) const { return path.GetHashCode(); }
    };
}