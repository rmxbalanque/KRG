#pragma once

#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Core/Defines.h"
#include "System/Core/Serialization/Serialization.h"
#include <type_traits>

//-------------------------------------------------------------------------
//  Bit Flags
//-------------------------------------------------------------------------
// Generic flag flags type

namespace KRG
{
    class BitFlags
    {
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_flags ) );

    public:

        constexpr static uint8 const MaxFlags = 32;
        KRG_FORCE_INLINE static uint32 GetFlagMask( uint8 flag ) { return (uint32) ( 1u << flag ); }

    public:

        inline BitFlags() = default;
        inline explicit BitFlags( uint32 flags ) : m_flags( flags ) {}

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE uint32 Get() const { return m_flags; }
        KRG_FORCE_INLINE void Set( uint32 flags ) { m_flags = flags; }
        inline operator uint32() const { return m_flags; }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE bool IsFlagSet( uint8 flag ) const
        {
            KRG_ASSERT( flag < MaxFlags );
            return ( m_flags & GetFlagMask( flag ) ) > 0;
        }

        KRG_FORCE_INLINE void SetFlag( uint8 flag )
        {
            KRG_ASSERT( flag >= 0 && flag < MaxFlags );
            m_flags |= GetFlagMask( flag );
        }

        KRG_FORCE_INLINE void SetFlag( uint8 flag, bool value )
        {
            KRG_ASSERT( flag < MaxFlags );
            value ? SetFlag( flag ) : ClearFlag( flag );
        }

        KRG_FORCE_INLINE void SetAllFlags()
        {
            m_flags = 0xFFFFFFFF;
        }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE bool IsFlagCleared( uint8 flag ) const
        {
            KRG_ASSERT( flag < MaxFlags );
            return ( m_flags & GetFlagMask( flag ) ) == 0;
        }

        KRG_FORCE_INLINE void ClearFlag( uint8 flag )
        {
            KRG_ASSERT( flag < MaxFlags );
            m_flags &= ~GetFlagMask( flag );
        }

        KRG_FORCE_INLINE void ClearAllFlags()
        {
            m_flags = 0;
        }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE void FlipFlag( uint8 flag )
        {
            KRG_ASSERT( flag >= 0 && flag < MaxFlags );
            m_flags ^= GetFlagMask( flag );
        }

        KRG_FORCE_INLINE void FlipAllFlags()
        {
            m_flags = ~m_flags;
        }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE BitFlags& operator| ( uint8 flag )
        {
            KRG_ASSERT( flag < MaxFlags );
            m_flags |= GetFlagMask( flag );
            return *this;
        }

        KRG_FORCE_INLINE BitFlags& operator& ( uint8 flag )
        {
            KRG_ASSERT( flag < MaxFlags );
            m_flags &= GetFlagMask( flag );
            return *this;
        }

    protected:

        uint32 m_flags = 0;
    };
}

//-------------------------------------------------------------------------
//  Templatized Bit Flags
//-------------------------------------------------------------------------
// Helper to create flag flags variables from a specific enum type

namespace KRG
{
    template<typename T>
    class TBitFlags : public BitFlags
    {
        static_assert( std::is_enum<T>::value, "TBitFlags only supports enum types" );

    public:

        using BitFlags::BitFlags;
        inline TBitFlags( T value ) 
            : BitFlags( GetFlagMask( (uint8) value ) )
        {
            KRG_ASSERT( (uint32) value < MaxFlags );
        }

        template<typename... Args>
        TBitFlags( Args&&... args )
        {
            ( ( m_flags |= 1u << (uint8) args ), ... );
        }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE bool IsFlagSet( T flag ) const { return BitFlags::IsFlagSet( (uint8) flag ); }
        KRG_FORCE_INLINE bool IsFlagCleared( T flag ) const { return BitFlags::IsFlagCleared( (uint8) flag ); }
        KRG_FORCE_INLINE void SetFlag( T flag ) { BitFlags::SetFlag( (uint8) flag ); }
        KRG_FORCE_INLINE void SetFlag( T flag, bool value ) { BitFlags::SetFlag( (uint8) flag, value ); }
        KRG_FORCE_INLINE void FlipFlag( T flag ) { BitFlags::FlipFlag( (uint8) flag ); }
        KRG_FORCE_INLINE void ClearFlag( T flag ) { BitFlags::ClearFlag( (uint8) flag ); }

        //-------------------------------------------------------------------------

        template<typename... Args>
        inline void SetMultipleFlags( Args&&... args )
        {
            ( ( m_flags |= 1u << (uint8) args ), ... );
        }

        template<typename... Args>
        inline bool AreAnyFlagsSet( Args&&... args ) const
        {
            uint32 mask = 0;
            ( ( mask |= 1u << (uint8) args ), ... );
            return ( m_flags & mask ) != 0;
        }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE TBitFlags& operator| ( T flag )
        {
            KRG_ASSERT( (uint8) flag < MaxFlags );
            m_flags |= GetFlagMask( flag );
            return *this;
        }

        KRG_FORCE_INLINE TBitFlags& operator& ( T flag )
        {
            KRG_ASSERT( (uint8) flag < MaxFlags );
            m_flags &= GetFlagMask( flag );
            return *this;
        }
    };

    //-------------------------------------------------------------------------

    static_assert( sizeof( TBitFlags<enum class Temp> ) == sizeof( BitFlags ), "TBitFlags is purely syntactic sugar for easy conversion of enums to flags. It must not contain any members!" );
}
