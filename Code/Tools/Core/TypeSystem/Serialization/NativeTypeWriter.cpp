#if _WIN32
#include "NativeTypeWriter.h"
#include "TypeSerializationCommon.h"
#include "System/TypeSystem/CoreTypeConversions.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    NativeTypeWriter::NativeTypeWriter( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    void NativeTypeWriter::Reset()
    {
        m_scratchBuffer.clear();
        m_stringBuffer.Clear();
        m_numTypesSerialized = 0;
    }

    bool NativeTypeWriter::WriteToFile( FileSystem::Path const& outPath )
    {
        KRG_ASSERT( outPath.IsFilePath() );

        if ( m_numTypesSerialized > 1 )
        {
            m_writer.EndArray();
        }

        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( outPath );

        FILE* pFile = fopen( outPath, "wb" );
        if ( pFile == nullptr )
        {
            Reset();
            return false;
        }

        fwrite( m_stringBuffer.GetString(), m_stringBuffer.GetSize(), 1, pFile );
        fclose( pFile );

        Reset();
        return true;
    }

    void NativeTypeWriter::PrepareBuffer()
    {
        if ( m_numTypesSerialized == 1 )
        {
            String const firstValueSerialized = m_stringBuffer.GetString();

            //-------------------------------------------------------------------------

            m_stringBuffer.Clear();
            m_writer.StartArray();
            m_writer.RawValue( firstValueSerialized.c_str(), firstValueSerialized.length(), Type::kObjectType );
        }
    }

    void NativeTypeWriter::SerializeType( TypeID typeID, void const* pTypeData )
    {
        KRG_ASSERT( !IsCoreType( typeID ) );
        auto const pTypeInfo = m_typeRegistry.GetTypeInfo( typeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        m_writer.StartObject();

        // Every type has to have a type ID
        m_writer.Key( Serialization::TypeSerialization::Key_TypeID );
        m_writer.String( typeID.GetAsStringID().ToString() );

        //-------------------------------------------------------------------------

        for ( auto const& entry : pTypeInfo->m_properties )
        {
            PropertyInfo const& propInfo = entry.second;

            // Write Key
            const char* pPropertyName = propInfo.m_ID.ToString();
            KRG_ASSERT( pPropertyName != nullptr );
            m_writer.Key( pPropertyName );

            // Write Value
            auto pPropertyDataAddress = propInfo.GetPropertyAddress( pTypeData );
            if ( propInfo.IsArrayProperty() )
            {
                size_t const elementByteSize = m_typeRegistry.GetTypeByteSize( propInfo.m_typeID );
                KRG_ASSERT( elementByteSize > 0 );

                m_writer.StartArray();

                size_t numArrayElements = 0;
                Byte const* pElementAddress = nullptr;

                // Static array
                if ( propInfo.IsStaticArrayProperty() )
                {
                    numArrayElements = propInfo.m_size / elementByteSize;
                    pElementAddress = propInfo.GetPropertyAddress<Byte>( pTypeData );
                }
                else // Dynamic array
                {
                    TVector<Byte> const& dynamicArray = *propInfo.GetPropertyAddress< TVector<Byte> >( pTypeData );
                    size_t const arrayByteSize = dynamicArray.size();
                    numArrayElements = arrayByteSize / elementByteSize;
                    pElementAddress = dynamicArray.data();
                }

                // Write array elements
                for ( auto i = 0; i < numArrayElements; i++ )
                {
                    SerializeProperty( propInfo, pElementAddress );
                    pElementAddress += elementByteSize;
                }

                m_writer.EndArray();
            }
            else
            {
                SerializeProperty( propInfo, pPropertyDataAddress );
            }
        }

        m_writer.EndObject();
    }

    void NativeTypeWriter::SerializeProperty( PropertyInfo const& propertyInfo, void const* pTypeData )
    {
        if ( IsCoreType( propertyInfo.m_typeID ) )
        {
            Conversion::ConvertNativeTypeToString( m_typeRegistry, propertyInfo, pTypeData, m_scratchBuffer );
            m_writer.String( m_scratchBuffer.c_str() );
        }
        else
        {
            SerializeType( propertyInfo.m_typeID, pTypeData );
        }
    }
}

#endif