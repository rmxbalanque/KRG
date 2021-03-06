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
        inline uint16 EncodeUnsignedNormalizedFloat( float value )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Encoding into more than 16bits is not allowed" );

            KRG_ASSERT( value >= 0 && value <= 1.0f );

            float const quantizedValue = value * ( ( 1 << ( N ) ) - 1 ) + 0.5f;
            return uint16( quantizedValue );
        }

        template<unsigned N>
        inline float DecodeUnsignedNormalizedFloat( uint16 encodedValue )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Decoding from more than 16bits is not allowed" );
            return encodedValue / float( ( 1 << ( N ) ) - 1 );
        }

        template<unsigned N>
        inline uint16 EncodeSignedNormalizedFloat( float value )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Encoding into more than 16bits is not allowed" );

            KRG_ASSERT( value >= -1.0f && value <= 1.0f );

            // Track the sign and create a unsigned normalized float
            uint16 sign = 0;
            if ( value < 0 )
            {
                sign = 1;
                value = -value;
            }

            return sign | ( EncodeUnsignedNormalizedFloat<N - 1>( value ) << 1 );
        }

        template<unsigned N>
        inline float DecodeSignedNormalizedFloat( uint16 encodedValue )
        {
            static_assert( N > 0, "Invalid number of bits specified" );
            static_assert( N <= 16, "Decoding from more than 16bits is not allowed" );

            float const sign = float( encodedValue & 1 ? -1 : 1 );
            return DecodeUnsignedNormalizedFloat<N - 1>( encodedValue >> 1 ) * sign;
        }

        //-------------------------------------------------------------------------
        // Float quantization
        //-------------------------------------------------------------------------
        // 32 bit float to 16bit uint

        inline uint16 EncodeFloat( float value, float const quantizationRangeStartValue, float const quantizationRangeLength )
        {
            KRG_ASSERT( quantizationRangeLength != 0 );

            float const normalizedValue = ( value - quantizationRangeStartValue ) / quantizationRangeLength;
            uint16 const encodedValue = EncodeUnsignedNormalizedFloat<16>( normalizedValue );
            return encodedValue;
        }

        inline float DecodeFloat( uint16 encodedValue, float const quantizationRangeStartValue, float const quantizationRangeLength )
        {
            KRG_ASSERT( quantizationRangeLength != 0 );

            float const normalizedValue = DecodeUnsignedNormalizedFloat<16>( encodedValue );
            float const decodedValue = ( normalizedValue * quantizationRangeLength ) + quantizationRangeStartValue;
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

            static constexpr float const ValueRangeMin = -Math::OneDivSqrtTwo;
            static constexpr float const ValueRangeMax = Math::OneDivSqrtTwo;
            static constexpr float const ValueRangeLength = ValueRangeMax - ValueRangeMin;

        public:

            EncodedQuaternion() = default;

            EncodedQuaternion( uint16 const& a, uint16 const& b, uint16 const& c )
                : m_data0( a )
                , m_data1( b )
                , m_data2( c )
            {}

            EncodedQuaternion( Quaternion const& value )
            {
                KRG_ASSERT( value.IsNormalized() );

                // X
                uint16 largestValueIndex = 0;
                float maxAbsValue = Math::Abs( value.x );
                float signMultiplier = ( value.x < 0 ) ? -1.0f : 1.0f;

                // Y
                float absValue = Math::Abs( value.y );
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

                static constexpr float const rangeMultiplier15Bit = float( 0x7FFF ) / ValueRangeLength;

                uint16 a = 0, b = 0, c = 0;

                if ( largestValueIndex == 0 )
                {
                    a = (uint16) Math::RoundToInt( ( ( value.y * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (uint16) Math::RoundToInt( ( ( value.z * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (uint16) Math::RoundToInt( ( ( value.w * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                }
                else if ( largestValueIndex == 1 )
                {
                    a = (uint16) Math::RoundToInt( ( ( value.x * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (uint16) Math::RoundToInt( ( ( value.z * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (uint16) Math::RoundToInt( ( ( value.w * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );

                    m_data1 = 0x8000; // 1 << 16
                }
                else if ( largestValueIndex == 2 )
                {
                    a = (uint16) Math::RoundToInt( ( ( value.x * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (uint16) Math::RoundToInt( ( ( value.y * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (uint16) Math::RoundToInt( ( ( value.w * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );

                    m_data0 = 0x8000; // 1 << 16
                }
                else if ( largestValueIndex == 3 )
                {
                    a = (uint16) Math::RoundToInt( ( ( value.x * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    b = (uint16) Math::RoundToInt( ( ( value.y * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );
                    c = (uint16) Math::RoundToInt( ( ( value.z * signMultiplier ) - ValueRangeMin ) * rangeMultiplier15Bit );

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
                uint16 const largestValueIndex = ( m_data0 >> 14 & 0x0002 ) | m_data1 >> 15;
                uint16 const da = m_data0 & 0x7FFF;
                uint16 const db = m_data1 & 0x7FFF;
                uint16 const dc = m_data2;

                static constexpr float const rangeMultiplier15Bit = ValueRangeLength / float( 0x7FFF );
                float const a = ( da * rangeMultiplier15Bit ) + ValueRangeMin;
                float const b = ( db * rangeMultiplier15Bit ) + ValueRangeMin;
                float const c = ( dc * rangeMultiplier15Bit ) + ValueRangeMin;

                float const sum = ( a * a + b * b + c * c );
                KRG_ASSERT( sum <= 1.0f );
                float const d = Math::Sqrt( 1.0f - sum );

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

            inline uint16 GetData0() const { return m_data0; }
            inline uint16 GetData1() const { return m_data1; }
            inline uint16 GetData2() const { return m_data2; }

        public:

            uint16 m_data0 = 0;
            uint16 m_data1 = 0;
            uint16 m_data2 = 0;
        };
    }
}