#include "TypeWriter.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

using namespace rapidjson;

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Serialization
{
    TypeWriter::TypeWriter( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    void TypeWriter::Reset()
    {
        JsonWriter::Reset();
        m_numTypesSerialized = 0;
    }

    void TypeWriter::PreSerializeType()
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

    void TypeWriter::FinalizeSerializedData()
    {
        if ( m_numTypesSerialized > 1 )
        {
            m_writer.EndArray();
        }
    }
}