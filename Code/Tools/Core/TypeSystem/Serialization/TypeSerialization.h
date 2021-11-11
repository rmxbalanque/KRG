#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/ThirdParty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeDescriptors.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem 
{
    class TypeRegistry;
    class TypeInstanceModel;
}

//-------------------------------------------------------------------------
// Type Serialization
//-------------------------------------------------------------------------
// This file contain basic facilities to convert between JSON and the various type representations we have

namespace KRG::TypeSystem::Serialization
{
    struct Constants
    {
        constexpr static char const* const s_typeID = "TypeID";
    };

    // Descriptors
    //-------------------------------------------------------------------------

    KRG_TOOLS_CORE_API bool ReadTypeDescriptorFromJSON( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, TypeDescriptor& outDesc );
    KRG_TOOLS_CORE_API void WriteTypeDescriptorToJSON( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeDescriptor const& type );

    // Type Instances
    //-------------------------------------------------------------------------

    // Read the data for a native type from JSON - expect a fully created type to be supplied and will override the values
    KRG_TOOLS_CORE_API bool ReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, IRegisteredType* pTypeInstance );

    // Read the data for a native type from JSON - expect a fully created type to be supplied and will override the values
    KRG_TOOLS_CORE_API bool ReadNativeTypeFromString( TypeRegistry const& typeRegistry, String const& jsonString, IRegisteredType* pTypeInstance );

    // Write the property data for a supplied native type to JSON
    KRG_TOOLS_CORE_API void WriteNativeType( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, RapidJsonWriter& writer );

    // Write the property data for a supplied native type to JSON
    KRG_TOOLS_CORE_API void WriteNativeTypeToString( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, String& outString );

    // Create a new instance of a type from a supplied JSON version
    KRG_TOOLS_CORE_API IRegisteredType* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue );

    // Create a new instance of a type from a supplied JSON version
    template<typename T>
    T* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue )
    {
        auto pCreatedType = CreateAndReadNativeType( typeRegistry, typeObjectValue );
        return Cast<T>( pCreatedType );
    }
}