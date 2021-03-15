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
    struct PropertyInfo;

    // Native type serialization
    //-------------------------------------------------------------------------
    // Supports multiple compound types in a single archive
    // // Do not try to serialize core-types using this writer
    // An archive is either a single serialized type or an array of serialized types
    // Each type is serialized as a JSON object with a 'TypeID' property containing the type ID of the serialized type

    class KRG_TOOLS_CORE_API NativeTypeWriter
    {

    public:

        NativeTypeWriter( TypeRegistry const& typeRegistry );

        //-------------------------------------------------------------------------

        // Reset all serialized data without writing to disk
        void Reset();

        // Write currently serialized data to disk and reset serialized data
        bool WriteToFile( FileSystemPath const& outPath );

        //-------------------------------------------------------------------------

        template<typename T>
        inline NativeTypeWriter& operator<<( T const* pType )
        {
            PrepareBuffer();
            SerializeType( T::GetStaticTypeID(), pType );
            m_numTypesSerialized++;
            return *this;
        }

        template<typename T>
        inline NativeTypeWriter& operator<<( T* pType )
        {
            PrepareBuffer();
            SerializeType( T::GetStaticTypeID(), pType );
            m_numTypesSerialized++;
            return *this;
        }

        template<typename T>
        inline NativeTypeWriter& operator<<( T const& pType )
        {
            PrepareBuffer();
            SerializeType( T::GetStaticTypeID(), &pType );
            m_numTypesSerialized++;
            return *this;
        }

        template<typename T>
        inline NativeTypeWriter& operator<<( T& pType )
        {
            PrepareBuffer();
            SerializeType( T::GetStaticTypeID(), &pType );
            m_numTypesSerialized++;
            return *this;
        }

    private:

        void PrepareBuffer();
        void SerializeType( TypeID typeID, void const* pTypeData );
        void SerializeProperty( PropertyInfo const& propertyInfo, void const* pTypeData );

    private:

        TypeRegistry const&                                         m_typeRegistry;
        String                                                      m_scratchBuffer = String( 256, '0' );
        Serialization::RapidJsonStringBuffer                        m_stringBuffer;
        rapidjson::Writer<Serialization::RapidJsonStringBuffer>     m_writer = rapidjson::Writer<Serialization::RapidJsonStringBuffer>( m_stringBuffer );
        int32                                                       m_numTypesSerialized = 0;
    };
}

#endif