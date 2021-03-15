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

        BitFlags,
        TBitFlags,

        TVector,

        ResourceTypeID,
        ResourceID,
        ResourcePtr,
        TResourcePtr,

        NumTypes,
    };

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

        static TArray<CoreTypeRecord, (uint8) CoreTypes::NumTypes>  s_coreTypeRecords;
        static bool                                                 s_areCoreTypeRecordsInitialized;

    public:

        static void Initialize();
        static void Shutdown();

        static bool IsCoreType( TypeID typeID );
        static CoreTypes GetType( TypeID typeID );
        static size_t GetTypeSize( TypeID typeID );
        static size_t GetTypeAlignment( TypeID typeID );

        KRG_FORCE_INLINE static TypeID GetTypeID( CoreTypes coreType )
        {
            return s_coreTypeRecords[(uint8) coreType].m_ID;
        }

        KRG_FORCE_INLINE static size_t GetTypeSize( CoreTypes coreType )
        {
            return s_coreTypeRecords[(uint8) coreType].m_typeSize;
        }

        KRG_FORCE_INLINE static size_t GetTypeAlignment( CoreTypes coreType )
        {
            return s_coreTypeRecords[(uint8) coreType].m_typeAlignment;
        }
    };

    // Syntactic Sugar
    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE TypeID GetCoreTypeID( CoreTypes coreType ) { return CoreTypeRegistry::GetTypeID( coreType ); }
    KRG_FORCE_INLINE CoreTypes GetCoreType( TypeID typeID ) { return CoreTypeRegistry::GetType( typeID ); }
    KRG_FORCE_INLINE bool IsCoreType( TypeID typeID ){ return CoreTypeRegistry::IsCoreType( typeID ); }
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