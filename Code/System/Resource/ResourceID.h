#pragma once

#include "ResourceTypeID.h"
#include "System/Core/FileSystem/DataPath.h"
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
            ar( m_dataPath );
        }

        template <class Archive>
        void load( Archive& ar )
        {
            ar( m_dataPath );
            OnDataPathChanged();
        }

    public:

        static bool IsValidResourceIDString( char const* pStr );
        inline static bool IsValidResourceIDString( String const& str ) { return IsValidResourceIDString( str.c_str() ); }

    public:

        ResourceID() = default;
        ResourceID( DataPath const& path ) : m_dataPath( path ) { KRG_ASSERT( path.IsValid() ); OnDataPathChanged(); }
        ResourceID( DataPath&& path ) : m_dataPath( std::forward<DataPath>( path ) ) { KRG_ASSERT( path.IsValid() ); OnDataPathChanged(); }
        inline ResourceID( String&& path ) : m_dataPath( path ) { OnDataPathChanged(); }
        inline ResourceID( String const& path ) : m_dataPath( path ) { OnDataPathChanged(); }
        inline ResourceID( char const* pPath ) : m_dataPath( pPath ) { OnDataPathChanged(); }

        inline bool IsValid() const { return m_dataPath.IsValid(); }
        inline uint32 GetID() const { return m_dataPath.GetID(); }
        inline DataPath const& GetDataPath() const { return m_dataPath; }
        inline ResourceTypeID GetResourceTypeID() const { return m_type; }

        //-------------------------------------------------------------------------

        inline String const& ToString() const { return m_dataPath.ToString(); }
        inline char const* c_str() const { return m_dataPath.c_str(); }

        //-------------------------------------------------------------------------

        inline bool operator==( ResourceID const& rhs ) const { return m_dataPath == rhs.m_dataPath; }
        inline bool operator!=( ResourceID const& rhs ) const { return m_dataPath != rhs.m_dataPath; }

        inline bool operator==( uint32 const& ID ) const { return m_dataPath.GetID() == ID; }
        inline bool operator!=( uint32 const& ID ) const { return m_dataPath.GetID() != ID; }

    private:

        void OnDataPathChanged();

    private:

        DataPath            m_dataPath;
        ResourceTypeID      m_type = ResourceTypeID::Unknown;
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