#include "ToolTypeWriter.h"
#include "TypeSerializationCommon.h"
#include "Tools/Core/TypeSystem/ToolTypeInstance.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    ToolTypeWriter::ToolTypeWriter( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    void ToolTypeWriter::Reset()
    {
        m_scratchBuffer.clear();
        m_stringBuffer.Clear();
        m_numTypesSerialized = 0;
    }

    bool ToolTypeWriter::WriteToFile( FileSystem::Path const& outPath )
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

    void ToolTypeWriter::SerializeType( ToolTypeInstance const& type )
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

        // Write Type to JSON
        //-------------------------------------------------------------------------

        ToolTypeInstanceDescriptor const typeDescriptor = type.GetDescriptor();

        m_writer.StartObject();

        // Every type has to have a type ID
        m_writer.Key( Serialization::TypeSerialization::Key_TypeID );
        m_writer.String( type.GetTypeID().GetAsStringID().ToString() );

        // Write all property values
        for ( auto const& propertyValue : typeDescriptor.m_properties )
        {
            // Write Key
            m_writer.Key( propertyValue.m_path.ToString().c_str() );

            // Write Value
            m_writer.Key( propertyValue.m_value.c_str() );
        }

        m_writer.EndObject();

        //-------------------------------------------------------------------------

        m_numTypesSerialized++;
    }
}