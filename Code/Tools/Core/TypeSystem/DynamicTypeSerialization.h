#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/TypeSystem/PropertyPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }
    class DynamicTypeInstance;

    //-------------------------------------------------------------------------

    namespace Serialization
    {
        struct SerializedDynamicPropertyValue
        {
            SerializedDynamicPropertyValue( TypeSystem::PropertyPath const& path, String const& value, TypeSystem::TypeID typeID = TypeSystem::TypeID() )
                : m_path( path )
                , m_value( value )
                , m_typeID( typeID )
            {
                KRG_ASSERT( path.IsValid() && !value.empty() );
            }

            inline bool IsOnlyRelevantForTools() const { return !m_typeID.IsValid(); }

        public:

            TypeSystem::PropertyPath                                    m_path;
            String                                                      m_value;
            TypeSystem::TypeID                                          m_typeID; // Not serialized - but needed for certain conversions
        };

        struct SerializedDynamicType
        {
            TypeSystem::TypeID                                          m_typeID;
            TVector<SerializedDynamicPropertyValue>                     m_properties;
        };

        // Serialization format
        //-------------------------------------------------------------------------

        KRG_TOOLS_CORE_API bool SerializeDynamicType( TypeSystem::TypeRegistry const& typeRegistry, DynamicTypeInstance const& typeInstance, SerializedDynamicType& outType );
        KRG_TOOLS_CORE_API bool DeserializeDynamicType( TypeSystem::TypeRegistry const& typeRegistry, SerializedDynamicType const& serializedType, DynamicTypeInstance& outInstance );

        // JSON
        //-------------------------------------------------------------------------

        KRG_TOOLS_CORE_API void WriteSerializedDynamicTypeToJSON( TypeSystem::TypeRegistry const& typeRegistry, rapidjson::Writer<RapidJsonStringBuffer>& writer, DynamicTypeInstance const& type );
        KRG_TOOLS_CORE_API void ReadSerializedDynamicTypeFromJSON( TypeSystem::TypeRegistry const& typeRegistry, rapidjson::Value const& jsonObject, DynamicTypeInstance& outType );
    }
}