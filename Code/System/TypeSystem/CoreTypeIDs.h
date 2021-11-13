#pragma once
#include "TypeID.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Percentage.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/BitFlags.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Math/NumericRange.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    enum class CoreTypeID
    {
        Invalid = -1,

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
        TypeID,
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
        IntRange,
        FloatRange,

        BitFlags,
        TBitFlags,

        TVector,

        ResourcePath,
        ResourceTypeID,
        ResourceID,
        ResourcePtr,
        TResourcePtr,

        NumTypes,
    };

    //-------------------------------------------------------------------------
    // Core Type Registry
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_TYPESYSTEM_API CoreTypeRegistry
    {
        struct CoreTypeRecord
        {
            inline bool operator==( TypeID ID ) const { return m_ID == ID; }

            TypeID          m_ID;
            size_t          m_typeSize;
            size_t          m_typeAlignment;
        };

    private:

        static TArray<CoreTypeRecord, (uint8) CoreTypeID::NumTypes>  s_coreTypeRecords;
        static bool                                                 s_areCoreTypeRecordsInitialized;

    public:

        static void Initialize();
        static void Shutdown();

        static bool IsCoreType( TypeID typeID );
        static CoreTypeID GetType( TypeID typeID );
        static size_t GetTypeSize( TypeID typeID );
        static size_t GetTypeAlignment( TypeID typeID );

        KRG_FORCE_INLINE static TypeID GetTypeID( CoreTypeID coreType )
        {
            return s_coreTypeRecords[(uint8) coreType].m_ID;
        }

        KRG_FORCE_INLINE static size_t GetTypeSize( CoreTypeID coreType )
        {
            return s_coreTypeRecords[(uint8) coreType].m_typeSize;
        }

        KRG_FORCE_INLINE static size_t GetTypeAlignment( CoreTypeID coreType )
        {
            return s_coreTypeRecords[(uint8) coreType].m_typeAlignment;
        }
    };

    //-------------------------------------------------------------------------
    // Query
    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE TypeID GetCoreTypeID( CoreTypeID coreType ) { return CoreTypeRegistry::GetTypeID( coreType ); }
    KRG_FORCE_INLINE CoreTypeID GetCoreType( TypeID typeID ) { return CoreTypeRegistry::GetType( typeID ); }
    KRG_FORCE_INLINE bool IsCoreType( TypeID typeID ){ return CoreTypeRegistry::IsCoreType( typeID ); }

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
    template<> inline bool IsCoreType<TypeID>() { return true; }
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
    template<> inline bool IsCoreType<IntRange>() { return true; }
    template<> inline bool IsCoreType<FloatRange>() { return true; }
    template<> inline bool IsCoreType<BitFlags>() { return true; }
    template<> inline bool IsCoreType<TBitFlags>() { return true; }
    template<> inline bool IsCoreType<TVector>() { return true; }
    template<> inline bool IsCoreType<ResourcePath>() { return true; }
    template<> inline bool IsCoreType<ResourceTypeID>() { return true; }
    template<> inline bool IsCoreType<ResourceID>() { return true; }
    template<> inline bool IsCoreType<Resource::ResourcePtr>() { return true; }
    template<> inline bool IsCoreType<TResourcePtr>() { return true; }

    //-------------------------------------------------------------------------

    template<typename T> inline TypeID GetCoreTypeID() { return TypeID(); }
    template<template<typename> typename C> inline TypeID GetCoreTypeID() { return TypeID(); }

    template<> inline TypeID GetCoreTypeID<bool>() { return GetCoreTypeID( CoreTypeID::Bool ); }
    template<> inline TypeID GetCoreTypeID<int8>() { return GetCoreTypeID( CoreTypeID::Int8 ); }
    template<> inline TypeID GetCoreTypeID<int16>() { return GetCoreTypeID( CoreTypeID::Int16 ); }
    template<> inline TypeID GetCoreTypeID<int32>() { return GetCoreTypeID( CoreTypeID::Int32 ); }
    template<> inline TypeID GetCoreTypeID<int64>() { return GetCoreTypeID( CoreTypeID::Int64 ); }
    template<> inline TypeID GetCoreTypeID<uint8>() { return GetCoreTypeID( CoreTypeID::Uint8 ); }
    template<> inline TypeID GetCoreTypeID<uint16>() { return GetCoreTypeID( CoreTypeID::Uint16 ); }
    template<> inline TypeID GetCoreTypeID<uint32>() { return GetCoreTypeID( CoreTypeID::Uint32 ); }
    template<> inline TypeID GetCoreTypeID<uint64>() { return GetCoreTypeID( CoreTypeID::Uint64 ); }
    template<> inline TypeID GetCoreTypeID<float>() { return GetCoreTypeID( CoreTypeID::Float ); }
    template<> inline TypeID GetCoreTypeID<double>() { return GetCoreTypeID( CoreTypeID::Double ); }
    template<> inline TypeID GetCoreTypeID<UUID>() { return GetCoreTypeID( CoreTypeID::UUID ); }
    template<> inline TypeID GetCoreTypeID<StringID>() { return GetCoreTypeID( CoreTypeID::StringID ); }
    template<> inline TypeID GetCoreTypeID<TypeID>() { return GetCoreTypeID( CoreTypeID::TypeID ); }
    template<> inline TypeID GetCoreTypeID<String>() { return GetCoreTypeID( CoreTypeID::String ); }
    template<> inline TypeID GetCoreTypeID<Color>() { return GetCoreTypeID( CoreTypeID::Color ); }
    template<> inline TypeID GetCoreTypeID<Float2>() { return GetCoreTypeID( CoreTypeID::Float2 ); }
    template<> inline TypeID GetCoreTypeID<Float3>() { return GetCoreTypeID( CoreTypeID::Float3 ); }
    template<> inline TypeID GetCoreTypeID<Float4>() { return GetCoreTypeID( CoreTypeID::Float4 ); }
    template<> inline TypeID GetCoreTypeID<Vector>() { return GetCoreTypeID( CoreTypeID::Vector ); }
    template<> inline TypeID GetCoreTypeID<Quaternion>() { return GetCoreTypeID( CoreTypeID::Quaternion ); }
    template<> inline TypeID GetCoreTypeID<Matrix>() { return GetCoreTypeID( CoreTypeID::Matrix ); }
    template<> inline TypeID GetCoreTypeID<Transform>() { return GetCoreTypeID( CoreTypeID::Transform ); }
    template<> inline TypeID GetCoreTypeID<Microseconds>() { return GetCoreTypeID( CoreTypeID::Microseconds ); }
    template<> inline TypeID GetCoreTypeID<Milliseconds>() { return GetCoreTypeID( CoreTypeID::Milliseconds ); }
    template<> inline TypeID GetCoreTypeID<Seconds>() { return GetCoreTypeID( CoreTypeID::Seconds ); }
    template<> inline TypeID GetCoreTypeID<Percentage>() { return GetCoreTypeID( CoreTypeID::Percentage ); }
    template<> inline TypeID GetCoreTypeID<Degrees>() { return GetCoreTypeID( CoreTypeID::Degrees ); }
    template<> inline TypeID GetCoreTypeID<Radians>() { return GetCoreTypeID( CoreTypeID::Radians ); }
    template<> inline TypeID GetCoreTypeID<EulerAngles >() { return GetCoreTypeID( CoreTypeID::EulerAngles ); }
    template<> inline TypeID GetCoreTypeID<IntRange>() { return GetCoreTypeID( CoreTypeID::IntRange ); }
    template<> inline TypeID GetCoreTypeID<FloatRange>() { return GetCoreTypeID( CoreTypeID::FloatRange ); }
    template<> inline TypeID GetCoreTypeID<BitFlags>() { return GetCoreTypeID( CoreTypeID::BitFlags ); }
    template<> inline TypeID GetCoreTypeID<TBitFlags>() { return GetCoreTypeID( CoreTypeID::TBitFlags ); }
    template<> inline TypeID GetCoreTypeID<TVector>() { return GetCoreTypeID( CoreTypeID::TVector ); }
    template<> inline TypeID GetCoreTypeID<ResourcePath>() { return GetCoreTypeID( CoreTypeID::ResourcePath ); }
    template<> inline TypeID GetCoreTypeID<ResourceTypeID>() { return GetCoreTypeID( CoreTypeID::ResourceTypeID ); }
    template<> inline TypeID GetCoreTypeID<ResourceID>() { return GetCoreTypeID( CoreTypeID::ResourceID ); }
    template<> inline TypeID GetCoreTypeID<Resource::ResourcePtr>() { return GetCoreTypeID( CoreTypeID::ResourcePtr ); }
    template<> inline TypeID GetCoreTypeID<TResourcePtr>() { return GetCoreTypeID( CoreTypeID::TResourcePtr ); }

    //-------------------------------------------------------------------------
    // Validation for getters/setters
    //-------------------------------------------------------------------------

    template<typename T>
    inline bool ValidateTypeAgainstTypeID( TypeID typeID )
    {
        TypeID const nativeTypeID = GetCoreTypeID<T>();

        // Handle derivation of ResourcePtr to TResourcePtr
        if ( nativeTypeID == GetCoreTypeID( CoreTypeID::ResourcePtr ) )
        {
            return typeID == GetCoreTypeID( CoreTypeID::ResourcePtr ) || typeID == GetCoreTypeID( CoreTypeID::TResourcePtr );
        }

        // Handle derivation of BitFlags to TBitFlags
        if ( nativeTypeID == GetCoreTypeID( CoreTypeID::BitFlags ) )
        {
            return typeID == GetCoreTypeID( CoreTypeID::BitFlags ) || typeID == GetCoreTypeID( CoreTypeID::TBitFlags );
        }

        //-------------------------------------------------------------------------

        return GetCoreTypeID<T>() == typeID;
    }
}

//-------------------------------------------------------------------------
// Global conversion operators
//-------------------------------------------------------------------------

inline bool operator==( KRG::TypeSystem::TypeID const& typeID, KRG::TypeSystem::CoreTypeID coreType )
{
    return typeID == KRG::TypeSystem::GetCoreTypeID( coreType );
}

inline bool operator!=( KRG::TypeSystem::TypeID const& typeID, KRG::TypeSystem::CoreTypeID coreType )
{
    return typeID != KRG::TypeSystem::GetCoreTypeID( coreType );
}

inline bool operator==( KRG::TypeSystem::CoreTypeID coreType, KRG::TypeSystem::TypeID const& typeID )
{
    return typeID == KRG::TypeSystem::GetCoreTypeID( coreType );
}

inline bool operator!=( KRG::TypeSystem::CoreTypeID coreType, KRG::TypeSystem::TypeID const& typeID )
{
    return typeID != KRG::TypeSystem::GetCoreTypeID( coreType );
}