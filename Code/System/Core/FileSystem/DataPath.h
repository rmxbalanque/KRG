#pragma once

#include "../_Module/API.h"
#include "FileSystemPath.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// Path relative to the currently set source data directory
//-------------------------------------------------------------------------
// Note: data paths are ALWAYS lowercase
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API DataPath
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

        static char const* PathPrefix;
        static constexpr char const PathDelimiter = '/';

        static bool IsValidDataPath( char const* pPath );
        static bool IsValidDataPath( String const& path ) { return IsValidDataPath( path.c_str() ); }

        static DataPath FromFileSystemPath( FileSystem::Path const& sourceDataPath, FileSystem::Path const& filePath );
        static FileSystem::Path ToFileSystemPath( FileSystem::Path const& sourceDataPath, DataPath const& dataPath );

    public:

        DataPath() = default;
        DataPath( String&& path );
        DataPath( String const& path );
        DataPath( char const* pPath );

        // Accessors
        inline bool IsValid() const { return !m_path.empty() && IsValidDataPath( m_path ); }
        inline void Clear() { m_path.clear(); m_ID = 0; }
        inline uint32 GetID() const { return m_ID; }

        // Extension
        String GetExtension() const;
        void ReplaceExtension( const char* pExtension );
        inline void ReplaceExtension( String const& extension ) { ReplaceExtension( extension.c_str() ); }
        template<size_t S> void ReplaceExtension( InlineString<S> const& extension ) { ReplaceExtension( extension.c_str() ); }

        // Conversion
        inline String const& ToString() const { return m_path; }
        inline char const* c_str() const { return m_path.c_str(); }
        inline FileSystem::Path ToFileSystemPath( FileSystem::Path const& sourceDataPath ) const { return ToFileSystemPath( sourceDataPath, *this ); }

        // Comparison
        inline bool operator==( DataPath const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( DataPath const& rhs ) const { return m_ID != rhs.m_ID; }

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
    struct hash<KRG::DataPath>
    {
        eastl_size_t operator()( KRG::DataPath const& ID ) const
        {
            return ID.GetID();
        }
    };
}