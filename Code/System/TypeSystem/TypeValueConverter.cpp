#include "TypeValueConverter.h"
#include "TypeRegistry.h"


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

        void ConvertValueToString( TypeID typeID, void const* pValueData, String& strValue )
        {
            KRG_ASSERT( IsCoreType( typeID ) && pValueData != nullptr );
            strValue.clear();

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::ToString<bool>( *(bool*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::ToString( *(U8*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::ToString( *(S8*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::ToString( *(U16*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::ToString( *(S16*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::ToString( *(U32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::ToString( *(S32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::ToString( *(U64*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::ToString( *(S64*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::F32 || 
                      typeID == CoreTypes::Microseconds || 
                      typeID == CoreTypes::Milliseconds || 
                      typeID == CoreTypes::Seconds || 
                      typeID == CoreTypes::Percentage || 
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::ToString( *(F32*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::ToString( *(F64*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::ToString( *(String*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::ToString( *(StringID*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                TypeValueConverter::ToString( *(UUID*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::ToString( *(Color*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                return  TypeValueConverter::ToString( *(Float2*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::ToString( *(Float3*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::ToString( *(Float4*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::ToString( *(Transform*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::ToString( *(EulerAngles*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::ToString( *(Quaternion*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::ToString( *(Matrix*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::ToString( *(DataPath*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::ToString( *(ResourceTypeID*) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::ToString( *( Resource::ResourcePtr* ) pValueData, strValue );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::ToString( *(ResourceID*) pValueData, strValue );
            }
        }

        void ConvertStringToValue( TypeID typeID, String const& strValue, void* pValueData )
        {
            KRG_ASSERT( IsCoreType( typeID ) && pValueData != nullptr );

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::FromString( strValue, *(bool*) pValueData );
            }
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::FromString( strValue, *(U8*) pValueData );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::FromString( strValue, *(S8*) pValueData );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::FromString( strValue, *(U16*) pValueData );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::FromString( strValue, *(S16*) pValueData );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::FromString( strValue, *(U32*) pValueData );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::FromString( strValue, *(S32*) pValueData );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::FromString( strValue, *(U64*) pValueData );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::FromString( strValue, *(S64*) pValueData );
            }
            else if ( typeID == CoreTypes::F32 || 
                      typeID == CoreTypes::Microseconds || 
                      typeID == CoreTypes::Milliseconds || 
                      typeID == CoreTypes::Seconds || 
                      typeID == CoreTypes::Percentage || 
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::FromString( strValue, *(F32*) pValueData );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::FromString( strValue, *(F64*) pValueData );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::FromString( strValue, *(String*) pValueData );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::FromString( strValue, *(StringID*) pValueData );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::FromString( strValue, *(Color*) pValueData );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                TypeValueConverter::FromString( strValue, *(Float2*) pValueData );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::FromString( strValue, *(Float3*) pValueData );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::FromString( strValue, *(Float4*) pValueData );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::FromString( strValue, *(Transform*) pValueData );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::FromString( strValue, *(EulerAngles*) pValueData );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::FromString( strValue, *(Quaternion*) pValueData );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::FromString( strValue, *(Matrix*) pValueData );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::FromString( strValue, *(DataPath*) pValueData );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::FromString( strValue, *( ResourceTypeID* ) pValueData );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::FromString( strValue, *( Resource::ResourcePtr* ) pValueData );
            }
            else if( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::FromString( strValue, *( ResourceID* ) pValueData );
            }
        }

        //-------------------------------------------------------------------------

        void ConvertStringToByteArray( TypeID typeID, String const& strValue, TVector<Byte>& byteArray )
        {
            KRG_ASSERT( IsCoreType( typeID ) && !strValue.empty() );

            byteArray.clear();

            //-------------------------------------------------------------------------

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::ToByteArray<bool>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::ToByteArray<U8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::ToByteArray<S8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::ToByteArray<U16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::ToByteArray<S16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::ToByteArray<U32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::ToByteArray<S32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::ToByteArray<U64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::ToByteArray<S64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::F32 || 
                      typeID == CoreTypes::Microseconds ||
                      typeID == CoreTypes::Milliseconds ||
                      typeID == CoreTypes::Seconds ||
                      typeID == CoreTypes::Percentage ||
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::ToByteArray<F32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::ToByteArray<F64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::ToByteArray<String>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::ToByteArray<StringID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                TypeValueConverter::ToByteArray<UUID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::ToByteArray<Color>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                TypeValueConverter::ToByteArray<Float2>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::ToByteArray<Float3>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::ToByteArray<Float4>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::ToByteArray<Transform>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::ToByteArray<EulerAngles>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::ToByteArray<Quaternion>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::ToByteArray<Matrix>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::ToByteArray<DataPath>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::ToByteArray<ResourceTypeID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::ToByteArray<Resource::ResourcePtr>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::ToByteArray<ResourceID>( strValue, byteArray );
            }
        }

        void ConvertByteArrayToValue( TypeID typeID, TVector<Byte> const& byteArray, void* pValue )
        {
            KRG_ASSERT( IsCoreType( typeID ) && !byteArray.empty() && pValue != nullptr );

            if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::FromByteArray<bool>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::FromByteArray<U8>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::FromByteArray<S8>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::FromByteArray<U16>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::FromByteArray<S16>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::FromByteArray<U32>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::FromByteArray<S32>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::FromByteArray<U64>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::FromByteArray<S64>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::F32 || 
                      typeID == CoreTypes::Microseconds ||
                      typeID == CoreTypes::Milliseconds ||
                      typeID == CoreTypes::Seconds ||
                      typeID == CoreTypes::Percentage ||
                      typeID == CoreTypes::Degrees || 
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::FromByteArray<F32>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::FromByteArray<F64>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::FromByteArray<String>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::FromByteArray<StringID>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                TypeValueConverter::FromByteArray<UUID>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::FromByteArray<Color>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                TypeValueConverter::FromByteArray<Float2>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::FromByteArray<Float3>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::FromByteArray<Float4>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::FromByteArray<Transform>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::FromByteArray<EulerAngles>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::FromByteArray<Quaternion>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::FromByteArray<Matrix>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::FromByteArray<DataPath>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::FromByteArray<ResourceTypeID>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::FromByteArray<Resource::ResourcePtr>( byteArray, pValue );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::FromByteArray<ResourceID>( byteArray, pValue );
            }
        }

        bool IsValidStringValueForType( TypeSystem::TypeID typeID, String const& strValue )
        {
            KRG_ASSERT( IsCoreType( typeID ) && !strValue.empty() );

            // TODO
            /*if ( typeID == CoreTypes::Bool )
            {
                TypeValueConverter::ToByteArray<bool>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U8 )
            {
                TypeValueConverter::ToByteArray<U8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S8 )
            {
                TypeValueConverter::ToByteArray<S8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U16 )
            {
                TypeValueConverter::ToByteArray<U16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S16 )
            {
                TypeValueConverter::ToByteArray<S16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U32 )
            {
                TypeValueConverter::ToByteArray<U32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S32 )
            {
                TypeValueConverter::ToByteArray<S32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::U64 )
            {
                TypeValueConverter::ToByteArray<U64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::S64 )
            {
                TypeValueConverter::ToByteArray<S64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::F32 ||
                      typeID == CoreTypes::Microseconds ||
                      typeID == CoreTypes::Milliseconds ||
                      typeID == CoreTypes::Seconds ||
                      typeID == CoreTypes::Percentage ||
                      typeID == CoreTypes::Degrees ||
                      typeID == CoreTypes::Radians )
            {
                TypeValueConverter::ToByteArray<F32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::F64 )
            {
                TypeValueConverter::ToByteArray<F64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::String )
            {
                TypeValueConverter::ToByteArray<String>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                TypeValueConverter::ToByteArray<StringID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                TypeValueConverter::ToByteArray<UUID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Color )
            {
                TypeValueConverter::ToByteArray<Color>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                TypeValueConverter::ToByteArray<Float2>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                TypeValueConverter::ToByteArray<Float3>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                TypeValueConverter::ToByteArray<Float4>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                TypeValueConverter::ToByteArray<Transform>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                TypeValueConverter::ToByteArray<EulerAngles>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                TypeValueConverter::ToByteArray<Quaternion>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                TypeValueConverter::ToByteArray<Matrix>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                TypeValueConverter::ToByteArray<DataPath>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                TypeValueConverter::ToByteArray<ResourceTypeID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                TypeValueConverter::ToByteArray<Resource::ResourcePtr>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                TypeValueConverter::ToByteArray<ResourceID>( strValue, byteArray );
            }*/

            return true;
        }
    }
}