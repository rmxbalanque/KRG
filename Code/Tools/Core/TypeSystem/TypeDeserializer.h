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

        //-------------------------------------------------------------------------

        class KRG_TOOLS_CORE_API TypeDeserializer
        {
        public:

            TypeDeserializer( TypeSystem::TypeRegistry const& typeRegistry );
            ~TypeDeserializer();

            bool ReadFromFile( FileSystemPath const& filePath );

            //-------------------------------------------------------------------------

            template<typename T>
            inline TypeDeserializer const& operator>>( T* pType ) const
            {
                DeserializeType( GetValueToBeDeserialized(), T::GetStaticTypeID(), pType );
                m_deserializedTypeIdx++;
                return *this;
            }

            template<typename T>
            inline TypeDeserializer const& operator>>( T& pType ) const
            {
                DeserializeType( GetValueToBeDeserialized(), T::GetStaticTypeID(), &pType );
                m_deserializedTypeIdx++;
                return *this;
            }

        private:

            void Reset();
            rapidjson::Value const& GetValueToBeDeserialized() const;
            void DeserializeType( rapidjson::Value const& currentJsonValue, TypeSystem::TypeID typeID, void* pTypeData ) const;

        private:

            TypeSystem::TypeRegistry const&                             m_typeRegistry;
            Byte*                                                       m_pFileBuffer = nullptr;
            rapidjson::Document                                         m_document;
            S32                                                         m_numSerializedTypes = 0;
            mutable S32                                                 m_deserializedTypeIdx = 0;
        };
    }
}

#endif