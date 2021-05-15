#pragma once

#include "../_Module/API.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API Percentage
    {
        KRG_SERIALIZE_MEMBERS( m_value );

    public:

        static Percentage Clamp( Percentage value, bool allowLooping = true );

    public:

        Percentage() = default;

        inline Percentage( float percentage )
            : m_value( percentage )
        {}

        KRG_FORCE_INLINE operator float() const { return m_value; }
        KRG_FORCE_INLINE float ToFloat() const { return m_value; }
        KRG_FORCE_INLINE int32 GetLoopCount() const { return (int32) Math::Floor( m_value ); }

        KRG_FORCE_INLINE void Invert() { m_value = 1.0f - m_value; }
        KRG_FORCE_INLINE Percentage GetInverse() const { return Percentage( 1.0f - m_value ); }

        KRG_FORCE_INLINE Percentage& Clamp( bool allowLooping = true ) { *this = Clamp( *this, allowLooping ); return *this; }
        KRG_FORCE_INLINE Percentage GetClamped( bool allowLooping = true ) const { return Clamp( *this, allowLooping ); }

        inline void GetLoopCountAndNormalizedTime( int32& loopCount, Percentage& normalizedValue ) const;
        inline Percentage GetNormalizedTime() const;

        KRG_FORCE_INLINE Percentage operator+( Percentage const f ) const { return Percentage( m_value + f.m_value ); }
        KRG_FORCE_INLINE Percentage operator-( Percentage const f ) const { return Percentage( m_value - f.m_value ); }
        KRG_FORCE_INLINE Percentage& operator+=( Percentage const f ) { m_value += f.m_value; return *this; }
        KRG_FORCE_INLINE Percentage& operator-=( Percentage const f ) { m_value -= f.m_value; return *this; }
        KRG_FORCE_INLINE Percentage operator*( Percentage const f ) const { return Percentage( m_value * f.m_value ); }
        KRG_FORCE_INLINE Percentage operator/( Percentage const f ) const { return Percentage( m_value / f.m_value ); }
        KRG_FORCE_INLINE Percentage& operator*=( Percentage const f ) { m_value *= f.m_value; return *this; }
        KRG_FORCE_INLINE Percentage& operator/=( Percentage const f ) { m_value /= f.m_value; return *this; }

        KRG_FORCE_INLINE Percentage operator+( float const f ) const { return Percentage( m_value + f ); }
        KRG_FORCE_INLINE Percentage operator-( float const f ) const { return Percentage( m_value - f ); }
        KRG_FORCE_INLINE Percentage& operator+=( float const f ) { m_value += f; return *this; }
        KRG_FORCE_INLINE Percentage& operator-=( float const f ) { m_value -= f; return *this; }
        KRG_FORCE_INLINE Percentage operator*( float const f ) const { return Percentage( m_value * f ); }
        KRG_FORCE_INLINE Percentage operator/ ( float const f ) const { return Percentage( m_value / f ); }
        KRG_FORCE_INLINE Percentage& operator*=( float const f ) { m_value *= f; return *this; }
        KRG_FORCE_INLINE Percentage& operator/=( float const f ) { m_value /= f; return *this; }

    private:

        float m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    inline void Percentage::GetLoopCountAndNormalizedTime( int32& loopCount, Percentage& normalizedValue ) const
    {
        float integerPortion;
        normalizedValue = Percentage( Math::ModF( m_value, &integerPortion ) );
        loopCount = (int32) integerPortion;
    }

    inline Percentage Percentage::GetNormalizedTime() const
    {
        float loopCount;
        float normalizedValue = Math::ModF( m_value, &loopCount );
        if ( loopCount > 0 && normalizedValue == 0.0f )
        {
            normalizedValue = 1.0f;
        }

        return Percentage( normalizedValue );
    }
}