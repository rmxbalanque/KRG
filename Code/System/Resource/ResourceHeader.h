#pragma once

#include "ResourceID.h"
#include "System/Core/Serialization/Serialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        // Describes the contents of a resource, every resource has a header
        struct ResourceHeader
        {
            KRG_SERIALIZE_MEMBERS( KRG_NVP( m_version ), KRG_NVP( m_resourceType ), KRG_NVP( m_installDependencies ) );

        public:

            ResourceHeader()
                : m_version( -1 )
            {}

            ResourceHeader( S32 version, ResourceTypeID type )
                : m_version( version )
                , m_resourceType( type )
            {}

            ResourceTypeID GetResourceTypeID() const { return m_resourceType; }
            void AddInstallDependency( ResourceID resourceID ) { m_installDependencies.push_back( resourceID ); }

        public:

            S32                     m_version;
            ResourceTypeID          m_resourceType;
            TVector<ResourceID>     m_installDependencies;
        };
    }
}