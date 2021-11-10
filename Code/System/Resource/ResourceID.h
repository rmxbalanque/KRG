#pragma once

#include "ResourceTypeID.h"
#include "ResourcePath.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------
// Unique ID for a resource - Used for resource look up and dependencies
//-------------------------------------------------------------------------
// Resource ID's are the same as data paths except we extract the resource type from the extension
// e.g. data://directory/someResource.mesh ->  ResourceTypeID = 'mesh'
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_RESOURCE_API ResourceID
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
            OnPathChanged();
        }

    public:

        static bool IsValidResourceIDString( char const* pStr );
        inline static bool IsValidResourceIDString( String const& str ) { return IsValidResourceIDString( str.c_str() ); }
        inline static ResourceID FromFileSystemPath( FileSystem::Path const& rawResourceDirectoryPath, FileSystem::Path const& filePath ) { return ResourceID( ResourcePath::FromFileSystemPath( rawResourceDirectoryPath, filePath ) ); }

    public:

        ResourceID() = default;
        ResourceID( ResourcePath const& path ) : m_path( path ) { KRG_ASSERT( m_path.IsValid() ); OnPathChanged(); }
        ResourceID( ResourcePath&& path ) : m_path( std::move( path ) ) { KRG_ASSERT( m_path.IsValid() ); OnPathChanged(); }
        inline ResourceID( String&& path ) : m_path( path ) { OnPathChanged(); }
        inline ResourceID( String const& path ) : m_path( path ) { OnPathChanged(); }
        inline ResourceID( char const* pPath ) : m_path( pPath ) { OnPathChanged(); }

        inline bool IsValid() const { return m_path.IsValid(); }
        inline uint32 GetID() const { return m_path.GetID(); }
        inline ResourcePath const& GetResourcePath() const { return m_path; }
        inline ResourceTypeID GetResourceTypeID() const { return m_type; }

        //-------------------------------------------------------------------------

        inline String const& ToString() const { return m_path.GetString(); }
        inline char const* c_str() const { return m_path.c_str(); }

        //-------------------------------------------------------------------------

        inline bool operator==( ResourceID const& rhs ) const { return m_path == rhs.m_path; }
        inline bool operator!=( ResourceID const& rhs ) const { return m_path != rhs.m_path; }

        inline bool operator==( uint32 const& ID ) const { return m_path.GetID() == ID; }
        inline bool operator!=( uint32 const& ID ) const { return m_path.GetID() != ID; }

    private:

        void OnPathChanged();

    private:

        ResourcePath            m_path;
        ResourceTypeID          m_type = ResourceTypeID::Unknown;
    };
}

//-------------------------------------------------------------------------
// Support for THashMap

namespace eastl
{
    template <>
    struct hash<KRG::ResourceID>
    {
        eastl_size_t operator()( KRG::ResourceID const& ID ) const
        {
            return ( KRG::uint32 ) ID.GetID();
        }
    };
}