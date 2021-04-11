#include "TypeInstanceModelWriter.h"
#include "TypeSerializationCommon.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    TypeInstanceModelWriter::TypeInstanceModelWriter( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {
        m_writer.SetFormatOptions( kFormatDefault );
    }

    void TypeInstanceModelWriter::Reset()
    {
        m_scratchBuffer.clear();
        m_stringBuffer.Clear();
        m_numTypesSerialized = 0;
    }

    bool TypeInstanceModelWriter::WriteToFile( FileSystem::Path const& outPath )
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

    void TypeInstanceModelWriter::SerializeType( TypeInstanceModel const& type )
    {
        KRG_ASSERT( type.IsValid() );

        //-------------------------------------------------------------------------

        if ( m_numTypesSerialized == 1 )
        {
            String const firstValueSerialized = m_stringBuffer.GetString();

            //-------------------------------------------------------------------------

            m_stringBuffer.Clear();
            m_writer.StartArray();
            m_writer.RawValue( firstValueSerialized.c_str(), firstValueSerialized.length(), Type::kObjectType );
        }

        //-------------------------------------------------------------------------

        SerializeStructure( *type.GetAsPropertyInstance() );
        m_numTypesSerialized++;
    }

    void TypeInstanceModelWriter::SerializeStructure( PropertyInstanceModel const& propertyModel )
    {
        KRG_ASSERT( propertyModel.IsStructure() );

        m_writer.StartObject();

        // Every type has to have a type ID
        m_writer.Key( Serialization::TypeSerialization::Key_TypeID );
        m_writer.String( propertyModel.GetStructureTypeName() );

        // Write all property values
        for ( auto const& propertyValue : propertyModel.GetProperties() )
        {
            SerializeProperty( propertyValue );
        }

        m_writer.EndObject();
    }

    void TypeInstanceModelWriter::SerializeProperty( PropertyInstanceModel const& propertyModel )
    {
        if ( propertyModel.IsStructure() )
        {
            SerializeStructure( propertyModel );
        }
        else if ( propertyModel.IsArray() )
        {
            SerializeArray( propertyModel );
        }
        else
        {
            SerializeCoreType( propertyModel );
        }
    }

    void TypeInstanceModelWriter::SerializeArray( PropertyInstanceModel const& propertyModel )
    {
        m_writer.Key( propertyModel.GetName() );

        m_writer.StartArray();

        for ( auto const& childProperty : propertyModel.GetProperties() )
        {
            if ( childProperty.IsStructure() || childProperty.IsArray() )
            {
                SerializeProperty( childProperty );
            }
            else
            {
                m_writer.Key( childProperty.GetStringValue().c_str() );
            }
        }

        m_writer.EndArray();
    }

    void TypeInstanceModelWriter::SerializeCoreType( PropertyInstanceModel const& propertyModel )
    {
        m_writer.Key( propertyModel.GetName() );
        m_writer.Key( propertyModel.GetStringValue().c_str() );
    }
}