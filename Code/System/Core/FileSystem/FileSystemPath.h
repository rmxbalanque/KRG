#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// File System Path
//-------------------------------------------------------------------------
// A helper to abstract and perform some file system path operations
// Not the smartest thing in the world, and has some limitations made for performance
//
// For directory paths, the only robust methods to distinguish between a file and a directory path is to end with a path delimiter, 
// So any path that do not end in the path delimiter are assumed to be files.

namespace KRG::FileSystem
{
    class KRG_SYSTEM_CORE_API Path
    {
        friend KRG_SYSTEM_CORE_API void EnsureCorrectPathStringFormat( Path& filePath );

    public:

        // Platform specific path delimiter
        static char const s_pathDelimiter;

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
        inline void Clear() { m_fullpath.clear(); m_hashCode = 0; }

        // Basic Operations
        //-------------------------------------------------------------------------

        // Append a string to the end of the path. Optional specify if we should add a path delimiter at the end of the appended string
        Path& Append( char const* pPathString, bool asDirectory = false );

        // Append a string to the end of the path. Optional specify if we should add a path delimiter at the end of the appended string
        inline Path& Append( String const& pathString, bool asDirectory = false ) { return Append( pathString.c_str(), asDirectory ); }

        inline Path& operator+=( char const* pPathString ) { return Append( pPathString ); }
        inline Path operator+( char const* pPathString ) const { return Path( m_fullpath ).Append( pPathString ); }
        inline Path& operator+=( String const& pathString ) { return Append( pathString.c_str() ); }
        inline Path operator+( String const& pathString ) const { return Path( m_fullpath ).Append( pathString.c_str() ); }

        // Get the directory depth for this path e.g. D:\Foo\Bar\Moo.txt = 2
        int32 GetDirectoryDepth() const;

        // Get the full path depth for this path e.g. D:\Foo\Bar\Moo.txt = 3
        int32 GetPathDepth() const;

        // Splits the path into multiple strings e.g. D:\Foo\Bar\Moo.txt -> { D:, Foo, Bar, Moo.txt }
        TInlineVector<String, 10> Split() const;

        // Splits the path into multiple paths starting from the parent and going to the root e.g. D:\Foo\Bar\Moo.txt -> { D:\Foo\Bar, D:\Foo, D:\ }
        TInlineVector<Path, 10> GetDirectoryHierarchy() const;

        // Files
        //-------------------------------------------------------------------------
        // NOTE! These functions just operate on the string path and infer information from it, they dont check the actual file system

        // This will return true if the path string doesnt end with a path delimiter
        inline bool IsFile() const { KRG_ASSERT( IsValid() ); return m_fullpath[m_fullpath.length() - 1] != s_pathDelimiter; }

        inline String GetFileName() const { return String( GetFileNameSubstr() ); }
        bool IsFilenameEqual( char const* pString ) const;
        inline bool IsFilenameEqual( String const& pString ) const { KRG_ASSERT( !pString.empty() ); return IsFilenameEqual( pString.c_str() ); }

        String GetFileNameWithoutExtension() const;

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
        // NOTE! These functions just operate on the string path and infer information from it, they dont check the actual file system

        // This will return true if the path string ends with a path delimiter
        inline bool IsDirectory() const { KRG_ASSERT( IsValid() ); return ( m_fullpath[m_fullpath.length() - 1] == s_pathDelimiter ); }

        // This will ensure that this path ends in a path delimiter
        void MakeDirectory();

        Path GetParentDirectory() const;
        String GetDirectoryName() const;
        bool IsUnderDirectory( Path const& parentDirectory ) const;
        void ReplaceParentDirectory( Path const& newParentDirectory );

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
        uint32      m_hashCode = 0;
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