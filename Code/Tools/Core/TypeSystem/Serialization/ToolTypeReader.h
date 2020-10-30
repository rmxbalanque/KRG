#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class TypeRegistry;
    class ToolTypeInstance;

    //-------------------------------------------------------------------------
    // Native type serialization
    //-------------------------------------------------------------------------
    // Supports multiple types in a single archive
    // An archive is an unnamed array of serialized types
    // Each entry in that array is itself a JSON object with at least two members: the type ID of the serialized type and the serialized property data

    class KRG_TOOLS_CORE_API ToolTypeReader
    {

    public:

        ToolTypeReader( TypeSystem::TypeRegistry const& typeRegistry );
        ~ToolTypeReader();

        bool ReadFromFile( FileSystemPath const& filePath );

        void DeserializeType( ToolTypeInstance& outType ) const;

    private:

        void Reset();
        rapidjson::Value const& GetValueToBeDeserialized() const;
        void DeserializeType( rapidjson::Value const& currentJsonValue, TypeSystem::TypeID typeID, void* pTypeData ) const;

    private:

        TypeRegistry const&                                         m_typeRegistry;
        Byte*                                                       m_pFileBuffer = nullptr;
        rapidjson::Document                                         m_document;
        S32                                                         m_numSerializedTypes = 0;
        mutable S32                                                 m_deserializedTypeIdx = 0;
    };
}