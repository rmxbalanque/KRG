#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeDescriptors.h"

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

    KRG_TOOLS_CORE_API bool ReadTypeDescriptor( TypeRegistry const& typeRegistry, rapidjson::Value const& typeObjectValue, TypeDescriptor& outDesc );
    KRG_TOOLS_CORE_API void WriteTypeDescriptor( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeDescriptor const& type );

    KRG_TOOLS_CORE_API void CreateTypeDescriptorFromNativeType( TypeRegistry const& typeRegistry, TypeID const& typeID, void const* pTypeInstance, TypeDescriptor& outDesc );

    template<typename T>
    void CreateTypeDescriptorFromNativeType( TypeRegistry const& typeRegistry, T* pTypeInstance, TypeDescriptor& outDesc )
    {
        CreateTypeDescriptorFromNativeType( typeRegistry, T::GetStaticTypeID(), pTypeInstance, outDesc );
    }

    // Native Type
    //-------------------------------------------------------------------------

    KRG_TOOLS_CORE_API bool ReadNativeType( TypeRegistry const& typeRegistry, rapidjson::Value const& typeObjectValue, TypeID const& typeID, void* pTypeInstance );
    KRG_TOOLS_CORE_API void WriteNativeType( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeID const& typeID, void const* pTypeInstance );

    template<typename T>
    bool ReadNativeType( TypeRegistry const& typeRegistry, rapidjson::Value const& typeObjectValue, T* pOutType ) 
    {
        return ReadNativeType( typeRegistry, typeObjectValue, T::GetStaticTypeID(), pOutType ); 
    }
}