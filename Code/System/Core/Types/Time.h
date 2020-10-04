#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Serialization/Serialization.h"
#include "EASTL/chrono.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Forward Declarations
    //-------------------------------------------------------------------------

    class Ticks;
    class Milliseconds;
    class Seconds;

    //-------------------------------------------------------------------------
    // Data Types
    //-------------------------------------------------------------------------

    class Microseconds
    {
        friend Milliseconds;
        friend Seconds;
        KRG_SERIALIZE_MEMBERS( m_value );

    public:

        static inline Microseconds Now();

    public:

        Microseconds() : m_value(0) {}
        Microseconds( F32 time ) : m_value( time ) {}
        inline explicit Microseconds( Ticks time );
        inline explicit Microseconds( Milliseconds time );
        inline explicit Microseconds( Seconds time );

        inline Microseconds& operator= ( F32 time ) { m_value = time; return *this; }
        inline Microseconds& operator= ( Milliseconds time );
        inline Microseconds& operator= ( Seconds time );

        inline operator F32() const { return m_value; }
        inline F32 ToFloat() const { return m_value; }
        inline explicit operator Milliseconds() const;
        inline explicit operator Seconds() const;

        inline Milliseconds ToMilliseconds() const;
        inline Seconds ToSeconds() const;

        inline Microseconds operator+ ( F32 const time ) const;
        inline Microseconds operator+ ( Microseconds const time )  const;
        inline Microseconds operator+ ( Milliseconds const time ) const;
        inline Microseconds operator+ ( Seconds const time )  const;

        inline Microseconds operator- ( F32 const time )  const;
        inline Microseconds operator- ( Microseconds const time ) const;
        inline Microseconds operator- ( Milliseconds const time ) const;
        inline Microseconds operator- ( Seconds const time ) const;

        inline Microseconds& operator+= ( F32 const time );
        inline Microseconds& operator+= ( Microseconds const time );
        inline Microseconds& operator+= ( Milliseconds const time );
        inline Microseconds& operator+= ( Seconds const time );

        inline Microseconds& operator-= ( F32 const time );
        inline Microseconds& operator-= ( Microseconds const time );
        inline Microseconds& operator-= ( Milliseconds const time );
        inline Microseconds& operator-= ( Seconds const time );

    private:

        F32 m_value;
    };

    //-------------------------------------------------------------------------

    class Milliseconds
    {
        friend Microseconds;
        friend Seconds;
        KRG_SERIALIZE_MEMBERS( m_value );

    public:

        static inline Milliseconds Now();

    public:

        Milliseconds() : m_value( 0 ) {}
        Milliseconds( F32 time ) : m_value( time ) {}
        inline explicit Milliseconds( Ticks time );
        inline explicit Milliseconds( Microseconds time );
        inline explicit Milliseconds( Seconds time );

        inline Milliseconds& operator= ( F32 time ) { m_value = time; return *this; }
        inline Milliseconds& operator= ( Microseconds time );
        inline Milliseconds& operator= ( Seconds time );

        inline operator F32() const { return m_value; }
        inline F32 ToFloat() const { return m_value; }
        inline explicit operator Microseconds() const;
        inline explicit operator Seconds() const;

        inline Microseconds ToMicroseconds() const;
        inline Seconds ToSeconds() const;

        inline Milliseconds operator+ ( F32 const time ) const;
        inline Milliseconds operator+ ( Milliseconds const time ) const;
        inline Milliseconds operator+ ( Microseconds const time ) const;
        inline Milliseconds operator+ ( Seconds const time ) const;

        inline Milliseconds operator- ( F32 const time ) const;
        inline Milliseconds operator- ( Milliseconds const time ) const;
        inline Milliseconds operator- ( Microseconds const time ) const;
        inline Milliseconds operator- ( Seconds const time ) const;

        inline Milliseconds& operator+= ( F32 const time );
        inline Milliseconds& operator+= ( Milliseconds const time );
        inline Milliseconds& operator+= ( Microseconds const time );
        inline Milliseconds& operator+= ( Seconds const time );

        inline Milliseconds& operator-= ( F32 const time );
        inline Milliseconds& operator-= ( Milliseconds const time );
        inline Milliseconds& operator-= ( Microseconds const time );
        inline Milliseconds& operator-= ( Seconds const time );

    private:

        F32 m_value;
    };

    //-------------------------------------------------------------------------

    class Seconds
    {
        friend Microseconds;
        friend Milliseconds;
        KRG_SERIALIZE_MEMBERS( m_value );

    public:

        static inline Seconds Now();

    public:

        Seconds() : m_value( 0 ) {}
        Seconds( F32 time ) : m_value( time ) {}
        inline explicit Seconds( Ticks time );
        inline explicit Seconds( Microseconds time );
        inline explicit Seconds( Milliseconds time );

        inline Seconds& operator= ( F32 time ) { m_value = time; return *this; }
        inline Seconds& operator= ( Microseconds time );
        inline Seconds& operator= ( Milliseconds time );

        inline operator F32() const { return m_value; }
        inline F32 ToFloat() const { return m_value; }
        inline explicit operator Microseconds() const;
        inline explicit operator Milliseconds() const;

        inline Microseconds ToMicroseconds() const;
        inline Milliseconds ToMilliseconds() const;

        inline Seconds operator+ ( F32 const time ) const;
        inline Seconds operator+ ( Seconds const time ) const;
        inline Seconds operator+ ( Microseconds const time ) const;
        inline Seconds operator+ ( Milliseconds const time ) const;

        inline Seconds operator- ( F32 const time ) const;
        inline Seconds operator- ( Seconds const time ) const;
        inline Seconds operator- ( Microseconds const time ) const;
        inline Seconds operator- ( Milliseconds const time ) const;

        inline Seconds& operator+= ( F32 const time );
        inline Seconds& operator+= ( Seconds const time );
        inline Seconds& operator+= ( Microseconds const time );
        inline Seconds& operator+= ( Milliseconds const time );

        inline Seconds& operator-= ( F32 const time );
        inline Seconds& operator-= ( Seconds const time );
        inline Seconds& operator-= ( Microseconds const time );
        inline Seconds& operator-= ( Milliseconds const time );

        inline Seconds operator* ( F32 const time ) const;
        inline Seconds operator* ( Seconds const time ) const;
        inline Seconds operator* ( Microseconds const time ) const;
        inline Seconds operator* ( Milliseconds const time ) const;

        inline Seconds operator/ ( F32 const time ) const;
        inline Seconds operator/ ( Seconds const time ) const;
        inline Seconds operator/ ( Microseconds const time ) const;
        inline Seconds operator/ ( Milliseconds const time ) const;

        inline Seconds& operator*= ( F32 const time );
        inline Seconds& operator*= ( Seconds const time );
        inline Seconds& operator*= ( Microseconds const time );
        inline Seconds& operator*= ( Milliseconds const time );

        inline Seconds& operator/= ( F32 const time );
        inline Seconds& operator/= ( Seconds const time );
        inline Seconds& operator/= ( Microseconds const time );
        inline Seconds& operator/= ( Milliseconds const time );

    private:

        F32 m_value;
    };

    //-------------------------------------------------------------------------
    // System high resolution timing
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API Ticks
    {
        KRG_SERIALIZE_MEMBERS( m_value );

    public:

        static Ticks Now();

    public:

        Ticks() : m_value( 0 ) {}
        Ticks( U64 time ) : m_value( time ) {}
        explicit Ticks( Ticks const& time ) : m_value( time.m_value ) {}

        inline Ticks& operator= ( U64 time ) { m_value = time; return *this; }
        inline Ticks& operator= ( Ticks time ) { m_value = time.m_value; return *this; }

        inline operator U64() const { return m_value; }
        inline U64 ToU64() const { return m_value; }
        explicit operator Microseconds() const;
        inline explicit operator Milliseconds() const { return Milliseconds( operator Microseconds() ); }
        inline explicit operator Seconds() const { return Seconds( operator Microseconds() ); }

        inline Microseconds ToMicroseconds() const { return operator Microseconds(); }
        inline Milliseconds ToMilliseconds() const { return operator Milliseconds(); }
        inline Seconds ToSeconds() const { return operator Seconds(); }

        inline Ticks operator+ ( U64 const time ) const { return Ticks( m_value + time ); }
        inline Ticks operator+ ( Ticks const time ) const { return Ticks( m_value + time.m_value ); }

        inline Ticks operator- ( U64 const time ) const { return Ticks( m_value - time ); }
        inline Ticks operator- ( Ticks const time ) const { return Ticks( m_value - time.m_value ); }

        inline Ticks& operator+= ( U64 const time ) { m_value += time; return *this; };
        inline Ticks& operator+= ( Ticks const time ) { m_value += time.m_value; return *this; };

        inline Ticks& operator-= ( U64 const time ) { m_value -= time; return *this; }
        inline Ticks& operator-= ( Ticks const time ) { m_value -= time.m_value; return *this; }

    private:

        U64 m_value;
    };

    //-------------------------------------------------------------------------
    // Conversion functions
    //-------------------------------------------------------------------------

    inline Microseconds::Microseconds( Ticks time ) : m_value( time.ToMicroseconds() ) {}
    inline Microseconds::Microseconds( Milliseconds time ) : m_value( time.m_value * 1000 ) {}
    inline Microseconds::Microseconds( Seconds time ) : m_value( time.m_value * 1000000 ) {}
    inline Microseconds& Microseconds::operator= ( Milliseconds time ) { m_value = time.m_value * 1000; return *this; }
    inline Microseconds& Microseconds::operator= ( Seconds time ) { m_value = time.m_value * 1000000; return *this; }
    inline Microseconds::operator Milliseconds() const { return Milliseconds( m_value / 1000 ); }
    inline Microseconds::operator Seconds() const { return Seconds( m_value / 1000000 ); }
    inline Milliseconds Microseconds::ToMilliseconds() const { return operator Milliseconds(); }
    inline Seconds Microseconds::ToSeconds() const { return operator Seconds(); }

    //-------------------------------------------------------------------------

    inline Milliseconds::Milliseconds( Ticks time ) : m_value( time.ToMilliseconds() ) {}
    inline Milliseconds::Milliseconds( Microseconds time ) : m_value( time.m_value / 1000 ) {}
    inline Milliseconds::Milliseconds( Seconds time ) : m_value( time.m_value * 1000 ) {}
    inline Milliseconds& Milliseconds::operator= ( Microseconds time ) { m_value = time.m_value / 1000; return *this; }
    inline Milliseconds& Milliseconds::operator= ( Seconds time ) { m_value = time.m_value * 1000; return *this; }
    inline Milliseconds::operator Microseconds() const { return Microseconds( m_value * 1000 ); }
    inline Milliseconds::operator Seconds() const { return Seconds( m_value / 1000 ); }
    inline Microseconds Milliseconds::ToMicroseconds() const { return operator Microseconds(); }
    inline Seconds Milliseconds::ToSeconds() const { return operator Seconds(); }

    //-------------------------------------------------------------------------

    inline Seconds::Seconds( Ticks time ) : m_value( time.ToSeconds() ) {}
    inline Seconds::Seconds( Microseconds time ) : m_value( time.m_value / 1000000 ) {}
    inline Seconds::Seconds( Milliseconds time ) : m_value( time.m_value / 1000 ) {}
    inline Seconds& Seconds::operator= ( Microseconds time ) { m_value = time.m_value / 1000000; return *this; }
    inline Seconds& Seconds::operator= ( Milliseconds time ) { m_value = time.m_value / 1000; return *this; }
    inline Seconds::operator Microseconds() const { return Microseconds( m_value * 1000000 ); }
    inline Seconds::operator Milliseconds() const { return Milliseconds( m_value * 1000 ); }
    inline Microseconds Seconds::ToMicroseconds() const { return operator Microseconds(); }
    inline Milliseconds Seconds::ToMilliseconds() const { return operator Milliseconds(); }

    //-------------------------------------------------------------------------
    // Arithmetic functions
    //-------------------------------------------------------------------------

    inline Microseconds Microseconds::operator+ ( F32 const time ) const { return Microseconds( m_value + time ); }
    inline Microseconds Microseconds::operator+ ( Microseconds const time ) const { return Microseconds( m_value + time.m_value ); }
    inline Microseconds Microseconds::operator+ ( Milliseconds const time ) const { return *this + (Microseconds) time; }
    inline Microseconds Microseconds::operator+ ( Seconds const time ) const { return *this + (Microseconds) time; }

    inline Microseconds Microseconds::operator- ( F32 const time ) const { return Microseconds( m_value - time ); }
    inline Microseconds Microseconds::operator- ( Microseconds const time ) const { return Microseconds( m_value - time.m_value ); }
    inline Microseconds Microseconds::operator- ( Milliseconds const time ) const { return *this - (Microseconds) time; }
    inline Microseconds Microseconds::operator- ( Seconds const time ) const { return *this - (Microseconds) time; }

    inline Microseconds& Microseconds::operator+= ( F32 const time ) { m_value += time; return *this; }
    inline Microseconds& Microseconds::operator+= ( Microseconds const time ) { m_value += time.m_value; return *this; }
    inline Microseconds& Microseconds::operator+= ( Milliseconds const time ) { *this += (Microseconds) time; return *this; }
    inline Microseconds& Microseconds::operator+= ( Seconds const time ) { *this += (Microseconds) time; return *this; }

    inline Microseconds& Microseconds::operator-= ( F32 const time ) { m_value -= time; return *this; }
    inline Microseconds& Microseconds::operator-= ( Microseconds const time ) { m_value -= time.m_value; return *this; }
    inline Microseconds& Microseconds::operator-= ( Milliseconds const time ) { *this -= (Microseconds) time; return *this; }
    inline Microseconds& Microseconds::operator-= ( Seconds const time ) { *this -= (Microseconds) time; return *this; }

    //-------------------------------------------------------------------------

    inline Milliseconds Milliseconds::operator+ ( F32 const time ) const { return Milliseconds( m_value + time ); }
    inline Milliseconds Milliseconds::operator+ ( Milliseconds const time ) const { return Milliseconds( m_value + time.m_value ); }
    inline Milliseconds Milliseconds::operator+ ( Microseconds const time ) const { return *this + (Milliseconds) time; }
    inline Milliseconds Milliseconds::operator+ ( Seconds const time ) const { return *this + (Milliseconds) time; }

    inline Milliseconds Milliseconds::operator- ( F32 const time ) const { return Milliseconds( m_value - time ); }
    inline Milliseconds Milliseconds::operator- ( Milliseconds const time ) const { return Milliseconds( m_value - time.m_value ); }
    inline Milliseconds Milliseconds::operator- ( Microseconds const time ) const { return *this - (Milliseconds) time; }
    inline Milliseconds Milliseconds::operator- ( Seconds const time ) const { return *this - (Milliseconds) time; }

    inline Milliseconds& Milliseconds::operator+= ( F32 const time ) { m_value += time; return *this; }
    inline Milliseconds& Milliseconds::operator+= ( Milliseconds const time ) { m_value += time.m_value; return *this; }
    inline Milliseconds& Milliseconds::operator+= ( Microseconds const time ) { *this += (Milliseconds) time; return *this; }
    inline Milliseconds& Milliseconds::operator+= ( Seconds const time ) { *this += (Milliseconds) time; return *this; }

    inline Milliseconds& Milliseconds::operator-= ( F32 const time ) { m_value -= time; return *this; }
    inline Milliseconds& Milliseconds::operator-= ( Milliseconds const time ) { m_value -= time.m_value; return *this; }
    inline Milliseconds& Milliseconds::operator-= ( Microseconds const time ) { *this -= (Milliseconds) time; return *this; }
    inline Milliseconds& Milliseconds::operator-= ( Seconds const time ) { *this -= (Milliseconds) time; return *this; }

    //-------------------------------------------------------------------------

    inline Seconds Seconds::operator+ ( F32 const time ) const { return Seconds( m_value + time ); }
    inline Seconds Seconds::operator+ ( Seconds const time ) const { return Seconds( m_value + time.m_value ); }
    inline Seconds Seconds::operator+ ( Microseconds const time ) const { return *this + (Seconds) time; }
    inline Seconds Seconds::operator+ ( Milliseconds const time ) const { return *this + (Seconds) time; }

    inline Seconds Seconds::operator- ( F32 const time ) const { return Seconds( m_value - time ); }
    inline Seconds Seconds::operator- ( Seconds const time ) const { return Seconds( m_value - time.m_value ); }
    inline Seconds Seconds::operator- ( Microseconds const time ) const { return *this - (Seconds) time; }
    inline Seconds Seconds::operator- ( Milliseconds const time ) const { return *this - (Seconds) time; }

    inline Seconds& Seconds::operator+= ( F32 const time ) { m_value += time; return *this; }
    inline Seconds& Seconds::operator+= ( Seconds const time ) { m_value += time.m_value; return *this; }
    inline Seconds& Seconds::operator+= ( Microseconds const time ) { *this += (Seconds) time; return *this; }
    inline Seconds& Seconds::operator+= ( Milliseconds const time ) { *this += (Seconds) time; return *this; }

    inline Seconds& Seconds::operator-= ( F32 const time ) { m_value -= time; return *this; }
    inline Seconds& Seconds::operator-= ( Seconds const time ) { m_value -= time.m_value; return *this; }
    inline Seconds& Seconds::operator-= ( Microseconds const time ) { *this -= (Seconds) time; return *this; }
    inline Seconds& Seconds::operator-= ( Milliseconds const time ) { *this -= (Seconds) time; return *this; }

    inline Seconds Seconds::operator* ( F32 const time ) const { return Seconds( m_value * time ); }
    inline Seconds Seconds::operator* ( Seconds const time ) const { return Seconds( m_value * time.m_value ); }
    inline Seconds Seconds::operator* ( Microseconds const time ) const { return *this * (Seconds) time; }
    inline Seconds Seconds::operator* ( Milliseconds const time ) const { return *this * (Seconds) time; }

    inline Seconds Seconds::operator/ ( F32 const time ) const { return Seconds( m_value / time ); }
    inline Seconds Seconds::operator/ ( Seconds const time ) const { return Seconds( m_value / time.m_value ); }
    inline Seconds Seconds::operator/ ( Microseconds const time ) const { return *this / (Seconds) time; }
    inline Seconds Seconds::operator/ ( Milliseconds const time ) const { return *this / (Seconds) time; }

    inline Seconds& Seconds::operator*= ( F32 const time ) { m_value *= time; return *this; }
    inline Seconds& Seconds::operator*= ( Seconds const time ) { m_value *= time.m_value; return *this; }
    inline Seconds& Seconds::operator*= ( Microseconds const time ) { *this *= (Seconds) time; return *this; }
    inline Seconds& Seconds::operator*= ( Milliseconds const time ) { *this *= (Seconds) time; return *this; }

    inline Seconds& Seconds::operator/= ( F32 const time ) { m_value /= time; return *this; }
    inline Seconds& Seconds::operator/= ( Seconds const time ) { m_value /= time.m_value; return *this; }
    inline Seconds& Seconds::operator/= ( Microseconds const time ) { *this /= (Seconds) time; return *this; }
    inline Seconds& Seconds::operator/= ( Milliseconds const time ) { *this /= (Seconds) time; return *this; }

    //-------------------------------------------------------------------------

    inline Ticks GetSystemTime() { return Ticks::Now(); }

    //-------------------------------------------------------------------------

    inline Microseconds Microseconds::Now() { return Microseconds( GetSystemTime() ); }
    inline Milliseconds Milliseconds::Now() { return Milliseconds( GetSystemTime() ); }
    inline Seconds Seconds::Now() { return Seconds( GetSystemTime() ); }
}