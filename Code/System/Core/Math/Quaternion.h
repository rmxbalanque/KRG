#pragma once

#include "Vector.h"

//-------------------------------------------------------------------------
// The Kruger math library is heavily based on the DirectX math library
//-------------------------------------------------------------------------
// https://github.com/Microsoft/DirectXMath
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API alignas( 16 ) Quaternion
    {
        KRG_SERIALIZE_MEMBERS( x, y, z, w );

    public:

        static Quaternion const Identity;

        inline static Quaternion FromRotationBetweenVectors( Vector const sourceVector, Vector const targetVector );

        inline static Quaternion NLerp( Quaternion from, Quaternion to, F32 t );
        inline static Quaternion SLerp( Quaternion from, Quaternion to, F32 t );
        inline static Quaternion SQuad( Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, F32 t );

        inline static Vector Dot( Quaternion const& q0, Quaternion const& q1 ) { return Vector::Dot4( q0.AsVector(), q1.AsVector() ); }
        inline static Radians Distance( Quaternion const& q0, Quaternion const& q1 );
    public:

        inline Quaternion() = default;
        inline explicit Quaternion( IdentityInit ) : m_data( Vector::UnitW.m_data ) {}
        inline explicit Quaternion( Vector const v ) : m_data( v.m_data ) {}
        inline explicit Quaternion( F32 ix, F32 iy, F32 iz, F32 iw ) { m_data = _mm_set_ps( iw, iz, iy, ix ); }
        inline explicit Quaternion( Float4 const& v ) : Quaternion( v.x, v.y, v.z, v.w ) {}

        inline explicit Quaternion( Vector const axis, Radians angle );
        inline explicit Quaternion( AxisAngle axisAngle ) : Quaternion( Vector( axisAngle.m_axis ), axisAngle.m_angle ) {}

        inline explicit Quaternion( EulerAngles const& eulerAngles );
        inline explicit Quaternion( Radians rotX, Radians rotY, Radians rotZ ) : Quaternion( EulerAngles( rotX, rotY, rotZ ) ) {}

        KRG_FORCE_INLINE operator __m128&( ) { return m_data; }
        KRG_FORCE_INLINE operator __m128 const&() const { return m_data; }

        inline Vector const& ToVector() const { return reinterpret_cast<Vector const&>( *this ); }
        inline AxisAngle ToAxisAngle() const;
        EulerAngles ToEulerAngles() const;

        inline Vector RotateVector( Vector const vector ) const;
        inline Vector RotateVectorInverse( Vector const vector ) const;

        // Operations
        inline Quaternion& Conjugate();
        inline Quaternion GetConjugate() const;

        inline Quaternion& Negate();
        inline Quaternion GetNegated() const;

        inline Quaternion& Invert();
        inline Quaternion GetInverse() const;

        inline Quaternion& Normalize();
        inline Quaternion GetNormalized() const;

        inline bool IsNormalized() const { return AsVector().IsNormalized4(); }
        inline bool IsIdentity() const { return AsVector().IsEqual3( Vector::UnitW ); }

        // Concatenate the rotation of this onto rhs and return the result i.e. first rotate by rhs then by this
        // This means order of rotation is right-to-left: child-rotation * parent-rotation
        inline Quaternion operator*( Quaternion const& rhs ) const;
        inline Quaternion& operator*=( Quaternion const& rhs ) { *this = *this * rhs; return *this; }

        inline bool operator==( Quaternion const& rhs ) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
        inline bool operator!=( Quaternion const& rhs ) const { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

    private:

        inline Vector& AsVector() { return reinterpret_cast<Vector&>( *this ); }
        inline Vector const& AsVector() const { return reinterpret_cast<Vector const&>( *this ); }

        Quaternion& operator=( Vector const v ) = delete;

    public:

        union
        {
            struct { F32 x, y, z, w; };
            __m128 m_data;
        };
    };

    static_assert( sizeof( Vector ) == 16, "Quaternion size must be 16 bytes!" );

    //-------------------------------------------------------------------------

    inline Radians Quaternion::Distance( Quaternion const& q0, Quaternion const& q1 )
    {
        return 2 * Math::ACos( Math::Abs( Dot( q0, q1 ).ToFloat() ) );
    }

    inline Quaternion& Quaternion::Conjugate()
    {
        static __m128 const conj = { -1.0f, -1.0f, -1.0f, 1.0f };
        m_data = _mm_mul_ps( *this, conj );
        return *this;
    }

    inline Quaternion Quaternion::GetConjugate() const
    {
        Quaternion q = *this;
        q.Conjugate();
        return q;
    }

    inline Quaternion& Quaternion::Negate()
    {
        m_data = _mm_mul_ps( *this, Vector::NegativeOne );
        return *this;
    }

    inline Quaternion Quaternion::GetNegated() const
    {
        Quaternion q = *this;
        q.Negate();
        return q;
    }

    inline Quaternion& Quaternion::Invert()
    {
        Vector const conjugate = GetConjugate().AsVector();
        Vector const length = AsVector().Length4();
        Vector const mask = length.LessThanEqual( Vector::Epsilon );
        Vector const result = conjugate / length;
        AsVector() = result.Select( result, Vector::Zero, mask );
        return *this;
    }

    inline Quaternion Quaternion::GetInverse() const
    {
        Quaternion q = *this;
        q.Invert();
        return q;
    }

    inline Quaternion& Quaternion::Normalize()
    {
        AsVector().Normalize4();
        return *this;
    }

    inline Quaternion Quaternion::GetNormalized() const
    {
        Quaternion q = *this;
        q.Normalize();
        return q;
    }

    //-------------------------------------------------------------------------

    inline Quaternion::Quaternion( Vector const axis, Radians angle )
    {
        KRG_ASSERT( axis.IsNormalized3() );

        auto N = _mm_and_ps( axis, SIMD::g_maskXYZ0 );
        N = _mm_or_ps( N, Vector::UnitW );
        auto scale = _mm_set_ps1( 0.5f * (F32) angle );

        Vector sine, cosine;
        Vector::SinCos( sine, cosine, scale );

        scale = _mm_and_ps( sine, SIMD::g_maskXYZ0 );
        cosine = _mm_and_ps( cosine, SIMD::g_mask000W );
        scale = _mm_or_ps( scale, cosine );

        N = _mm_mul_ps( N, scale );
        m_data = N;
    }

    inline Quaternion::Quaternion( EulerAngles const& eulerAngles )
    {
        auto const rotationX = Quaternion( Vector::UnitX, eulerAngles.x );
        auto const rotationY = Quaternion( Vector::UnitY, eulerAngles.y );
        auto const rotationZ = Quaternion( Vector::UnitZ, eulerAngles.z );

        // Rotation order is XYZ - all in global space, hence the order is reversed
        m_data = ( rotationX * rotationY * rotationZ ).GetNormalized().m_data;
    }

    inline Quaternion Quaternion::FromRotationBetweenVectors( Vector const v0, Vector const v1 )
    {
        Quaternion result;

        // Parallel vectors - return zero rotation
        Vector const dot = Vector::Dot3( v0, v1 );
        if ( dot.IsGreaterThanEqual4( Vector::OneMinusEpsilon ) )
        {
            result = Quaternion::Identity;
        }
        // Opposite vectors - return 180 rotation around any orthogonal axis
        else if ( dot.IsLessThanEqual4( Vector::EpsilonMinusOne ) )
        {
            result = Quaternion( -v0.z, v0.y, v0.x, 0 );
            result.Normalize();
        }
        else // Calculate quaternion rotation
        {
            Vector const cross = Vector::Cross3( v0, v1 );
            Vector Q = Vector::Select( cross, dot, Vector::Select0001 );
            Q += Vector::Select( Vector::Zero, Q.Length4(), Vector::Select0001 );
            result = Quaternion( Q );
            result.Normalize();
        }

        return result;
    }

    //-------------------------------------------------------------------------

    inline AxisAngle Quaternion::ToAxisAngle() const
    {
        return AxisAngle( AsVector(), 2.0f * Math::ACos( w ) );
    }

    inline Vector Quaternion::RotateVector( Vector const vector ) const
    {
        Quaternion const A( Vector::Select( Vector::Select1110, vector, Vector::Select1110 ) );
        Quaternion const Result = GetConjugate() * A;
        return ( Result * *this ).AsVector();
    }

    inline Vector Quaternion::RotateVectorInverse( Vector const vector ) const
    {
        Quaternion const A( Vector::Select( Vector::Select1110, vector, Vector::Select1110 ) );
        Quaternion const Result = *this * A;
        return ( Result * GetConjugate() ).AsVector();
    }

    inline Quaternion Quaternion::operator*( Quaternion const& rhs ) const
    {
        static const __m128 ControlWZYX = { 1.0f,-1.0f, 1.0f,-1.0f };
        static const __m128 ControlZWXY = { 1.0f, 1.0f,-1.0f,-1.0f };
        static const __m128 ControlYXWZ = { -1.0f, 1.0f, 1.0f,-1.0f };

        // Copy to SSE registers and use as few as possible for x86
        __m128 Q2X = rhs;
        __m128 Q2Y = rhs;
        __m128 Q2Z = rhs;
        __m128 vResult = rhs;
        // Splat with one instruction
        vResult = _mm_shuffle_ps( vResult, vResult, _MM_SHUFFLE( 3, 3, 3, 3 ) );
        Q2X = _mm_shuffle_ps( Q2X, Q2X, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Q2Y = _mm_shuffle_ps( Q2Y, Q2Y, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Q2Z = _mm_shuffle_ps( Q2Z, Q2Z, _MM_SHUFFLE( 2, 2, 2, 2 ) );
        // Retire Q1 and perform Q1*Q2W
        vResult = _mm_mul_ps( vResult, *this );
        __m128 Q1Shuffle = *this;
        // Shuffle the copies of Q1
        Q1Shuffle = _mm_shuffle_ps( Q1Shuffle, Q1Shuffle, _MM_SHUFFLE( 0, 1, 2, 3 ) );
        // Mul by Q1WZYX
        Q2X = _mm_mul_ps( Q2X, Q1Shuffle );
        Q1Shuffle = _mm_shuffle_ps( Q1Shuffle, Q1Shuffle, _MM_SHUFFLE( 2, 3, 0, 1 ) );
        // Flip the signs on y and z
        Q2X = _mm_mul_ps( Q2X, ControlWZYX );
        // Mul by Q1ZWXY
        Q2Y = _mm_mul_ps( Q2Y, Q1Shuffle );
        Q1Shuffle = _mm_shuffle_ps( Q1Shuffle, Q1Shuffle, _MM_SHUFFLE( 0, 1, 2, 3 ) );
        // Flip the signs on z and w
        Q2Y = _mm_mul_ps( Q2Y, ControlZWXY );
        // Mul by Q1YXWZ
        Q2Z = _mm_mul_ps( Q2Z, Q1Shuffle );
        vResult = _mm_add_ps( vResult, Q2X );
        // Flip the signs on x and w
        Q2Z = _mm_mul_ps( Q2Z, ControlYXWZ );
        Q2Y = _mm_add_ps( Q2Y, Q2Z );
        vResult = _mm_add_ps( vResult, Q2Y );

        return Quaternion( vResult );
    }

    //-------------------------------------------------------------------------

    inline Quaternion Quaternion::NLerp( Quaternion from, Quaternion to, F32 T )
    {
        KRG_ASSERT( T >= 0.0f && T <= 1.0f );

        // Ensure that the rotations are in the same direction
        if ( Quaternion::Dot( from, to ).IsLessThan4( Vector::Zero ) )
        {
            from.Negate();
        }

        Quaternion result( Vector::Lerp( from.ToVector(), to.ToVector(), T ) );
        result.Normalize();
        return result;
    }

    inline Quaternion Quaternion::SLerp( Quaternion from, Quaternion to, F32 T )
    {
        KRG_ASSERT( T >= 0.0f && T <= 1.0f );

        static SIMD::UIntMask const maskSign = { 0x80000000,0x00000000,0x00000000,0x00000000 };
        static __m128 const oneMinusEpsilon = { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f };

        Vector const VecT( T );

        Vector CosOmega = Quaternion::Dot( from, to );

        Vector Control = CosOmega.LessThan( Vector::Zero );
        Vector Sign = Vector::Select( Vector::One, Vector::NegativeOne, Control );

        CosOmega = _mm_mul_ps( CosOmega, Sign );
        Control = CosOmega.LessThan( oneMinusEpsilon );

        Vector SinOmega = _mm_mul_ps( CosOmega, CosOmega );
        SinOmega = _mm_sub_ps( Vector::One, SinOmega );
        SinOmega = _mm_sqrt_ps( SinOmega );

        Vector Omega = Vector::ATan2( SinOmega, CosOmega );

        Vector V01 = _mm_shuffle_ps( VecT, VecT, _MM_SHUFFLE( 2, 3, 0, 1 ) );
        V01 = _mm_and_ps( V01, SIMD::g_maskXY00 );
        V01 = _mm_xor_ps( V01, maskSign );
        V01 = _mm_add_ps( Vector::UnitX, V01 );

        Vector S0 = _mm_mul_ps( V01, Omega );
        S0 = Vector::Sin( S0 );
        S0 = _mm_div_ps( S0, SinOmega );
        S0 = Vector::Select( V01, S0, Control );

        Vector S1 = S0.GetSplatY();
        S0 = S0.GetSplatX();

        S1 = _mm_mul_ps( S1, Sign );
        Vector result = _mm_mul_ps( from, S0 );
        S1 = _mm_mul_ps( S1, to );
        result = _mm_add_ps( result, S1 );

        return Quaternion( result );
    }

    inline Quaternion Quaternion::SQuad( Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, F32 t )
    {
        KRG_ASSERT( t >= 0.0f && t <= 1.0f );

        Quaternion const q03 = Quaternion::SLerp( q0, q3, t );
        Quaternion const q12 = Quaternion::SLerp( q1, q2, t );
        t = ( t - ( t * t ) ) * 2;
        Quaternion const result = Quaternion::SLerp( q03, q12, t );
        return result;
    }
}