#pragma once

#include "System/Core/Math/Math.h"
#include "System/Core/Math/Quaternion.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Quantization
    {
        //-------------------------------------------------------------------------
        // Normalized float quantization
        //-------------------------------------------------------------------------
        // Supports variable output bits (0-16)

        template<unsigned N>
        inline U16 EncodeUnsignedNormalizedFloat( F32 value )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Encoding into more than 16bits is not allowed" );

            KRG_ASSERT( value >= 0 && value <= 1.0f );

            F32 const quantizedValue = value * ( ( 1 << ( N ) ) - 1 ) + 0.5f;
            return U16( quantizedValue );
        }

        template<unsigned N>
        inline F32 DecodeUnsignedNormalizedFloat( U16 encodedValue )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Decoding from more than 16bits is not allowed" );
            return encodedValue / F32( ( 1 << ( N ) ) - 1 );
        }

        template<unsigned N>
        inline U16 EncodeSignedNormalizedFloat( F32 value )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Encoding into more than 16bits is not allowed" );

            KRG_ASSERT( value >= -1.0f && value <= 1.0f );

            // Track the sign and create a unsigned normalized float
            U16 sign = 0;
            if ( value < 0 )
            {
                sign = 1;
                value = -value;
            }

            return sign | ( EncodeUnsignedNormalizedFloat<N - 1>( value ) << 1 );
        }

        template<unsigned N>
        inline F32 DecodeSignedNormalizedFloat( U16 encodedValue )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Decoding from more than 16bits is not allowed" );

            F32 const sign = F32( encodedValue & 1 ? -1 : 1 );
            return DecodeUnsignedNormalizedFloat<N - 1>( encodedValue >> 1 ) * sign;
        }

        //-------------------------------------------------------------------------
        // Float quantization
        //-------------------------------------------------------------------------
        // 32 bit float to 16bit uint

        inline U16 EncodeFloat( F32 value, F32 const quantizationRangeStartValue, F32 const quantizationRangeLength )
        {
            KRG_ASSERT( quantizationRangeLength != 0 );

            F32 const normalizedValue = ( value - quantizationRangeStartValue ) / quantizationRangeLength;
            U16 const encodedValue = EncodeUnsignedNormalizedFloat<16>( normalizedValue );
            return encodedValue;
        }

        inline F32 DecodeFloat( U16 encodedValue, F32 const quantizationRangeStartValue, F32 const quantizationRangeLength )
        {
            KRG_ASSERT( quantizationRangeLength != 0 );

            F32 const normalizedValue = DecodeUnsignedNormalizedFloat<16>( encodedValue );
            F32 const decodedValue = ( normalizedValue * quantizationRangeLength ) + quantizationRangeStartValue;
            return decodedValue;
        }

        //-------------------------------------------------------------------------
        // Quaternion Encoding
        //-------------------------------------------------------------------------
        // Encode a quaternion into 48 bits -> 2 bits for largest component index, 3x15bit component values, 1 wasted bit
        // Storage:
        // m_data0 => [Largest Component High Bit] [15 bit for component 0]
        // m_data1 => [Largest Component Low Bit ] [15 bit for component 1]
        // m_data2 => [Unused Bit                ] [15 bit for component 2]

        struct EncodedQuaternion
        {
            KRG_SERIALIZE_MEMBERS( m_data0, m_data2 );

        private:

            static constexpr F32 const ValueRangeMin = -Math::OneDivSqrtTwo;
            static constexpr F32 const ValueRangeMax = Math::OneDivSqrtTwo;
            static constexpr F32 const ValueRangeLength = ValueRangeMax - ValueRangeMin;

        public:

            EncodedQuaternion() = default;

            EncodedQuaternion( U16 const& a, U16 const& b, U16 const& c )
                : m_data0( a )
                , m_data1( b )
                , m_data2( c )
            {}

            EncodedQuaternion( Quaternion const& value )
            {
                KRG_ASSERT( value.IsNormalized() );

                // X
                U16 largestValueIndex = 0;
                F32 maxAbsValue = Math::Abs( value.x );
                F32 signMultiplier = ( value.x < 0 ) ? -1.0f : 1.0f;

                // Y
                F32 absValue = Math::Abs( value.y );
                if ( absValue > maxAbsValue )
                {
                    largestValueIndex = 1;
                    maxAbsValue = absValue;
                    signMultiplier = ( value.y < 0 ) ? -1.0f : 1.0f;
                }

                // Z
                absValue = Math::Abs( value.z );
                if ( absValue > maxAbsValue )
                {
                    largestValueIndex = 2;
                    maxAbsValue = absValue;
                    signMultiplier = ( value.z < 0 ) ? -1.0f : 1.0f;
                }

                // W
                absValue = Math::Abs( value.w );
                if ( absValue > maxAbsValue )
                {
                    largestValueIndex = 3;
                    maxAbsValue = absValue;
                    signMultiplier = ( value.w < 0 ) ? -1.0f : 1.0f;
                }

                //-------------------------------------------------------------------------

                static constexpr F32 const rangeMultiplier15Bit = F32( 0x7FFF ) / ValueRangeLength;

                U16 a = 0, b = 0, c = 0;

                if ( largestValueIndex == 0 )
                {
                    a = (U16) Math::RoundToInt( ( ( value.y * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (U16) Math::RoundToInt( ( ( value.z * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (U16) Math::RoundToInt( ( ( value.w * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                }
                else if ( largestValueIndex == 1 )
                {
                    a = (U16) Math::RoundToInt( ( ( value.x * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (U16) Math::RoundToInt( ( ( value.z * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (U16) Math::RoundToInt( ( ( value.w * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );

                    m_data1 = 0x8000; // 1 << 16
                }
                else if ( largestValueIndex == 2 )
                {
                    a = (U16) Math::RoundToInt( ( ( value.x * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (U16) Math::RoundToInt( ( ( value.y * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (U16) Math::RoundToInt( ( ( value.w * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );

                    m_data0 = 0x8000; // 1 << 16
                }
                else if ( largestValueIndex == 3 )
                {
                    a = (U16) Math::RoundToInt( ( ( value.x * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (U16) Math::RoundToInt( ( ( value.y * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (U16) Math::RoundToInt( ( ( value.z * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );

                    m_data0 = 0x8000; // 1 << 16
                    m_data1 = 0x8000; // 1 << 16
                }
                else
                {
                    KRG_HALT();
                }

                //-------------------------------------------------------------------------

                m_data0 = m_data0 | a;
                m_data1 = m_data1 | b;
                m_data2 = c;
            }

            inline Quaternion ToQuaternion() const
            {
                U16 const largestValueIndex = ( m_data0 >> 14 & 0x0002 ) | m_data1 >> 15;
                U16 const da = m_data0 & 0x7FFF;
                U16 const db = m_data1 & 0x7FFF;
                U16 const dc = m_data2;

                static constexpr F32 const rangeMultiplier15Bit = ValueRangeLength / F32( 0x7FFF );
                F32 const a = ( da * rangeMultiplier15Bit ) + ValueRangeMin;
                F32 const b = ( db * rangeMultiplier15Bit ) + ValueRangeMin;
                F32 const c = ( dc * rangeMultiplier15Bit ) + ValueRangeMin;

                F32 const sum = ( a * a + b * b + c * c );
                KRG_ASSERT( sum <= 1.0f );
                F32 const d = Math::Sqrt( 1.0f - sum );

                if ( largestValueIndex == 0 )
                {
                    return Quaternion( d, a, b, c );
                }
                else if ( largestValueIndex == 1 )
                {
                    return Quaternion( a, d, b, c );
                }
                else if ( largestValueIndex == 2 )
                {
                    return Quaternion( a, b, d, c );
                }
                else if ( largestValueIndex == 3 )
                {
                    return Quaternion( a, b, c, d );
                }
                else
                {
                    KRG_HALT();
                    return Quaternion();
                }
            }

            inline U16 GetData0() const { return m_data0; }
            inline U16 GetData1() const { return m_data1; }
            inline U16 GetData2() const { return m_data2; }

        public:

            U16 m_data0 = 0;
            U16 m_data1 = 0;
            U16 m_data2 = 0;
        };
    }
}