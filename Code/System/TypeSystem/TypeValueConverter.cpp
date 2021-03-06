#include "TypeValueConverter.h"
#include "TypeRegistry.h"
#include "System/Core/Types/Percentage.h"


//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        void TypeValueConverter::StringToFloatArray( String const& str, char const* delim, int32 const numFloats, float* pFloats )
        {
            char substr[128] = { 0 };
            int32 resIdx = 0;
            auto delimLength = strlen( delim );

            bool complete = false;
            size_t startIdx = 0;
            while ( resIdx < numFloats && !complete )
            {
                auto endIdx = str.find_first_of( delim, startIdx );
                if ( endIdx == String::npos )
                {
                    endIdx = str.length();
                    complete = true;
                }

                auto sizeToCopy = endIdx - startIdx;
                KRG_ASSERT( sizeToCopy < 128 );
                memcpy( substr, &str.c_str()[startIdx], sizeToCopy );
                substr[sizeToCopy] = '\0';

                pFloats[resIdx++] = (float) atof( substr );
                startIdx = endIdx + delimLength;
            }
        }

        void TypeValueConverter::FloatArrayToString( float const* pFloats, char const* delim, int32 const numFloats, String& outStr )
        {
            outStr.clear();

            for ( int32 i = 0; i < numFloats; i++ )
            {
                outStr += eastl::to_string( pFloats[i] );

                if ( i != ( numFloats - 1 ) )
                {
                    outStr += delim;
                }
            }
        }

        //-------------------------------------------------------------------------

        void TypeValueConverter::ConvertValueToString( TypeID typeID, void const* pValueData, String& strValue )
        {
            KRG_ASSERT( IsCoreType( typeID ) && pValueData != nullptr );
            strValue.clear();

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::ValueToString<bool>( *(bool*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                TypeValueConverter::ValueToString( *(uint8*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                TypeValueConverter::ValueToString( *(int8*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                TypeValueConverter::ValueToString( *(uint16*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                TypeValueConverter::ValueToString( *(int16*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                TypeValueConverter::ValueToString( *(uint32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                TypeValueConverter::ValueToString( *(int32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                TypeValueConverter::ValueToString( *(uint64*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                TypeValueConverter::ValueToString( *(int64*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Float || 
                      typeID == CoreTypes::Microseconds || 
                      typeID == CoreTypes::Milliseconds || 
                      typeID == CoreTypes::Seconds || 
                      typeID == CoreTypes::Percentage || 
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::ValueToString( *(float*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Double )
            {
                TypeValueConverter::ValueToString( *(double*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::ValueToString( *(String*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::ValueToString( *(StringID*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                TypeValueConverter::ValueToString( *(UUID*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::ValueToString( *(Color*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                return  TypeValueConverter::ValueToString( *(Float2*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::ValueToString( *(Float3*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::ValueToString( *(Float4*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::ValueToString( *(Transform*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::ValueToString( *(EulerAngles*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::ValueToString( *(Quaternion*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::ValueToString( *(Matrix*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::ValueToString( *(DataPath*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::ValueToString( *(ResourceTypeID*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::ValueToString( *( Resource::ResourcePtr* ) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::ValueToString( *(ResourceID*) pValueData, strValue );
            }
        }

        void TypeValueConverter::ConvertValueToByteArray( TypeSystem::TypeID typeID, void const* pValueData, TVector<Byte>& byteArray )
        {
            KRG_UNIMPLEMENTED_FUNCTION();
        }

        void TypeValueConverter::ConvertStringToValue( TypeID typeID, String const& strValue, void* pValueData )
        {
            KRG_ASSERT( IsCoreType( typeID ) && pValueData != nullptr );

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::StringToValue( strValue, *(bool*) pValueData );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                TypeValueConverter::StringToValue( strValue, *(uint8*) pValueData );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                TypeValueConverter::StringToValue( strValue, *(int8*) pValueData );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                TypeValueConverter::StringToValue( strValue, *(uint16*) pValueData );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                TypeValueConverter::StringToValue( strValue, *(int16*) pValueData );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                TypeValueConverter::StringToValue( strValue, *(uint32*) pValueData );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                TypeValueConverter::StringToValue( strValue, *(int32*) pValueData );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                TypeValueConverter::StringToValue( strValue, *(uint64*) pValueData );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                TypeValueConverter::StringToValue( strValue, *(int64*) pValueData );
            }
            else if ( typeID == CoreTypes::Float || 
                      typeID == CoreTypes::Microseconds || 
                      typeID == CoreTypes::Milliseconds || 
                      typeID == CoreTypes::Seconds || 
                      typeID == CoreTypes::Percentage || 
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::StringToValue( strValue, *(float*) pValueData );
            }
            else if ( typeID == CoreTypes::Double )
            {
                TypeValueConverter::StringToValue( strValue, *(double*) pValueData );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::StringToValue( strValue, *(String*) pValueData );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::StringToValue( strValue, *(StringID*) pValueData );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::StringToValue( strValue, *(Color*) pValueData );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                TypeValueConverter::StringToValue( strValue, *(Float2*) pValueData );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::StringToValue( strValue, *(Float3*) pValueData );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::StringToValue( strValue, *(Float4*) pValueData );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::StringToValue( strValue, *(Transform*) pValueData );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::StringToValue( strValue, *(EulerAngles*) pValueData );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::StringToValue( strValue, *(Quaternion*) pValueData );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::StringToValue( strValue, *(Matrix*) pValueData );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::StringToValue( strValue, *(DataPath*) pValueData );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::StringToValue( strValue, *( ResourceTypeID* ) pValueData );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::StringToValue( strValue, *( Resource::ResourcePtr* ) pValueData );
            }
            else if( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::StringToValue( strValue, *( ResourceID* ) pValueData );
            }
        }

        //-------------------------------------------------------------------------

        void TypeValueConverter::ConvertStringToByteArray( TypeID typeID, String const& strValue, TVector<Byte>& byteArray )
        {
            KRG_ASSERT( IsCoreType( typeID ) && !strValue.empty() );

            byteArray.clear();

            //-------------------------------------------------------------------------

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::StringToByteArray<bool>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                TypeValueConverter::StringToByteArray<uint8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                TypeValueConverter::StringToByteArray<int8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                TypeValueConverter::StringToByteArray<uint16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                TypeValueConverter::StringToByteArray<int16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                TypeValueConverter::StringToByteArray<uint32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                TypeValueConverter::StringToByteArray<int32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                TypeValueConverter::StringToByteArray<uint64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                TypeValueConverter::StringToByteArray<int64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float || 
                      typeID == CoreTypes::Microseconds ||
                      typeID == CoreTypes::Milliseconds ||
                      typeID == CoreTypes::Seconds ||
                      typeID == CoreTypes::Percentage ||
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::StringToByteArray<float>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Double )
            {
                TypeValueConverter::StringToByteArray<double>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::StringToByteArray<String>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::StringToByteArray<StringID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                TypeValueConverter::StringToByteArray<UUID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::StringToByteArray<Color>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                TypeValueConverter::StringToByteArray<Float2>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::StringToByteArray<Float3>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::StringToByteArray<Float4>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::StringToByteArray<Transform>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::StringToByteArray<EulerAngles>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::StringToByteArray<Quaternion>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::StringToByteArray<Matrix>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::StringToByteArray<DataPath>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::StringToByteArray<ResourceTypeID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::StringToByteArray<Resource::ResourcePtr>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::StringToByteArray<ResourceID>( strValue, byteArray );
            }
        }

        void TypeValueConverter::ConvertByteArrayToValue( TypeID typeID, TVector<Byte> const& byteArray, void* pValue )
        {
            KRG_ASSERT( IsCoreType( typeID ) && !byteArray.empty() && pValue != nullptr );

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (bool*) pValue );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (uint8*) pValue );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (int8*) pValue );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (uint16*) pValue );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (int16*) pValue );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (uint32*) pValue );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (int32*) pValue );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (uint64*) pValue );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                TypeValueConverter::ByteArrayToValue<int64>( byteArray, (int64*) pValue );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                TypeValueConverter::ByteArrayToValue<int64>( byteArray, (int64*) pValue );
            }
            else if ( typeID == CoreTypes::Float )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (float*) pValue );
            }
            else if ( typeID == CoreTypes::Microseconds )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Microseconds*) pValue );
            }
            else if ( typeID == CoreTypes::Milliseconds )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Milliseconds*) pValue );
            }
            else if ( typeID == CoreTypes::Seconds )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Seconds*) pValue );
            }
            else if ( typeID == CoreTypes::Percentage )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Percentage*) pValue );
            }
            else if ( typeID == CoreTypes::Degrees )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Degrees*) pValue );
            }
            else if ( typeID == CoreTypes::Radians )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Radians*) pValue );
            }
            else if ( typeID == CoreTypes::Double )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (double*) pValue );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (String*) pValue );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (StringID*) pValue );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (UUID*) pValue );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Color*) pValue );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Float2*) pValue );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Float3*) pValue );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Float4*) pValue );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Transform*) pValue );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (EulerAngles*) pValue );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Quaternion*) pValue );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Matrix*) pValue );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (DataPath*) pValue );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (ResourceTypeID*) pValue );
            }
            else if ( typeID == CoreTypes::ResourcePtr )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (Resource::ResourcePtr*) pValue );
            }
            else if( typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (TResourcePtr<Resource::IResource>*) pValue );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (ResourceID*) pValue );
            }
        }

        bool TypeValueConverter::IsValidStringValueForType( TypeSystem::TypeID typeID, String const& strValue )
        {
            KRG_ASSERT( IsCoreType( typeID ) && !strValue.empty() );

            //KRG_UNIMPLEMENTED_FUNCTION();

            return true;
        }
    }
}