#if _WIN32
#pragma once

#include "../_Module/API.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        class TypeRegistry;
    }

    //-------------------------------------------------------------------------

    namespace Serialization
    {
        // Native type serialization
        //-------------------------------------------------------------------------
        // Supports multiple types in a single archive
        // An archive is either a single serialized type or an array of serialized types
        // Each type is serialized as a JSON object with a 'TypeID' property containing the type ID of the serialized type

        class KRG_TOOLS_CORE_API TypeSerializer
        {

        public:

            TypeSerializer( TypeSystem::TypeRegistry const& typeRegistry );

            //-------------------------------------------------------------------------

            // Reset all serialized data without writing to disk
            void Reset();

            // Write currently serialized data to disk and reset serialized data
            bool WriteToFile( FileSystemPath const& outPath );

            //-------------------------------------------------------------------------

            template<typename T>
            inline TypeSerializer& operator<<( T const* pType )
            {
                PrepareBuffer();
                SerializeType( T::GetStaticTypeID(), pType );
                m_numTypesSerialized++;
                return *this;
            }

            template<typename T>
            inline TypeSerializer& operator<<( T* pType )
            {
                PrepareBuffer();
                SerializeType( T::GetStaticTypeID(), pType );
                m_numTypesSerialized++;
                return *this;
            }

            template<typename T>
            inline TypeSerializer& operator<<( T const& pType )
            {
                PrepareBuffer();
                SerializeType( T::GetStaticTypeID(), &pType );
                m_numTypesSerialized++;
                return *this;
            }

            template<typename T>
            inline TypeSerializer& operator<<( T& pType )
            {
                PrepareBuffer();
                SerializeType( T::GetStaticTypeID(), &pType );
                m_numTypesSerialized++;
                return *this;
            }

        private:

            void PrepareBuffer();
            void SerializeType( TypeSystem::TypeID typeID, void const* pTypeData );

        private:

            TypeSystem::TypeRegistry const&                             m_typeRegistry;
            String                                                      m_scratchBuffer = String( 256, '0' );
            RapidJsonStringBuffer                                       m_stringBuffer;
            rapidjson::Writer<RapidJsonStringBuffer>                    m_writer = rapidjson::Writer<RapidJsonStringBuffer>( m_stringBuffer );
            S32                                                         m_numTypesSerialized = 0;
        };
    }
}

#endif