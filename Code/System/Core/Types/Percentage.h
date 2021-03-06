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

        inline explicit Percentage( float percentage )
            : m_value( percentage )
        {}

        inline operator float() const { return m_value; }
        inline float ToFloat() const { return m_value; }
        inline int32 GetLoopCount() const { return (int32) Math::Floor( m_value ); }

        inline void Invert() { m_value = 1.0f - m_value; }
        inline Percentage GetInverse() const { return Percentage( 1.0f - m_value ); }

        inline Percentage& Clamp( bool allowLooping = true ) { *this = Clamp( *this, allowLooping ); return *this; }
        inline Percentage GetClamped( bool allowLooping = true ) const { return Clamp( *this, allowLooping ); }

        inline void GetLoopCountAndNormalizedTime( int32& loopCount, Percentage& normalizedValue ) const;
        inline Percentage GetNormalizedTime() const;

        inline Percentage operator+( Percentage const f ) const { return Percentage( m_value + f.m_value ); }
        inline Percentage operator-( Percentage const f ) const { return Percentage( m_value - f.m_value ); }
        inline Percentage& operator+=( Percentage const f ) { m_value += f.m_value; return *this; }
        inline Percentage& operator-=( Percentage const f ) { m_value -= f.m_value; return *this; }
        inline Percentage operator*( Percentage const f ) const { return Percentage( m_value * f.m_value ); }
        inline Percentage operator/( Percentage const f ) const { return Percentage( m_value / f.m_value ); }
        inline Percentage& operator*=( Percentage const f ) { m_value *= f.m_value; return *this; }
        inline Percentage& operator/=( Percentage const f ) { m_value /= f.m_value; return *this; }

        inline Percentage operator+( float const f ) const { return Percentage( m_value + f ); }
        inline Percentage operator-( float const f ) const { return Percentage( m_value - f ); }
        inline Percentage& operator+=( float const f ) { m_value += f; return *this; }
        inline Percentage& operator-=( float const f ) { m_value -= f; return *this; }
        inline Percentage operator*( float const f ) const { return Percentage( m_value * f ); }
        inline Percentage operator/ ( float const f ) const { return Percentage( m_value / f ); }
        inline Percentage& operator*=( float const f ) { m_value *= f; return *this; }
        inline Percentage& operator/=( float const f ) { m_value /= f; return *this; }

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