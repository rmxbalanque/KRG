#pragma once

#include "_Module/API.h"
#include "TypeInfo.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Types/Time.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        class KRG_SYSTEM_TYPESYSTEM_API TypeValueConverter
        {
        public:

            // Typed conversions
            //-------------------------------------------------------------------------

            template<typename T>
            inline static void ValueToString( T const& value, String& outStr ) { KRG_UNIMPLEMENTED_FUNCTION(); }

            template<typename T>
            inline static void StringToValue( String const& str, T& outValue ) { KRG_UNIMPLEMENTED_FUNCTION(); }

            template<typename T>
            inline static void ValueToByteArray( T const& value, TVector<Byte>& outArray )
            {
                Serialization::BinaryMemoryArchive archive( Serialization::Mode::Write, outArray );
                archive << value;
            }

            template<typename T>
            inline static void ByteArrayToValue( TVector<Byte> const& valueByteData, T& outValue )
            {
                Serialization::BinaryMemoryArchive archive( Serialization::Mode::Read, const_cast<TVector<Byte>&>( valueByteData ) );
                archive >> outValue;
            }

            template<typename T>
            inline static void ByteArrayToValue( TVector<Byte> const& valueByteData, T* pOutValue )
            {
                ByteArrayToValue( valueByteData, *pOutValue );
            }

            template<typename T>
            inline static void StringToByteArray( String const& str, TVector<Byte>& outArray )
            {
                T value;
                StringToValue<T>( str, value );
                ValueToByteArray( value, outArray );
            }

            // Untyped conversions
            //-------------------------------------------------------------------------

            static void ConvertValueToString( TypeSystem::TypeID typeID, void const* pValueData, String& strValue );
            static void ConvertValueToByteArray( TypeSystem::TypeID typeID, void const* pValueData, TVector<Byte>& byteArray );
            static void ConvertStringToValue( TypeSystem::TypeID typeID, String const& strValue, void* pValueData );
            static void ConvertStringToByteArray( TypeSystem::TypeID typeID, String const& strValue, TVector<Byte>& byteArray );
            static void ConvertByteArrayToValue( TypeSystem::TypeID typeID, TVector<Byte> const& byteArray, void* pValue );
            static bool IsValidStringValueForType( TypeSystem::TypeID typeID, String const& strValue );

        private:

            static void StringToFloatArray( String const& str, char const* delim, S32 const numFloats, F32* pFloats );
            static void FloatArrayToString( F32 const* pFloats, char const* delim, S32 const numFloats, String& outStr );
        };

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( bool const& value, String& outStr )
        {
            outStr = value ? "True" : "False";
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, bool& value )
        {
            String lowerString = str;
            lowerString.make_lower();
            value = ( lowerString == "true" );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( U8 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, U8& value )
        {
            value = (U8) strtoul( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( U16 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, U16& value )
        {
            value = (U16) strtoul( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( U32 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, U32& value )
        {
            value = (U32) strtoul( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( U64 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, U64& value )
        {
            value = (U64) strtoull( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( S8 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, S8& value )
        {
            value = (S8) strtol( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( S16 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, S16& value )
        {
            value = (S16) strtol( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( S32 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, S32& value )
        {
            value = (S32) strtol( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( S64 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, S64& value )
        {
            value = (S64) strtoll( str.c_str(), nullptr, 10 );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( F32 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, F32& value )
        {
            value = (F32) atof( str.c_str() );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( F64 const& value, String& outStr )
        {
            outStr = eastl::to_string( value );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, F64& value )
        {
            value = (F64) atof( str.c_str() );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( String const& value, String& outStr )
        {
            outStr = value;
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, String& value )
        {
            value = str;
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( StringID const& value, String& outStr )
        {
            char const* pStr = value.ToString();
            KRG_ASSERT( pStr != nullptr );
            outStr = pStr;
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, StringID& value )
        {
            value = StringID( str.c_str() );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Color const& value, String& outStr )
        {
            outStr.sprintf( "%02X%02X%02X%02X", value.m_byteColor.m_r, value.m_byteColor.m_g, value.m_byteColor.m_b, value.m_byteColor.m_a );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Color& value )
        {
            U32 const colorValue = (U32) strtoul( str.c_str(), nullptr, 16 );
            value = Color( colorValue );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( UUID const& value, String& outStr )
        {
            outStr = value.ToString();
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, UUID& value )
        {
            value = UUID( str );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Float2 const& value, String& outStr )
        {
            FloatArrayToString( &value[0], ",", 2, outStr );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Float2& value )
        {
            StringToFloatArray( str, ",", 2, &value[0] );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Float3 const& value, String& outStr )
        {
            FloatArrayToString( &value[0], ",", 3, outStr );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Float3& value )
        {
            StringToFloatArray( str, ",", 3, &value[0] );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Float4 const& value, String& outStr )
        {
            FloatArrayToString( &value[0], ",", 4, outStr );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Float4& value )
        {
            StringToFloatArray( str, ",", 4, &value[0] );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Matrix const& value, String& outStr )
        {
            // Handle uninitialized matrix
            if ( value.IsOrthonormal() )
            {
                auto eulerAngles = value.ToEulerAngles();

                F32 floatData[9];
                (Float3&) floatData = Float3( (F32) eulerAngles.x.ToDegrees(), (F32) eulerAngles.y.ToDegrees(), (F32) eulerAngles.z.ToDegrees() );
                (Float3&) floatData[3] = value.GetTranslation().ToFloat3();
                (Float3&) floatData[6] = value.GetScale().ToFloat3();

                String str;
                FloatArrayToString( floatData, ",", 9, str );
                outStr = str;
            }
            else
            {
                outStr = "0,0,0,0,0,0,0,0,0";
            }
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Matrix& value )
        {
            F32 floatData[9];
            StringToFloatArray( str, ",", 9, floatData );

            EulerAngles const rotation( floatData[0], floatData[1], floatData[2] );
            Vector const translation = Vector( floatData[3], floatData[4], floatData[5] );
            value = Matrix( Quaternion( rotation ), translation );

            Vector const scale = Vector( floatData[6], floatData[7], floatData[8] );
            value.SetScale( scale );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Transform const& value, String& outStr )
        {
            auto eulerAngles = value.GetRotation().ToEulerAngles();

            F32 floatData[9];

            floatData[0] = (F32) eulerAngles.x.ToDegrees();
            floatData[1] = (F32) eulerAngles.y.ToDegrees();
            floatData[2] = (F32) eulerAngles.z.ToDegrees();

            floatData[3] = value.GetTranslation().x;
            floatData[4] = value.GetTranslation().y;
            floatData[5] = value.GetTranslation().z;

            floatData[6] = value.GetScale().x;
            floatData[7] = value.GetScale().y;
            floatData[8] = value.GetScale().z;

            //-------------------------------------------------------------------------

            String str;
            FloatArrayToString( floatData, ",", 9, str );
            outStr = str;
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Transform& value )
        {
            F32 floatData[9];
            StringToFloatArray( str, ",", 9, floatData );

            // Set rotation
            EulerAngles const rotationEulerAngles( floatData[0], floatData[1], floatData[2] );
            value.SetRotation( Quaternion( rotationEulerAngles ) );

            // Set translation and scale
            value.SetTranslation( Vector( floatData[3], floatData[4], floatData[5], 1.0f ) );
            value.SetScale( Vector( floatData[6], floatData[7], floatData[8], 1.0f ) );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( EulerAngles const& value, String& outStr )
        {
            FloatArrayToString( (float*) &value.x, ",", 3, outStr );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, EulerAngles& value )
        {
            StringToFloatArray( str, ",", 3, (float*) &value.x );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Quaternion const& value, String& outStr )
        {
            EulerAngles angles = value.ToEulerAngles();
            FloatArrayToString( (float*) &angles.x, ",", 3, outStr );
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Quaternion& value )
        {
            EulerAngles angles;
            StringToFloatArray( str, ",", 3, (float*) &angles.x );
            value = Quaternion( angles );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( ResourceTypeID const& value, String& outStr )
        {
            outStr = value.ToString();
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, ResourceTypeID& value )
        {
            value = ResourceTypeID( str );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( DataPath const& value, String& outStr )
        {
            outStr = value.c_str();
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, DataPath& value )
        {
            value = DataPath( str );
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( ResourceID const& value, String& outStr )
        {
            outStr = value.ToString();
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, ResourceID& value )
        {
            if ( str.empty() )
            {
                value = ResourceID();
            }
            else
            {
                KRG_ASSERT( ResourceID::IsValidResourceIDString( str ) );
                value = ResourceID( str );
            }
        }

        //-------------------------------------------------------------------------

        template<>
        inline void TypeValueConverter::ValueToString( Resource::ResourcePtr const& value, String& outStr )
        {
            outStr = value.GetResourceID().ToString();
        }

        template<>
        inline void TypeValueConverter::StringToValue( String const& str, Resource::ResourcePtr& value )
        {
            if ( str.empty() )
            {
                value = Resource::ResourcePtr();
            }
            else
            {
                KRG_ASSERT( ResourceID::IsValidResourceIDString( str ) );
                ResourceID const ID( str );
                value = Resource::ResourcePtr( ID );
            }
        }
    }
}