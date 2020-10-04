#pragma once
#include "TypeID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        enum class CoreTypes
        {
            Bool = 0,
            U8,
            S8,
            U16,
            S16,
            U32,
            S32,
            U64,
            S64,
            F32,
            F64,
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