#pragma once
#include "System/Resource/ResourceID.h"
#include "System/Core/Serialization/Serialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        enum class NetworkMessageID
        {
            RequestResource = 1,
            ResourceRequestComplete = 2,
            ResourceUpdated = 3,
        };

        //-------------------------------------------------------------------------

        struct NetworkResourceRequest
        {
            KRG_SERIALIZE_MEMBERS( KRG_NVP( m_path ) );

            NetworkResourceRequest() = default;

            NetworkResourceRequest( ResourceID const& ID )
                : m_path( ID )
            {}

            ResourceID            m_path;
        };

        //-------------------------------------------------------------------------

        struct NetworkResourceResponse
        {
            KRG_SERIALIZE_MEMBERS( KRG_NVP( m_resourceID ), KRG_NVP( m_filePath ) );

            ResourceID          m_resourceID;
            String              m_filePath;
        };
    }
}