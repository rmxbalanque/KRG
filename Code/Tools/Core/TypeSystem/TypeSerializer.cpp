#if _WIN32
#include "TypeSerializer.h"
#include "System/TypeSystem/TypeValueConverter.h"
#include "System/TypeSystem/TypeSerialization.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;
using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        TypeSerializer::TypeSerializer( TypeSystem::TypeRegistry const& typeRegistry )
            : m_typeRegistry( typeRegistry )
        {}

        void TypeSerializer::Reset()
        {
            m_scratchBuffer.clear();
            m_stringBuffer.Clear();
            m_numTypesSerialized = 0;
        }

        bool TypeSerializer::WriteToFile( FileSystemPath const& outPath )
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

        void TypeSerializer::PrepareBuffer()
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

        void TypeSerializer::SerializeType( TypeSystem::TypeID typeID, void const* pTypeData )
        {
            if ( IsCoreType( typeID ) )
            {
                ConvertValueToString( typeID, pTypeData, m_scratchBuffer );
                m_writer.String( m_scratchBuffer.c_str() );
            }
            else // User types
            {
                auto const pTypeInfo = m_typeRegistry.GetTypeInfo( typeID );
                KRG_ASSERT( pTypeInfo != nullptr );

                m_writer.StartObject();

                // Every type has to have a type ID
                m_writer.Key( TypeSerializationSettings::TypeKey );
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
                            SerializeType( propInfo.m_typeID, pElementAddress );
                            pElementAddress += elementByteSize;
                        }

                        m_writer.EndArray();
                    }
                    else if ( IsCoreType( propInfo.m_typeID ) )
                    {
                        ConvertValueToString( propInfo.m_typeID, pPropertyDataAddress, m_scratchBuffer );
                        m_writer.String( m_scratchBuffer.c_str() );
                    }
                    else
                    {
                        SerializeType( propInfo.m_typeID, pPropertyDataAddress );
                    }
                }

                m_writer.EndObject();
            }
        }
    }
}

#endif