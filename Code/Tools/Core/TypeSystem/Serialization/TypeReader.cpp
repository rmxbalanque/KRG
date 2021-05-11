#include "TypeReader.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Serialization
{
    TypeReader::TypeReader( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    void TypeReader::OnFileReadSuccess()
    {
        if ( m_document.IsArray() )
        {
            m_numSerializedTypes = m_document.Size();
        }
        else
        {
            m_numSerializedTypes = m_document.IsObject() ? 1 : 0;
        }
    }

    void TypeReader::Reset()
    {
        JsonFileReader::Reset();
        m_numSerializedTypes = 0;
        m_deserializedTypeIdx = 0;
    }

    rapidjson::Value const& TypeReader::GetObjectValueToBeDeserialized()
    {
        KRG_ASSERT( m_deserializedTypeIdx < m_numSerializedTypes );

        if ( m_document.IsArray() )
        {
            KRG_ASSERT( m_document.IsArray() );
            return m_document[m_deserializedTypeIdx++];
        }
        else
        {
            return m_document;
        }
    }
}