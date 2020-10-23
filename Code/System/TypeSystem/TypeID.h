#pragma once

#include "_Module/API.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Serialization/Serialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        class KRG_SYSTEM_TYPESYSTEM_API TypeID
        {
            KRG_SERIALIZE_MEMBERS( KRG_NVP( m_ID ) );

        public:

            TypeID() {}
            TypeID( String const& type ) : m_ID( type ) {}
            TypeID( char const* pType ) : m_ID( pType ) {}
            TypeID( StringID ID ) : m_ID( ID ) {}
            TypeID( U32 ID ) : m_ID( ID ) {}

            inline bool IsValid() const { return m_ID.IsValid(); }

            inline operator U32() const { return m_ID.GetID(); }
            inline StringID GetAsStringID() const { return m_ID; }

        private:

            StringID m_ID;
        };
    }
}

//-------------------------------------------------------------------------

namespace eastl
{
    template <>
    struct hash<KRG::TypeSystem::TypeID>
    {
        eastl_size_t operator()( KRG::TypeSystem::TypeID const& ID ) const 
        {
            return (KRG::U32) ID; 
        }
    };
}