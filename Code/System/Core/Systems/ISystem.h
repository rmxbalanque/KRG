#pragma once

#include "../_Module/API.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Defines an interface for an engine system
    //-------------------------------------------------------------------------
    // 
    // Engine systems are global (often singleton) systems that are not tied to a game world
    // i.e. InputSystem, ResourceSystem, TypeRegistry...
    //
    // This interface exists primarily for type safety and ease of use
    // Each system needs a publicly accessible uint32 ID called 'SystemID'
    // Use the macro provided below to declare new systems
    // 
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ISystem
    {
    public:

        virtual uint32 GetSystemID() const = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_SYSTEM_ID( TypeName ) \
constexpr static uint32 const SystemID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetSystemID() const override final { return TypeName::SystemID; }