#pragma once
#include "_Module/API.h"
#include "PropertyPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        struct KRG_SYSTEM_TYPESYSTEM_API TypeSerializationSettings
        {
            static char const* const TypeKey;
        };

        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_TYPESYSTEM_API SerializedPropertyValue
        {
            KRG_SERIALIZE_MEMBERS( m_path, m_valueData );

            SerializedPropertyValue() = default;

            SerializedPropertyValue( TypeSystem::PropertyPath const& path, TVector<Byte> const& data )
                : m_path( path )
                , m_valueData( data )
            {
                KRG_ASSERT( m_path.IsValid() && data.size() > 0 );
            }

        public:

            TypeSystem::PropertyPath                                    m_path;
            TVector<Byte>                                               m_valueData;
        };
    }
}