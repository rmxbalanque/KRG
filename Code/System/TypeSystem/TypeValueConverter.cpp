#include "TypeValueConverter.h"
#include "TypeRegistry.h"
#include "System/Core/Types/Percentage.h"


//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        void TypeValueConverter::StringToFloatArray( String const& str, char const* delim, S32 const numFloats, F32* pFloats )
        {
            char substr[128] = { 0 };
            S32 resIdx = 0;
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

                pFloats[resIdx++] = (F32) atof( substr );
                startIdx = endIdx + delimLength;
            }
        }

        void TypeValueConverter::FloatArrayToString( F32 const* pFloats, char const* delim, S32 const numFloats, String& outStr )
        {
            outStr.clear();

            for ( S32 i = 0; i < numFloats; i++ )
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
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::ValueToString( *(U8*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::ValueToString( *(S8*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::ValueToString( *(U16*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::ValueToString( *(S16*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::ValueToString( *(U32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::ValueToString( *(S32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::ValueToString( *(U64*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::ValueToString( *(S64*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::F32 || 
                      typeID == CoreTypes::Microseconds || 
                      typeID == CoreTypes::Milliseconds || 
                      typeID == CoreTypes::Seconds || 
                      typeID == CoreTypes::Percentage || 
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::ValueToString( *(F32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::ValueToString( *(F64*) pValueData, strValue );
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
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::StringToValue( strValue, *(U8*) pValueData );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::StringToValue( strValue, *(S8*) pValueData );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::StringToValue( strValue, *(U16*) pValueData );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::StringToValue( strValue, *(S16*) pValueData );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::StringToValue( strValue, *(U32*) pValueData );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::StringToValue( strValue, *(S32*) pValueData );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::StringToValue( strValue, *(U64*) pValueData );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::StringToValue( strValue, *(S64*) pValueData );
            }
            else if ( typeID == CoreTypes::F32 || 
                      typeID == CoreTypes::Microseconds || 
                      typeID == CoreTypes::Milliseconds || 
                      typeID == CoreTypes::Seconds || 
                      typeID == CoreTypes::Percentage || 
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::StringToValue( strValue, *(F32*) pValueData );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::StringToValue( strValue, *(F64*) pValueData );
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
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::StringToByteArray<U8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::StringToByteArray<S8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::StringToByteArray<U16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::StringToByteArray<S16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::StringToByteArray<U32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::StringToByteArray<S32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::StringToByteArray<U64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::StringToByteArray<S64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::F32 || 
                      typeID == CoreTypes::Microseconds ||
                      typeID == CoreTypes::Milliseconds ||
                      typeID == CoreTypes::Seconds ||
                      typeID == CoreTypes::Percentage ||
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::StringToByteArray<F32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::StringToByteArray<F64>( strValue, byteArray );
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
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (U8*) pValue );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (S8*) pValue );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (U16*) pValue );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (S16*) pValue );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (U32*) pValue );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (S32*) pValue );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (U64*) pValue );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::ByteArrayToValue<S64>( byteArray, (S64*) pValue );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::ByteArrayToValue<S64>( byteArray, (S64*) pValue );
            }
            else if ( typeID == CoreTypes::F32 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (F32*) pValue );
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
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::ByteArrayToValue( byteArray, (F64*) pValue );
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