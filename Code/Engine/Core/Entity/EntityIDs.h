#pragma once
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct PointerID
    {
        KRG_SERIALIZE_MEMBERS( m_ID );

        PointerID() = default;
        PointerID( void const* pV ) : m_ID( reinterpret_cast<uint64>( pV ) ) {}
        explicit PointerID( uint64 v ) : m_ID( v ) {}

        inline bool IsValid() const { return m_ID != 0; }
        inline void Clear() { m_ID = 0; }
        inline bool operator==( PointerID const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( PointerID const& rhs ) const { return m_ID != rhs.m_ID; }

        inline uint64 ToUint64() const { return m_ID; }

        uint64 m_ID = 0;
    };

    using ComponentID = PointerID;
    using EntityID = PointerID;
    using EntityMapID = UUID;
    using EntityWorldID = UUID;
}

//-------------------------------------------------------------------------

namespace eastl
{
    template <>
    struct hash<KRG::PointerID>
    {
        KRG_FORCE_INLINE eastl_size_t operator()( KRG::PointerID const& t ) const { return t.m_ID; }
    };
}