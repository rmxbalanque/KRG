#pragma once
#include "TypeID.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    enum class CoreTypes
    {
        Bool = 0,
        Uint8,
        Int8,
        Uint16,
        Int16,
        Uint32,
        Int32,
        Uint64,
        Int64,
        Float,
        Double,
        UUID,
        StringID,
        String,
        Color,
        Float2,
        Float3,
        Float4,
        Vector,
        Quaternion,
        Matrix,
        Transform,
        Microseconds,
        Milliseconds,
        Seconds,
        Percentage,
        Degrees,
        Radians,
        EulerAngles,
        DataPath,
        TVector,

        ResourceTypeID,
        ResourceID,
        ResourcePtr,
        TResourcePtr,

        NumTypes,
    };

    // Core Type ID
    //-------------------------------------------------------------------------

    KRG_SYSTEM_TYPESYSTEM_API void Initialize();
    KRG_SYSTEM_TYPESYSTEM_API void Shutdown();

    KRG_SYSTEM_TYPESYSTEM_API TypeID GetCoreTypeID( CoreTypes coreType );
    KRG_SYSTEM_TYPESYSTEM_API size_t GetCoreTypeSize( CoreTypes coreType );
    KRG_SYSTEM_TYPESYSTEM_API size_t GetCoreTypeAlignment( CoreTypes coreType );

    KRG_SYSTEM_TYPESYSTEM_API CoreTypes GetCoreType( TypeID typeID );
    KRG_SYSTEM_TYPESYSTEM_API bool IsCoreType( TypeID typeID );
    KRG_SYSTEM_TYPESYSTEM_API size_t GetCoreTypeSize( TypeID typeID );
    KRG_SYSTEM_TYPESYSTEM_API size_t GetCoreTypeAlignment( TypeID typeID );
}

//-------------------------------------------------------------------------

inline bool operator==( KRG::TypeSystem::TypeID const& typeID, KRG::TypeSystem::CoreTypes coreType )
{
    return typeID == KRG::TypeSystem::GetCoreTypeID( coreType );
}

inline bool operator!=( KRG::TypeSystem::TypeID const& typeID, KRG::TypeSystem::CoreTypes coreType )
{
    return typeID != KRG::TypeSystem::GetCoreTypeID( coreType );
}