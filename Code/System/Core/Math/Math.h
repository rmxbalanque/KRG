#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Core/Defines.h"
#include <math.h>
#include <float.h>
#include <stdlib.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        static constexpr float const Epsilon = 1.0e-06f;
        static constexpr float const LargeEpsilon = 1.0e-04f;
        static constexpr float const HugeEpsilon = 1.0e-02f;
        static constexpr float const Pi = 3.141592654f;
        static constexpr float const TwoPi = 6.283185307f;
        static constexpr float const OneDivPi = 0.318309886f;
        static constexpr float const OneDivTwoPi = 0.159154943f;
        static constexpr float const PiDivTwo = 1.570796327f;
        static constexpr float const PiDivFour = 0.785398163f;

        static constexpr float const SqrtTwo = 1.4142135623730950488016887242097f;
        static constexpr float const OneDivSqrtTwo = 1.0f / SqrtTwo;

        static constexpr float const DegreesToRadians = 0.0174532925f;
        static constexpr float const RadiansToDegrees = 57.2957795f;

        static constexpr float const Infinity = std::numeric_limits<float>::infinity();
        static constexpr float const QNaN = std::numeric_limits<float>::quiet_NaN();

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE float Sin( float x ) { return sinf( x ); }
        KRG_FORCE_INLINE float Cos( float x ) { return cosf( x ); }
        KRG_FORCE_INLINE float Tan( float x ) { return tanf( x ); }

        KRG_FORCE_INLINE float ASin( float x ) { return asinf( x ); }
        KRG_FORCE_INLINE float ACos( float x ) { return acosf( x ); }
        KRG_FORCE_INLINE float ATan( float x ) { return atanf( x ); }
        KRG_FORCE_INLINE float ATan2( float y, float x ) { return atan2f( y, x ); }

        KRG_FORCE_INLINE float Cosec( float x ) { return 1.0f / sinf( x ); }
        KRG_FORCE_INLINE float Sec( float x ) { return 1.0f / cosf( x ); }
        KRG_FORCE_INLINE float Cot( float x ) { return 1.0f / tanf( PiDivTwo - x ); }

        KRG_FORCE_INLINE float Pow( float x, float y ) { return pow( x, y ); }
        KRG_FORCE_INLINE float Sqrt( float v ) { return sqrtf( v ); }

        KRG_FORCE_INLINE float AddToMovingAverage( float currentAverage, uint64 numCurrentSamples, float newValue )
        {
            return currentAverage + ( ( newValue - currentAverage ) / ( numCurrentSamples + 1 ) );
        }

        KRG_FORCE_INLINE float Reciprocal( float r ) { return 1.0f / r; }
        KRG_FORCE_INLINE double Reciprocal( double r ) { return 1.0 / r; }

        template<typename T>
        KRG_FORCE_INLINE T Min( T a, T b ) { return a <= b ? a : b; }

        template<typename T>
        KRG_FORCE_INLINE T Max( T a, T b ) { return a >= b ? a : b; }

        template<typename T>
        KRG_FORCE_INLINE T AbsMin( T a, T b ) { return abs( a ) <= abs( b ) ? a : b; }

        template<typename T>
        KRG_FORCE_INLINE T AbsMax( T a, T b ) { return abs( a ) >= abs( b ) ? a : b; }

        template<typename T>
        KRG_FORCE_INLINE T Abs( T a ) { return abs( a ); }

        template<typename T>
        KRG_FORCE_INLINE T Sqrt( T a ) { return sqrt( a ); }

        template<typename T>
        KRG_FORCE_INLINE T Clamp( T value, T lowerBound, T upperBound )
        {
            KRG_ASSERT( lowerBound < upperBound );
            return Min( Max( value, lowerBound ), upperBound );
        }

        template<typename T>
        KRG_FORCE_INLINE bool IsInRangeInclusive( T value, T lowerBound, T upperBound )
        {
            KRG_ASSERT( lowerBound < upperBound );
            return value >= lowerBound && value <= upperBound;
        }

        template<typename T>
        KRG_FORCE_INLINE bool IsInRangeExclusive( T value, T lowerBound, T upperBound )
        {
            KRG_ASSERT( lowerBound < upperBound );
            return value > lowerBound && value < upperBound;
        }

        KRG_FORCE_INLINE float ModF( float value, float* pIntegerPortion )
        {
            return modff( value, pIntegerPortion );
        }

        KRG_FORCE_INLINE float FModF( float x, float y )
        {
            return fmodf( x, y );
        }

        template<typename T>
        KRG_FORCE_INLINE T Lerp( T A, T B, float t )
        {
            return A + ( B - A ) * t;
        }

        template<typename T>
        KRG_FORCE_INLINE T PercentageThroughRange( T value, T lowerBound, T upperBound )
        {
            KRG_ASSERT( lowerBound < upperBound );
            return Clamp( value, lowerBound, upperBound ) / ( upperBound - lowerBound );
        }

        template<typename T>
        KRG_FORCE_INLINE bool IsNearEqual( T value, T comparand, float epsilon = Epsilon )
        {
            return abs( (double) value - comparand ) <= epsilon;
        }

        template<typename T>
        KRG_FORCE_INLINE bool IsNearlyZero( T value, float epsilon = Epsilon )
        {
            return abs( value ) <= epsilon;
        }

        template<typename T>
        KRG_FORCE_INLINE T Ceiling( T value )
        {
            return ceil( value );
        }

        template<typename T>
        KRG_FORCE_INLINE int32 CeilingToInt( T value )
        {
            return (int32) ceil( value );
        }

        template<typename T>
        KRG_FORCE_INLINE T Floor( T value )
        {
            return floor( value );
        }

        template <typename T>
        KRG_FORCE_INLINE int32 FloorToInt( T value )
        {
            return (int32) floor( value );
        }

        template<typename T>
        KRG_FORCE_INLINE T Round( T value )
        {
            return round( value );
        }

        template<typename T>
        KRG_FORCE_INLINE int32 RoundToInt( T value )
        {
            return (int32) round( value );
        }

        inline int32 GreatestCommonDivisor( int32 a, int32 b )
        {
            return b == 0 ? a : GreatestCommonDivisor( b, a % b );
        }

        inline int32 LowestCommonMultiple( int32 a, int32 b )
        {
            int32 const gcd = GreatestCommonDivisor( a, b );
            int32 const lcm = ( a / gcd ) * b;
            return lcm;
        }

        inline float RemapRange( float x, float fromRangeMin, float fromRangeMax, float toRangeMin, float toRangeMax )
        {
            float const fromRangeLength = fromRangeMax - fromRangeMin;
            float const toRangeLength = toRangeMax - toRangeMin;
            float const percentageThroughFromRange = ( x - fromRangeMin ) / fromRangeLength;
            float const result = toRangeMin + ( percentageThroughFromRange * toRangeLength );

            return result;
        }

        template<typename T>
        KRG_FORCE_INLINE bool IsOdd( T n )
        {
            static_assert( std::numeric_limits<T>::is_integer, "Integer type required" );
            return n % 2 != 0; 
        }

        template<typename T>
        KRG_FORCE_INLINE bool IsEven( T n )
        {
            static_assert( std::numeric_limits<T>::is_integer, "Integer type required." );
            return n % 2 == 0; 
        }
    }

    //-------------------------------------------------------------------------

    enum ZeroInit { InitToZero };
    enum IdentityInit { InitToIdentity };

    //-------------------------------------------------------------------------

    enum class Axis
    {
        X = 0,
        Y = 1,
        Z = 2,
    };

    //-------------------------------------------------------------------------

    struct Float2;
    struct Float3;
    struct Float4;

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API Int2
    {
        KRG_SERIALIZE_MEMBERS( x, y );

        static Int2 const Zero;

    public:

        inline Int2() {}
        inline Int2( ZeroInit ) : x( 0 ), y( 0 ) {}
        inline Int2( Float2 const& v );
        inline explicit Int2( int32 v ) : x( v ), y( v ) {}
        inline explicit Int2( int32 ix, int32 iy ) : x( ix ), y( iy ) {}

        inline int32& operator[]( uint32 i ) { KRG_ASSERT( i < 2 ); return ( (int32*) this )[i]; }
        inline int32 const& operator[]( uint32 i ) const { KRG_ASSERT( i < 2 ); return ( (int32*) this )[i]; }

        inline bool operator==( Int2 const rhs ) const { return x == rhs.x && y == rhs.y; }
        inline bool operator!=( Int2 const rhs ) const { return x != rhs.x || y != rhs.y; }

        inline Int2 operator+( Int2 const& rhs ) const { return Int2( x + rhs.x, y + rhs.y ); }
        inline Int2 operator-( Int2 const& rhs ) const { return Int2( x - rhs.x, y - rhs.y ); }
        inline Int2 operator*( Int2 const& rhs ) const { return Int2( x * rhs.x, y * rhs.y ); }
        inline Int2 operator/( Int2 const& rhs ) const { return Int2( x / rhs.x, y / rhs.y ); }

        inline Int2& operator+=( int32 const& rhs ) { x += rhs; y += rhs; return *this; }
        inline Int2& operator-=( int32 const& rhs ) { x -= rhs; y -= rhs; return *this; }
        inline Int2& operator*=( int32 const& rhs ) { x *= rhs; y *= rhs; return *this; }
        inline Int2& operator/=( int32 const& rhs ) { x /= rhs; y /= rhs; return *this; }

        // Component wise operation
        inline Int2 operator+( int32 const& rhs ) const { return Int2( x + rhs, y + rhs ); }
        inline Int2 operator-( int32 const& rhs ) const { return Int2( x - rhs, y - rhs ); }
        inline Int2 operator*( int32 const& rhs ) const { return Int2( x * rhs, y * rhs ); }
        inline Int2 operator/( int32 const& rhs ) const { return Int2( x / rhs, y / rhs ); }

        inline Int2& operator+=( Int2 const& rhs ) { x += rhs.x; y += rhs.y; return *this; }
        inline Int2& operator-=( Int2 const& rhs ) { x -= rhs.x; y -= rhs.y; return *this; }
        inline Int2& operator*=( Int2 const& rhs ) { x *= rhs.x; y *= rhs.y; return *this; }
        inline Int2& operator/=( Int2 const& rhs ) { x /= rhs.x; y /= rhs.y; return *this; }

    public:

        int32 x, y;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API Int4
    {
        KRG_SERIALIZE_MEMBERS( x, y, z, w );

        static Int4 const Zero;

    public:

        inline Int4() {}
        inline Int4( ZeroInit ) : x( 0 ), y( 0 ), z( 0 ), w( 0 ) {}
        inline explicit Int4( int32 v ) : x( v ), y( v ), z( v ), w( v ) {}
        inline explicit Int4( int32 ix, int32 iy, int32 iz, int32 iw ) : x( ix ), y( iy ), z( iz ), w( iw ) {}

        inline int32& operator[]( uint32 i ) { KRG_ASSERT( i < 4 ); return ( (int32*) this )[i]; }
        inline int32 const& operator[]( uint32 i ) const { KRG_ASSERT( i < 4 ); return ( (int32*) this )[i]; }

        inline bool operator==( Int4 const rhs ) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
        inline bool operator!=( Int4 const rhs ) const { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

        inline Int4 operator+( int32 const& rhs ) const { return Int4( x + rhs, y + rhs, z + rhs, w + rhs ); }
        inline Int4 operator-( int32 const& rhs ) const { return Int4( x - rhs, y - rhs, z - rhs, w - rhs ); }
        inline Int4 operator*( int32 const& rhs ) const { return Int4( x * rhs, y * rhs, z * rhs, w * rhs ); }
        inline Int4 operator/( int32 const& rhs ) const { return Int4( x / rhs, y / rhs, z / rhs, w / rhs ); }

        inline Int4& operator+=( int32 const& rhs ) { x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
        inline Int4& operator-=( int32 const& rhs ) { x -= rhs; y -= rhs; z -= rhs; w -= rhs; return *this; }
        inline Int4& operator*=( int32 const& rhs ) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
        inline Int4& operator/=( int32 const& rhs ) { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }

        // Component wise operation
        inline Int4 operator+( Int4 const& rhs ) const { return Int4( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w ); }
        inline Int4 operator-( Int4 const& rhs ) const { return Int4( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w ); }
        inline Int4 operator*( Int4 const& rhs ) const { return Int4( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w ); }
        inline Int4 operator/( Int4 const& rhs ) const { return Int4( x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w ); }

        inline Int4& operator+=( Int4 const& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
        inline Int4& operator-=( Int4 const& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
        inline Int4& operator*=( Int4 const& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
        inline Int4& operator/=( Int4 const& rhs ) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }

    public:

        int32 x, y, z, w;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API Float2
    {
        KRG_SERIALIZE_MEMBERS( x, y );

        static Float2 const Zero;
        static Float2 const One;
        static Float2 const UnitX;
        static Float2 const UnitY;

    public:

        inline Float2() {}
        KRG_FORCE_INLINE Float2( ZeroInit ) : x( 0 ), y( 0 ) {}
        KRG_FORCE_INLINE explicit Float2( float v ) : x( v ), y( v ) {}
        KRG_FORCE_INLINE explicit Float2( float ix, float iy ) : x( ix ), y( iy ) {}
        inline explicit Float2( Int2 const& v ) : x( (float) v.x ), y( (float) v.y ) {}
        inline explicit Float2( Float3 const& v );
        inline explicit Float2( Float4 const& v );

        inline float& operator[]( uint32 i ) { KRG_ASSERT( i < 2 ); return ( (float*) this )[i]; }
        inline float const& operator[]( uint32 i ) const { KRG_ASSERT( i < 2 ); return ( (float*) this )[i]; }

        inline bool operator==( Float2 const rhs ) const { return x == rhs.x && y == rhs.y; }
        inline bool operator!=( Float2 const rhs ) const { return x != rhs.x || y != rhs.y; }

        inline Float2 operator+( Float2 const& rhs ) const { return Float2( x + rhs.x, y + rhs.y ); }
        inline Float2 operator-( Float2 const& rhs ) const { return Float2( x - rhs.x, y - rhs.y ); }
        inline Float2 operator*( Float2 const& rhs ) const { return Float2( x * rhs.x, y * rhs.y ); }
        inline Float2 operator/( Float2 const& rhs ) const { return Float2( x / rhs.x, y / rhs.y ); }

        inline Float2 operator+( float const& rhs ) const { return Float2( x + rhs, y + rhs ); }
        inline Float2 operator-( float const& rhs ) const { return Float2( x - rhs, y - rhs ); }
        inline Float2 operator*( float const& rhs ) const { return Float2( x * rhs, y * rhs ); }
        inline Float2 operator/( float const& rhs ) const { return Float2( x / rhs, y / rhs ); }

        inline Float2& operator+=( Float2 const& rhs ) { x += rhs.x; y += rhs.y; return *this; }
        inline Float2& operator-=( Float2 const& rhs ) { x -= rhs.x; y -= rhs.y; return *this; }
        inline Float2& operator*=( Float2 const& rhs ) { x *= rhs.x; y *= rhs.y; return *this; }
        inline Float2& operator/=( Float2 const& rhs ) { x /= rhs.x; y /= rhs.y; return *this; }

        inline Float2& operator+=( float const& rhs ) { x += rhs; y += rhs; return *this; }
        inline Float2& operator-=( float const& rhs ) { x -= rhs; y -= rhs; return *this; }
        inline Float2& operator*=( float const& rhs ) { x *= rhs; y *= rhs; return *this; }
        inline Float2& operator/=( float const& rhs ) { x /= rhs; y /= rhs; return *this; }

        float x, y;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API Float3
    {
        KRG_SERIALIZE_MEMBERS( x, y, z );

        static Float3 const Zero;
        static Float3 const One;
        static Float3 const UnitX;
        static Float3 const UnitY;
        static Float3 const UnitZ;

        static Float3 const WorldForward;
        static Float3 const WorldUp;
        static Float3 const WorldRight;

    public:

        inline Float3() {}
        KRG_FORCE_INLINE Float3( ZeroInit ) : x( 0 ), y( 0 ), z( 0 ) {}
        KRG_FORCE_INLINE explicit Float3( float v ) : x( v ), y( v ), z( v ) {}
        KRG_FORCE_INLINE explicit Float3( float ix, float iy, float iz ) : x( ix ), y( iy ), z( iz ) {}
        inline explicit Float3( Float2 const& v, float iz = 0.0f ) : x( v.x ), y( v.y ), z( iz ) {}
        inline explicit Float3( Float4 const& v );

        inline float& operator[]( uint32 i ) { KRG_ASSERT( i < 3 ); return ( (float*) this )[i]; }
        inline float const& operator[]( uint32 i ) const { KRG_ASSERT( i < 3 ); return ( (float*) this )[i]; }

        inline bool operator==( Float3 const rhs ) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
        inline bool operator!=( Float3 const rhs ) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

        inline operator Float2() const { return Float2( x, y ); }

        inline Float3 operator+( Float3 const& rhs ) const { return Float3( x + rhs.x, y + rhs.y, z + rhs.z ); }
        inline Float3 operator-( Float3 const& rhs ) const { return Float3( x - rhs.x, y - rhs.y, z - rhs.z ); }
        inline Float3 operator*( Float3 const& rhs ) const { return Float3( x * rhs.x, y * rhs.y, z * rhs.z ); }
        inline Float3 operator/( Float3 const& rhs ) const { return Float3( x / rhs.x, y / rhs.y, z / rhs.z ); }

        inline Float3 operator+( float const& rhs ) const { return Float3( x + rhs, y + rhs, z + rhs ); }
        inline Float3 operator-( float const& rhs ) const { return Float3( x - rhs, y - rhs, z - rhs ); }
        inline Float3 operator*( float const& rhs ) const { return Float3( x * rhs, y * rhs, z * rhs ); }
        inline Float3 operator/( float const& rhs ) const { return Float3( x / rhs, y / rhs, z / rhs ); }

        inline Float3& operator+=( Float3 const& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
        inline Float3& operator-=( Float3 const& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        inline Float3& operator*=( Float3 const& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
        inline Float3& operator/=( Float3 const& rhs ) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

        inline Float3& operator+=( float const& rhs ) { x += rhs; y += rhs; z += rhs; return *this; }
        inline Float3& operator-=( float const& rhs ) { x -= rhs; y -= rhs; z -= rhs; return *this; }
        inline Float3& operator*=( float const& rhs ) { x *= rhs; y *= rhs; z *= rhs; return *this; }
        inline Float3& operator/=( float const& rhs ) { x /= rhs; y /= rhs; z /= rhs; return *this; }

        float x, y, z;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API Float4
    {
        KRG_SERIALIZE_MEMBERS( x, y, z, w );

        static Float4 const Zero;
        static Float4 const One;
        static Float4 const UnitX;
        static Float4 const UnitY;
        static Float4 const UnitZ;
        static Float4 const UnitW;

        static Float4 const WorldForward;
        static Float4 const WorldUp;
        static Float4 const WorldRight;

    public:

        Float4() {}
        KRG_FORCE_INLINE Float4( ZeroInit ) : x( 0 ), y( 0 ), z( 0 ), w( 0 ) {}
        KRG_FORCE_INLINE explicit Float4( float v ) : x( v ), y( v ), z( v ), w( v ) {}
        KRG_FORCE_INLINE explicit Float4( float ix, float iy, float iz, float iw ) : x( ix ), y( iy ), z( iz ), w( iw ) {}
        explicit Float4( Float2 const& v, float iz = 0.0f, float iw = 0.0f ) : x( v.x ), y( v.y ), z( iz ), w( iw ) {}
        explicit Float4( Float3 const& v, float iw = 0.0f ) : x( v.x ), y( v.y ), z( v.z ), w( iw ) {}

        float& operator[]( uint32 i ) { KRG_ASSERT( i < 4 ); return ( (float*) this )[i]; }
        float const& operator[]( uint32 i ) const { KRG_ASSERT( i < 4 ); return ( (float*) this )[i]; }

        bool operator==( Float4 const rhs ) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
        bool operator!=( Float4 const rhs ) const { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

        inline operator Float2() const { return Float2( x, y ); }
        inline operator Float3() const { return Float3( x, y, z ); }

        inline Float4 operator+( Float4 const& rhs ) const { return Float4( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w ); }
        inline Float4 operator-( Float4 const& rhs ) const { return Float4( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w ); }
        inline Float4 operator*( Float4 const& rhs ) const { return Float4( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w ); }
        inline Float4 operator/( Float4 const& rhs ) const { return Float4( x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w ); }

        inline Float4 operator+( float const& rhs ) const { return Float4( x + rhs, y + rhs, z + rhs, w + rhs ); }
        inline Float4 operator-( float const& rhs ) const { return Float4( x - rhs, y - rhs, z - rhs, w - rhs ); }
        inline Float4 operator*( float const& rhs ) const { return Float4( x * rhs, y * rhs, z * rhs, w * rhs ); }
        inline Float4 operator/( float const& rhs ) const { return Float4( x / rhs, y / rhs, z / rhs, w / rhs ); }

        inline Float4& operator+=( Float4 const& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
        inline Float4& operator-=( Float4 const& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
        inline Float4& operator*=( Float4 const& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
        inline Float4& operator/=( Float4 const& rhs ) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }

        inline Float4& operator+=( float const& rhs ) { x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
        inline Float4& operator-=( float const& rhs ) { x -= rhs; y -= rhs; z -= rhs; w -= rhs; return *this; }
        inline Float4& operator*=( float const& rhs ) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
        inline Float4& operator/=( float const& rhs ) { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }

        float x, y, z, w;
    };

    // Implicit conversions
    //-------------------------------------------------------------------------

    inline Int2::Int2( Float2 const& v )
        : x( (int32) v.x )
        , y( (int32) v.y )
    {}

    inline Float2::Float2( Float3 const& v )
        : x( v.x )
        , y( v.y )
    {}

    inline Float2::Float2( Float4 const& v )
        : x( v.x )
        , y( v.y )
    {}

    inline Float3::Float3( Float4 const& v )
        : x( v.x )
        , y( v.y )
        , z( v.z )
    {}

    //-------------------------------------------------------------------------
    // Type safe angle definitions
    //-------------------------------------------------------------------------

    struct Radians;
    struct Degrees;

    //-------------------------------------------------------------------------

    struct Degrees
    {
        KRG_SERIALIZE_MEMBERS( m_value );

    public:

        inline Degrees() = default;
        inline Degrees( float degrees ) : m_value( degrees ) {}
        inline explicit Degrees( Radians const& radians );

        inline explicit operator float() const { return m_value; }
        inline operator Radians() const;
        inline float GetValue() const { return m_value; }
        inline Radians ToRadians() const;

        inline Degrees operator-() const { return Degrees( -m_value ); }

        inline Degrees operator+( Degrees const& rhs ) const { return m_value + rhs.m_value; }
        inline Degrees operator-( Degrees const& rhs ) const { return m_value - rhs.m_value; }
        inline Degrees operator*( Degrees const& rhs ) const { return m_value * rhs.m_value; }
        inline Degrees operator/( Degrees const& rhs ) const { return m_value / rhs.m_value; }

        inline Degrees& operator+=( Degrees const& rhs ) { m_value += rhs.m_value; return *this; }
        inline Degrees& operator-=( Degrees const& rhs ) { m_value -= rhs.m_value; return *this; }
        inline Degrees& operator*=( Degrees const& rhs ) { m_value *= rhs.m_value; return *this; }
        inline Degrees& operator/=( Degrees const& rhs ) { m_value /= rhs.m_value; return *this; }

        inline Degrees operator+( float const& rhs ) const { return m_value + rhs; }
        inline Degrees operator-( float const& rhs ) const { return m_value - rhs; }
        inline Degrees operator*( float const& rhs ) const { return m_value * rhs; }
        inline Degrees operator/( float const& rhs ) const { return m_value / rhs; }

        inline Degrees& operator+=( float const& rhs ) { m_value += rhs; return *this; }
        inline Degrees& operator-=( float const& rhs ) { m_value -= rhs; return *this; }
        inline Degrees& operator*=( float const& rhs ) { m_value *= rhs; return *this; }
        inline Degrees& operator/=( float const& rhs ) { m_value /= rhs; return *this; }

        inline Degrees operator+( int32 const& rhs ) const { return m_value + rhs; }
        inline Degrees operator-( int32 const& rhs ) const { return m_value - rhs; }
        inline Degrees operator*( int32 const& rhs ) const { return m_value * rhs; }
        inline Degrees operator/( int32 const& rhs ) const { return m_value / rhs; }

        inline Degrees& operator+=( int32 const& rhs ) { m_value += rhs; return *this; }
        inline Degrees& operator-=( int32 const& rhs ) { m_value -= rhs; return *this; }
        inline Degrees& operator*=( int32 const& rhs ) { m_value *= rhs; return *this; }
        inline Degrees& operator/=( int32 const& rhs ) { m_value /= rhs; return *this; }

        inline Degrees operator+( uint32 const& rhs ) const { return m_value + rhs; }
        inline Degrees operator-( uint32 const& rhs ) const { return m_value - rhs; }
        inline Degrees operator*( uint32 const& rhs ) const { return m_value * rhs; }
        inline Degrees operator/( uint32 const& rhs ) const { return m_value / rhs; }

        inline Degrees& operator+=( uint32 const& rhs ) { m_value += rhs; return *this; }
        inline Degrees& operator-=( uint32 const& rhs ) { m_value -= rhs; return *this; }
        inline Degrees& operator*=( uint32 const& rhs ) { m_value *= rhs; return *this; }
        inline Degrees& operator/=( uint32 const& rhs ) { m_value /= rhs; return *this; }

        inline bool operator>( float const& rhs ) const { return m_value > rhs; };
        inline bool operator<( float const& rhs ) const { return m_value < rhs; }
        inline bool operator>=( float const& rhs ) const { return m_value >= rhs; }
        inline bool operator<=( float const& rhs ) const { return m_value <= rhs; }

        inline bool operator>( Degrees const& rhs ) const { return m_value > rhs.m_value; }
        inline bool operator<( Degrees const& rhs ) const { return m_value < rhs.m_value; }
        inline bool operator>=( Degrees const& rhs ) const { return m_value >= rhs.m_value; }
        inline bool operator<=( Degrees const& rhs ) const { return m_value <= rhs.m_value; }

        inline bool operator>( Radians const& rhs ) const;
        inline bool operator<( Radians const& rhs ) const;
        inline bool operator>=( Radians const& rhs ) const;
        inline bool operator<=( Radians const& rhs ) const;

        //-------------------------------------------------------------------------

        // Clamps between -360 and 360
        inline void Clamp()
        {
            m_value -= ( Math::Floor( m_value / 360.0f ) * 360.0f );
        }

        inline Degrees GetClamped() const
        {
            Degrees d( m_value );
            d.Clamp();
            return d;
        }

        // Clamps between 0 to 360
        inline Degrees& ClampToPositive()
        {
            Clamp();
            if ( m_value < 0 )
            {
                m_value += 360;
            }
            return *this;
        }

        inline Degrees GetClampedToPositive() const
        {
            Degrees d( m_value );
            d.ClampToPositive();
            return d;
        }

    private:

        float m_value = 0;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API Radians
    {
        KRG_SERIALIZE_MEMBERS( m_value );

        static Radians const Pi;
        static Radians const TwoPi;
        static Radians const OneDivPi;
        static Radians const OneDivTwoPi;
        static Radians const PiDivTwo;
        static Radians const PiDivFour;

    public:

        inline Radians() = default;
        inline Radians( float radians ) : m_value( radians ) {}
        inline explicit Radians( Degrees const& degrees );

        inline operator float() const { return m_value; }
        inline operator Degrees() const { return ToDegrees(); }
        inline float GetValue() const { return m_value; }
        inline Degrees ToDegrees() const { return m_value * Math::RadiansToDegrees; }

        inline Radians operator-() const { return Radians( -m_value ); }

        inline Radians operator+( Radians const& rhs ) const { return m_value + rhs.m_value; }
        inline Radians operator-( Radians const& rhs ) const { return m_value - rhs.m_value; }
        inline Radians operator*( Radians const& rhs ) const { return m_value * rhs.m_value; }
        inline Radians operator/( Radians const& rhs ) const { return m_value / rhs.m_value; }

        inline Radians& operator+=( Radians const& rhs ) { m_value += rhs.m_value; return *this; }
        inline Radians& operator-=( Radians const& rhs ) { m_value -= rhs.m_value; return *this; }
        inline Radians& operator*=( Radians const& rhs ) { m_value *= rhs.m_value; return *this; }
        inline Radians& operator/=( Radians const& rhs ) { m_value /= rhs.m_value; return *this; }

        inline Radians operator+( float const& rhs ) const { return m_value + rhs; }
        inline Radians operator-( float const& rhs ) const { return m_value - rhs; }
        inline Radians operator*( float const& rhs ) const { return m_value * rhs; }
        inline Radians operator/( float const& rhs ) const { return m_value / rhs; }

        inline Radians& operator+=( float const& rhs ) { m_value += rhs; return *this; }
        inline Radians& operator-=( float const& rhs ) { m_value -= rhs; return *this; }
        inline Radians& operator*=( float const& rhs ) { m_value *= rhs; return *this; }
        inline Radians& operator/=( float const& rhs ) { m_value /= rhs; return *this; }

        inline Radians operator+( int32 const& rhs ) const { return m_value + rhs; }
        inline Radians operator-( int32 const& rhs ) const { return m_value - rhs; }
        inline Radians operator*( int32 const& rhs ) const { return m_value * rhs; }
        inline Radians operator/( int32 const& rhs ) const { return m_value / rhs; }

        inline Radians& operator+=( int32 const& rhs ) { m_value += rhs; return *this; }
        inline Radians& operator-=( int32 const& rhs ) { m_value -= rhs; return *this; }
        inline Radians& operator*=( int32 const& rhs ) { m_value *= rhs; return *this; }
        inline Radians& operator/=( int32 const& rhs ) { m_value /= rhs; return *this; }

        inline Radians operator+( uint32 const& rhs ) const { return m_value + rhs; }
        inline Radians operator-( uint32 const& rhs ) const { return m_value - rhs; }
        inline Radians operator*( uint32 const& rhs ) const { return m_value * rhs; }
        inline Radians operator/( uint32 const& rhs ) const { return m_value / rhs; }

        inline Radians& operator+=( uint32 const& rhs ) { m_value += rhs; return *this; }
        inline Radians& operator-=( uint32 const& rhs ) { m_value -= rhs; return *this; }
        inline Radians& operator*=( uint32 const& rhs ) { m_value *= rhs; return *this; }
        inline Radians& operator/=( uint32 const& rhs ) { m_value /= rhs; return *this; }

        inline bool operator>( float const& rhs ) const { return m_value > rhs; };
        inline bool operator<( float const& rhs ) const { return m_value < rhs; }
        inline bool operator>=( float const& rhs ) const { return m_value >= rhs; }
        inline bool operator<=( float const& rhs ) const { return m_value <= rhs; }

        inline bool operator>( Radians const& rhs ) const { return m_value > rhs.m_value; }
        inline bool operator<( Radians const& rhs ) const { return m_value < rhs.m_value; }
        inline bool operator>=( Radians const& rhs ) const { return m_value >= rhs.m_value; }
        inline bool operator<=( Radians const& rhs ) const { return m_value <= rhs.m_value; }

        inline bool operator>( Degrees const& rhs ) const;
        inline bool operator<( Degrees const& rhs ) const;
        inline bool operator>=( Degrees const& rhs ) const;
        inline bool operator<=( Degrees const& rhs ) const;

        //-------------------------------------------------------------------------

        // Clamps between -2Pi to 2Pi
        inline void Clamp()
        {
            m_value -= int32( m_value / Math::TwoPi ) * Math::TwoPi;
        }

        inline Radians GetClamped() const
        {
            Radians r( m_value );
            r.Clamp();
            return r;
        }

        // Clamps between 0 to 2Pi
        inline void ClampToPositive()
        {
            Clamp();
            if( m_value < 0 )
            {
                m_value += Math::TwoPi;
            }
        }

        inline Radians GetClampedToPositive() const
        {
            Radians r( m_value );
            r.ClampToPositive();
            return r;
        }

        // Inverts angle between [0;2Pi] and [-2Pi;0]
        inline void Invert()
        {
            Clamp();
            float const delta = Math::TwoPi - Math::Abs( m_value );
            m_value = ( m_value < 0 ) ? delta : -delta;
        }

        inline Radians GetInverse() const
        {
            Radians r( m_value );
            r.Invert();
            return r;
        }

        // Clamps to -Pi to Pi
        inline void ClampPi()
        {
            Clamp();

            float delta = Math::Pi - Math::Abs( m_value );
            if ( delta < 0 )
            {
                delta += Math::Pi;
                m_value = ( m_value < 0 ) ? delta : -delta;
            }
        }

        inline Radians GetClampedPi() const
        {
            Radians r( m_value );
            r.ClampPi();
            return r;
        }

        // Flips the front and rear 180 degree arc i.e. 135 becomes -45, -90 becomes 90, etc.
        inline void Flip()
        {
            ClampPi();
            float const delta = Math::Pi - Math::Abs( m_value );
            m_value = ( m_value < 0 ) ? delta : -delta;
        }

        inline Radians GetFlipped() const
        {
            Radians r( m_value );
            r.Flip();
            return r;
        }

    private:

        float m_value = 0;
    };

    //-------------------------------------------------------------------------

    inline Degrees::Degrees( Radians const& radians )
        : m_value( radians.ToDegrees() )
    {}

    inline Radians Degrees::ToRadians() const
    {
        return m_value * Math::DegreesToRadians;
    }

    inline Degrees::operator Radians() const
    { 
        return ToRadians(); 
    }

    inline bool Degrees::operator>( Radians const& rhs ) const { return m_value > rhs.ToDegrees().m_value; }
    inline bool Degrees::operator<( Radians const& rhs ) const { return m_value < rhs.ToDegrees().m_value; }
    inline bool Degrees::operator>=( Radians const& rhs ) const { return m_value >= rhs.ToDegrees().m_value; }
    inline bool Degrees::operator<=( Radians const& rhs ) const { return m_value <= rhs.ToDegrees().m_value; }

    //-------------------------------------------------------------------------

    inline Radians::Radians( Degrees const& degrees )
        : m_value( degrees.ToRadians() )
    {}

    inline bool Radians::operator>( Degrees const& rhs ) const { return m_value > rhs.ToRadians().m_value; }
    inline bool Radians::operator<( Degrees const& rhs ) const { return m_value < rhs.ToRadians().m_value; }
    inline bool Radians::operator>=( Degrees const& rhs ) const { return m_value >= rhs.ToRadians().m_value; }
    inline bool Radians::operator<=( Degrees const& rhs ) const { return m_value <= rhs.ToRadians().m_value; }

    //-------------------------------------------------------------------------
    // Euler Angles - Order of rotation in KRG is XYZ
    //-------------------------------------------------------------------------

    struct EulerAngles
    {
        KRG_SERIALIZE_MEMBERS( x, y, z );

    public:

        EulerAngles() = default;

        inline EulerAngles( Degrees inX, Degrees inY, Degrees inZ )
            : x( inX )
            , y( inY )
            , z( inZ )
        {}

        inline void Clamp()
        {
            x.Clamp();
            y.Clamp();
            z.Clamp();
        }

        inline EulerAngles GetClamped() const
        { 
            EulerAngles clamped = *this;
            clamped.Clamp();
            return clamped;
        }

        inline Radians GetYaw() const { return z; }
        inline Radians GetPitch() const { return x; }
        inline Radians GetRoll() const { return y; }

        inline bool operator==( EulerAngles const& other ) const { return x == other.x && y == other.y && z == other.z; }
        inline bool operator!=( EulerAngles const& other ) const { return x != other.x || y != other.y || z != other.z; }

        inline Radians& operator[]( uint32 i ) { KRG_ASSERT( i < 3 ); return ( (Radians*) this )[i]; }
        inline Radians const& operator[]( uint32 i ) const { KRG_ASSERT( i < 3 ); return ( (Radians*) this )[i]; }

    public:

        Radians x = 0;
        Radians y = 0;
        Radians z = 0;
    };

    //-------------------------------------------------------------------------
    // Axis Angle
    //-------------------------------------------------------------------------

    struct AxisAngle
    {
        KRG_SERIALIZE_MEMBERS( m_axis, m_angle );

    public:

        inline AxisAngle() = default;
        inline AxisAngle( Float3 axis, Radians angle ) : m_axis( axis ), m_angle( angle ) {}

        inline bool IsValid() const
        {
            float const lengthSq = m_axis.x * m_axis.x + m_axis.y * m_axis.y + m_axis.z * m_axis.z;
            return Math::Abs( lengthSq - 1.0f ) < Math::Epsilon;
        }

    public:

        Float3      m_axis = Float3::Zero;
        Radians     m_angle = 0.0f;
    };
}