#pragma once
#include "CoreTypeIDs.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Resource/ResourceID.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Types/BitFlags.h"
#include "System/Core/Math/Math.h"
#include "System/Core/FileSystem/DataPath.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/Percentage.h"
#include "System/Core/Math/Vector.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------
// This is in a separate file to reduce the number of includes for the "CoreTypesIDs" header

namespace KRG::TypeSystem
{
    //-------------------------------------------------------------------------
    // Query
    //-------------------------------------------------------------------------

    template<typename T> inline bool IsCoreType() { return false; }
    template<template<typename> typename C> inline bool IsCoreType() { return false; }

    //-------------------------------------------------------------------------

    template<> inline bool IsCoreType<bool>() { return true; }
    template<> inline bool IsCoreType<int8>() { return true; }
    template<> inline bool IsCoreType<int16>() { return true; }
    template<> inline bool IsCoreType<int32>() { return true; }
    template<> inline bool IsCoreType<int64>() { return true; }
    template<> inline bool IsCoreType<uint8>() { return true; }
    template<> inline bool IsCoreType<uint16>() { return true; }
    template<> inline bool IsCoreType<uint32>() { return true; }
    template<> inline bool IsCoreType<uint64>() { return true; }
    template<> inline bool IsCoreType<float>() { return true; }
    template<> inline bool IsCoreType<double>() { return true; }
    template<> inline bool IsCoreType<UUID>() { return true; }
    template<> inline bool IsCoreType<StringID>() { return true; }
    template<> inline bool IsCoreType<String>() { return true; }
    template<> inline bool IsCoreType<Color>() { return true; }
    template<> inline bool IsCoreType<Float2>() { return true; }
    template<> inline bool IsCoreType<Float3>() { return true; }
    template<> inline bool IsCoreType<Float4>() { return true; }
    template<> inline bool IsCoreType<Vector>() { return true; }
    template<> inline bool IsCoreType<Quaternion>() { return true; }
    template<> inline bool IsCoreType<Matrix>() { return true; }
    template<> inline bool IsCoreType<Transform>() { return true; }
    template<> inline bool IsCoreType<Microseconds>() { return true; }
    template<> inline bool IsCoreType<Milliseconds>() { return true; }
    template<> inline bool IsCoreType<Seconds>() { return true; }
    template<> inline bool IsCoreType<Percentage>() { return true; }
    template<> inline bool IsCoreType<Degrees>() { return true; }
    template<> inline bool IsCoreType<Radians>() { return true; }
    template<> inline bool IsCoreType<EulerAngles >() { return true; }
    template<> inline bool IsCoreType<DataPath>() { return true; }
    template<> inline bool IsCoreType<BitFlags>() { return true; }
    template<> inline bool IsCoreType<TBitFlags>() { return true; }
    template<> inline bool IsCoreType<TVector>() { return true; }
    template<> inline bool IsCoreType<ResourceTypeID>() { return true; }
    template<> inline bool IsCoreType<ResourceID>() { return true; }
    template<> inline bool IsCoreType<Resource::ResourcePtr>() { return true; }
    template<> inline bool IsCoreType<TResourcePtr>() { return true; }

    //-------------------------------------------------------------------------

    template<typename T> inline TypeID GetCoreTypeID() { return TypeID(); }
    template<template<typename> typename C> inline TypeID GetCoreTypeID() { return TypeID(); }

