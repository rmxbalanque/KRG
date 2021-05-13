#pragma once
#include "ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceRequesterID
    {
    public:

        // No ID - manual request
        ResourceRequesterID() = default;

        // Install dependency reference
        ResourceRequesterID( ResourceID const& resourceID )
            : m_ID( UUID( 0, 0, 0, resourceID.GetDataPath().GetID() ) )
        {}

        // Explicit ID - generally refers to an entity
        ResourceRequesterID( UUID ID )
            : m_ID( ID )
        {
            KRG_ASSERT( ID.IsValid() );
        }

        //-------------------------------------------------------------------------

        // This ID refers to a manual request outside of the default resource loading flow (usually only used for resources like maps)
        inline bool IsManualRequest() const
        {
            return m_ID.GetValueU64( 0 ) == 0 && m_ID.GetValueU64( 1 ) == 0;
        }

        // A normal request via the entity system
        inline bool IsNormalRequest() const
        {
            return ( m_ID.GetValueU32( 3 ) != 0 ) && ( m_ID.GetValueU64( 0 ) != 0 || m_ID.GetValueU32( 2 ) != 0 );
        }

        // A install dependency request, coming from the resource system as part of resource loading
        inline bool IsInstallDependencyRequest() const
        {
            return m_ID.GetValueU64( 0 ) == 0 && m_ID.GetValueU32( 2 ) == 0 && m_ID.GetValueU32( 3 ) != 0;
        }

        //-------------------------------------------------------------------------

        // Get the requester ID
        inline UUID GetID() const { return m_ID; }

        // Get the ID for the data path for install dependencies, used for reverse look ups
        inline uint32 GetInstallDependencyDataPathID() const
        {
            KRG_ASSERT( IsInstallDependencyRequest() );
            return m_ID.GetValueU32( 3 );
        }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE UUIDString ToString() const { return m_ID.ToString(); }

        //-------------------------------------------------------------------------

        inline bool operator==( ResourceRequesterID const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( ResourceRequesterID const& rhs ) const { return m_ID != rhs.m_ID; }

    private:

        UUID m_ID;
    };
}