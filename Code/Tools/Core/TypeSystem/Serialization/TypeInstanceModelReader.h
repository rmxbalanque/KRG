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
    class TypeInstanceModel;
    struct TypeInfo;
    struct PropertyInstanceDescriptor;

    //-------------------------------------------------------------------------
    // Native type serialization
    //-------------------------------------------------------------------------
    // Supports multiple types in a single archive
    // An archive is an unnamed array of serialized types
    // Each entry in that array is itself a JSON object with at least two members: the type ID of the serialized type and the serialized property data

    class KRG_TOOLS_CORE_API TypeInstanceModelReader
    {

    public:

        TypeInstanceModelReader( TypeRegistry const& typeRegistry );
        ~TypeInstanceModelReader();

        bool ReadFromFile( FileSystem::Path const& filePath );

        bool DeserializeType( TypeInstanceModel& outType ) const;

    private:

        void Reset();

        bool DeserializeType( rapidjson::Value const& typeObject, TVector<PropertyInstanceDescriptor>& outPropertyValues, String const& propertyPathPrefix ) const;
        bool DeserializePropertyArray( rapidjson::Value const& arrayValue, TVector<PropertyInstanceDescriptor>& outPropertyValues, String const& propertyPathPrefix ) const;

    private:

        TypeRegistry const&                                         m_typeRegistry;
        Byte*                                                       m_pFileBuffer = nullptr;
        rapidjson::Document                                         m_document;
        int32                                                       m_numSerializedTypes = 0;
        mutable int32                                               m_deserializedTypeIdx = 0;
    };
}