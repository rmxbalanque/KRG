#pragma once

#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Core/Defines.h"
#include "System/Core/Serialization/Serialization.h"
#include <type_traits>

//-------------------------------------------------------------------------

namespace KRG
{
    // Helper to create flag variables from enum types
    template<typename T>
    class TBitFlags
    {
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_flags ) );

        static_assert( std::is_enum<T>::value, "Flags only support enum types" );
        static_assert( sizeof( T ) <= 4, "Max number of support flags is 32" );

    public:

        inline static uint32 GetFlagMask( T flag ) { return (uint32) ( 1u << (uint32) flag ); }

    public:

        inline TBitFlags() = default;
        inline explicit TBitFlags( T value ) : m_flags( GetFlagMask( value ) ) {}
        inline explicit TBitFlags( uint32 flags ) : m_flags( flags ) {}

        //-------------------------------------------------------------------------

        inline bool IsFlagSet( T flag ) const { return ( m_flags & GetFlagMask( flag ) ) > 0; }
        inline void SetFlag( T flag, bool value ) { value ? SetFlag( flag ) : ClearFlag( flag ); }
        inline void SetFlag( T flag ) { m_flags |= GetFlagMask( flag ); }
        inline void ClearFlag( T flag ) { m_flags &= ~GetFlagMask( flag ); }
        inline void SetAll( uint32 flags ) { m_flags = flags; }
        inline void SetAll() { m_flags = 0xFFFFFFFF; }
        inline void ClearAll() { m_flags = 0; }

        //-------------------------------------------------------------------------

        inline TBitFlags& operator| ( T flag )
        {
            m_flags |= GetFlagMask( flag );
            return *this;
        }

        inline TBitFlags& operator& ( T flag )
        {
            m_flags &= GetFlagMask( flag );
            return *this;
        }

        inline operator uint32() const { return m_flags; }

    protected:

        uint32 m_flags = 0;
    };
}
