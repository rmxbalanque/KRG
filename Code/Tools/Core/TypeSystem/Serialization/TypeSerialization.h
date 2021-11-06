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

    // Type Descriptor
    //-------------------------------------------------------------------------

    KRG_TOOLS_CORE_API bool ReadTypeDescriptor( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, TypeDescriptor& outDesc );
    KRG_TOOLS_CORE_API void WriteTypeDescriptor( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeDescriptor const& type );
    KRG_TOOLS_CORE_API void CreateTypeDescriptorFromNativeType( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, TypeDescriptor& outDesc, bool setStringValues = false );

    // Native Type
    //-------------------------------------------------------------------------

    KRG_TOOLS_CORE_API bool ReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, IRegisteredType* pTypeInstance );
    KRG_TOOLS_CORE_API void WriteNativeType( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, IRegisteredType const* pTypeInstance );
    KRG_TOOLS_CORE_API IRegisteredType* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue );

    template<typename T>
    T* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue )
    {
        auto pCreatedType = CreateAndReadNativeType( typeRegistry, typeObjectValue );
        return Cast<T>( pCreatedType );
    }
}