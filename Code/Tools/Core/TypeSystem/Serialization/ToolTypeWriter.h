#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class ToolTypeInstance;
    class TypeRegistry;

    //-------------------------------------------------------------------------
    // Native type serialization
    //-------------------------------------------------------------------------
    // Supports multiple types in a single archive
    // An archive is an unnamed array of serialized types
    // Each entry in that array is itself a JSON object with at least two members: the type ID of the serialized type and the serialized property data

    class KRG_TOOLS_CORE_API ToolTypeWriter
    {

    public:

        ToolTypeWriter( TypeRegistry const& typeRegistry );

        //-------------------------------------------------------------------------

        // Reset all serialized data without writing to disk
        void Reset();

        // Write currently serialized data to disk and reset serialized data
        bool WriteToFile( FileSystemPath const& outPath );

        //-------------------------------------------------------------------------

        void SerializeType( ToolTypeInstance const& type );

    private:

        TypeRegistry const&                                         m_typeRegistry;
        String                                                      m_scratchBuffer = String( 256, '0' );
        Serialization::RapidJsonStringBuffer                        m_stringBuffer;
        rapidjson::Writer<Serialization::RapidJsonStringBuffer>     m_writer = rapidjson::Writer<Serialization::RapidJsonStringBuffer>( m_stringBuffer );
        S32                                                         m_numTypesSerialized = 0;
    };
}