#include "DynamicTypeSerializer.h"
#include "DynamicTypeInstance.h"
#include "DynamicTypeSerialization.h"
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
        DynamicTypeSerializer::DynamicTypeSerializer( TypeSystem::TypeRegistry const& typeRegistry )
            : m_typeRegistry( typeRegistry )
        {}

        void DynamicTypeSerializer::Reset()
        {
            m_scratchBuffer.clear();
            m_stringBuffer.Clear();
            m_numTypesSerialized = 0;
        }

        bool DynamicTypeSerializer::WriteToFile( FileSystemPath const& outPath )
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

        void DynamicTypeSerializer::SerializeType( DynamicTypeInstance const& type )
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

            WriteSerializedDynamicTypeToJSON( m_typeRegistry, m_writer, type );
            m_numTypesSerialized++;
        }
    }
}