    template<> inline TypeID GetCoreTypeID<bool>() { return GetCoreTypeID( CoreTypes::Bool ); }
    template<> inline TypeID GetCoreTypeID<int8>() { return GetCoreTypeID( CoreTypes::Int8 ); }
    template<> inline TypeID GetCoreTypeID<int16>() { return GetCoreTypeID( CoreTypes::Int16 ); }
    template<> inline TypeID GetCoreTypeID<int32>() { return GetCoreTypeID( CoreTypes::Int32 ); }
    template<> inline TypeID GetCoreTypeID<int64>() { return GetCoreTypeID( CoreTypes::Int64 ); }
    template<> inline TypeID GetCoreTypeID<uint8>() { return GetCoreTypeID( CoreTypes::Uint8 ); }
    template<> inline TypeID GetCoreTypeID<uint16>() { return GetCoreTypeID( CoreTypes::Uint16 ); }
    template<> inline TypeID GetCoreTypeID<uint32>() { return GetCoreTypeID( CoreTypes::Uint32 ); }
    template<> inline TypeID GetCoreTypeID<uint64>() { return GetCoreTypeID( CoreTypes::Uint64 ); }
    template<> inline TypeID GetCoreTypeID<float>() { return GetCoreTypeID( CoreTypes::Float ); }
    template<> inline TypeID GetCoreTypeID<double>() { return GetCoreTypeID( CoreTypes::Double ); }
    template<> inline TypeID GetCoreTypeID<UUID>() { return GetCoreTypeID( CoreTypes::UUID ); }
    template<> inline TypeID GetCoreTypeID<StringID>() { return GetCoreTypeID( CoreTypes::StringID ); }
    template<> inline TypeID GetCoreTypeID<String>() { return GetCoreTypeID( CoreTypes::String ); }
    template<> inline TypeID GetCoreTypeID<Color>() { return GetCoreTypeID( CoreTypes::Color ); }
    template<> inline TypeID GetCoreTypeID<Float2>() { return GetCoreTypeID( CoreTypes::Float2 ); }
    template<> inline TypeID GetCoreTypeID<Float3>() { return GetCoreTypeID( CoreTypes::Float3 ); }
    template<> inline TypeID GetCoreTypeID<Float4>() { return GetCoreTypeID( CoreTypes::Float4 ); }
    template<> inline TypeID GetCoreTypeID<Vector>() { return GetCoreTypeID( CoreTypes::Vector ); }
    template<> inline TypeID GetCoreTypeID<Quaternion>() { return GetCoreTypeID( CoreTypes::Quaternion ); }
    template<> inline TypeID GetCoreTypeID<Matrix>() { return GetCoreTypeID( CoreTypes::Matrix ); }
    template<> inline TypeID GetCoreTypeID<Transform>() { return GetCoreTypeID( CoreTypes::Transform ); }
    template<> inline TypeID GetCoreTypeID<Microseconds>() { return GetCoreTypeID( CoreTypes::Microseconds ); }
    template<> inline TypeID GetCoreTypeID<Milliseconds>() { return GetCoreTypeID( CoreTypes::Milliseconds ); }
    template<> inline TypeID GetCoreTypeID<Seconds>() { return GetCoreTypeID( CoreTypes::Seconds ); }
    template<> inline TypeID GetCoreTypeID<Percentage>() { return GetCoreTypeID( CoreTypes::Percentage ); }
    template<> inline TypeID GetCoreTypeID<Degrees>() { return GetCoreTypeID( CoreTypes::Degrees ); }
    template<> inline TypeID GetCoreTypeID<Radians>() { return GetCoreTypeID( CoreTypes::Radians ); }
    template<> inline TypeID GetCoreTypeID<EulerAngles >() { return GetCoreTypeID( CoreTypes::EulerAngles ); }
    template<> inline TypeID GetCoreTypeID<DataPath>() { return GetCoreTypeID( CoreTypes::DataPath ); }
    template<> inline TypeID GetCoreTypeID<BitFlags>() { return GetCoreTypeID( CoreTypes::BitFlags ); }
    template<> inline TypeID GetCoreTypeID<TBitFlags>() { return GetCoreTypeID( CoreTypes::TBitFlags ); }
    template<> inline TypeID GetCoreTypeID<TVector>() { return GetCoreTypeID( CoreTypes::TVector ); }
    template<> inline TypeID GetCoreTypeID<ResourceTypeID>() { return GetCoreTypeID( CoreTypes::ResourceTypeID ); }
    template<> inline TypeID GetCoreTypeID<ResourceID>() { return GetCoreTypeID( CoreTypes::ResourceID ); }
    template<> inline TypeID GetCoreTypeID<Resource::ResourcePtr>() { return GetCoreTypeID( CoreTypes::ResourcePtr ); }
    template<> inline TypeID GetCoreTypeID<TResourcePtr>() { return GetCoreTypeID( CoreTypes::TResourcePtr ); }

    //-------------------------------------------------------------------------
    // Validation for getters/setters
    //-------------------------------------------------------------------------

    template<typename T>
    inline bool ValidateTypeAgainstTypeID( TypeID typeID )
    {
        TypeID const nativeTypeID = GetCoreTypeID<T>();

        // Handle derivation of ResourcePtr to TResourcePtr
        if ( nativeTypeID == GetCoreTypeID( CoreTypes::ResourcePtr ) )
        {
            return typeID == GetCoreTypeID( CoreTypes::ResourcePtr ) || typeID == GetCoreTypeID( CoreTypes::TResourcePtr );
        }

        // Handle derivation of BitFlags to TBitFlags
        if ( nativeTypeID == GetCoreTypeID( CoreTypes::BitFlags ) )
        {
            return typeID == GetCoreTypeID( CoreTypes::BitFlags ) || typeID == GetCoreTypeID( CoreTypes::TBitFlags );
        }

        //-------------------------------------------------------------------------

        return GetCoreTypeID<T>() == typeID;
    }
}