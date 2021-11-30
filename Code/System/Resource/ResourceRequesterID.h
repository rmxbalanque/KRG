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
            : m_ID( resourceID.GetResourcePath().GetID() )
            , m_isInstallDependency( true )
        {}

        // Explicit ID - generally refers to an entity ID
        explicit ResourceRequesterID( uint64 ID )
            : m_ID( ID )
        {
            KRG_ASSERT( ID > 0 );
        }

        //-------------------------------------------------------------------------

        // This ID refers to a manual request outside of the default resource loading flow (usually only used for resources like maps)
        inline bool IsManualRequest() const
        {
            return m_ID == 0;
        }

        // A normal request via the entity system
        inline bool IsNormalRequest() const
        {
            return m_ID > 0 && !m_isInstallDependency;
        }

        // A install dependency request, coming from the resource system as part of resource loading
        inline bool IsInstallDependencyRequest() const
        {
            return m_isInstallDependency;
        }

        //-------------------------------------------------------------------------

        // Get the requester ID
        inline uint64 GetID() const { return m_ID; }

        // Get the ID for the data path for install dependencies, used for reverse look ups
        inline uint32 GetInstallDependencyResourcePathID() const
        {
            KRG_ASSERT( m_isInstallDependency );
            return (uint32) m_ID;
        }

        //-------------------------------------------------------------------------

        inline bool operator==( ResourceRequesterID const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( ResourceRequesterID const& rhs ) const { return m_ID != rhs.m_ID; }

    private:

        uint64  m_ID = 0;
        bool    m_isInstallDependency = false;
    };
}