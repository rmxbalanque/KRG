#if _WIN32
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
   
    // Native type serialization
    //-------------------------------------------------------------------------
    // Supports multiple types in a single archive
    // An archive is either a single serialized type or an array of serialized types
    // Each type is serialized as a JSON object with a 'TypeID' property containing the type ID of the serialized type

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API NativeTypeReader
    {
    public:

        NativeTypeReader( TypeRegistry const& typeRegistry );
        ~NativeTypeReader();

        bool ReadFromFile( FileSystemPath const& filePath );

        inline int32 GetNumSerializedTypes() const { return m_numSerializedTypes; }

        //-------------------------------------------------------------------------

        template<typename T>
        inline NativeTypeReader const& operator>>( T* pType ) const
        {
            DeserializeType( GetValueToBeDeserialized(), T::GetStaticTypeID(), pType );
            return *this;
        }

        template<typename T>
        inline NativeTypeReader const& operator>>( T& pType ) const
        {
            DeserializeType( GetValueToBeDeserialized(), T::GetStaticTypeID(), &pType );
            return *this;
        }

    private:

        void Reset();
        rapidjson::Value const& GetValueToBeDeserialized() const;
        void DeserializeType( rapidjson::Value const& currentJsonValue, TypeID typeID, void* pTypeData ) const;

    private:

        TypeRegistry const&                                         m_typeRegistry;
        Byte*                                                       m_pFileBuffer = nullptr;
        rapidjson::Document                                         m_document;
        int32                                                       m_numSerializedTypes = 0;
        mutable int32                                               m_deserializedTypeIdx = 0;
    };
}

#endif