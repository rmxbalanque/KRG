#pragma once

#include "../_Module/API.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }
    class DynamicTypeInstance;

    //-------------------------------------------------------------------------

    namespace Serialization
    {
        // Native type serialization
        //-------------------------------------------------------------------------
        // Supports multiple types in a single archive
        // An archive is an unnamed array of serialized types
        // Each entry in that array is itself a JSON object with at least two members: the type ID of the serialized type and the serialized property data

        class KRG_TOOLS_CORE_API DynamicTypeDeserializer
        {

        public:

            DynamicTypeDeserializer( TypeSystem::TypeRegistry const& typeRegistry );
            ~DynamicTypeDeserializer();

            bool ReadFromFile( FileSystemPath const& filePath );

            void DeserializeType( DynamicTypeInstance& outType ) const;

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