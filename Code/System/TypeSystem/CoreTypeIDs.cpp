#include "CoreTypeIDs.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Percentage.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

#define REGISTER_TYPE_RECORD( coreTypeEnum, fullyQualifiedTypeName ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName ), alignof( fullyQualifiedTypeName ) };

#define REGISTER_TEMPLATE_TYPE_RECORD_GENERIC( coreTypeEnum, fullyQualifiedTypeName ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName<uint8> ), alignof( fullyQualifiedTypeName<uint8> ) };

#define REGISTER_TEMPLATE_TYPE_RECORD_RESOURCE( coreTypeEnum, fullyQualifiedTypeName ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName<KRG::Resource::IResource> ), alignof( fullyQualifiedTypeName<KRG::Resource::IResource> ) };

#define REGISTER_TEMPLATE_TYPE_RECORD_OBJECT( coreTypeEnum, fullyQualifiedTypeName ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName<KRG::Entity> ), alignof( fullyQualifiedTypeName<KRG::Entity> ) };

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    struct CoreTypeRecord
    {
        inline bool operator==( TypeID ID ) const { return m_ID == ID; }

        TypeID          m_ID;
        size_t          m_typeSize;
        size_t          m_typeAlignment;
    };

    static TInlineVector<CoreTypeRecord, (uint8) CoreTypes::NumTypes> CoreTypeRecords;
    static bool g_areCoreTypeRecordsInitialized = false;

    //-------------------------------------------------------------------------

    void Initialize()
    {
        KRG_ASSERT( !g_areCoreTypeRecordsInitialized );

        CoreTypeRecords.resize( (uint8) CoreTypes::NumTypes );

        TypeID ID;

        REGISTER_TYPE_RECORD( CoreTypes::Bool, bool );
        REGISTER_TYPE_RECORD( CoreTypes::Uint8, KRG::uint8 );
        REGISTER_TYPE_RECORD( CoreTypes::Int8, KRG::int8 );
        REGISTER_TYPE_RECORD( CoreTypes::Uint16, KRG::uint16 );
        REGISTER_TYPE_RECORD( CoreTypes::Int16, KRG::int16 );
        REGISTER_TYPE_RECORD( CoreTypes::Uint32, KRG::uint32 );
        REGISTER_TYPE_RECORD( CoreTypes::Int32, KRG::int32 );
        REGISTER_TYPE_RECORD( CoreTypes::Uint64, KRG::uint64 );
        REGISTER_TYPE_RECORD( CoreTypes::Int64, KRG::int64 );
        REGISTER_TYPE_RECORD( CoreTypes::Float, float );
        REGISTER_TYPE_RECORD( CoreTypes::Double, double );
        REGISTER_TYPE_RECORD( CoreTypes::UUID, KRG::UUID );
        REGISTER_TYPE_RECORD( CoreTypes::StringID, KRG::StringID );
        REGISTER_TYPE_RECORD( CoreTypes::String, KRG::String );
        REGISTER_TYPE_RECORD( CoreTypes::Color, KRG::Color );
        REGISTER_TYPE_RECORD( CoreTypes::Float2, KRG::Float2 );
        REGISTER_TYPE_RECORD( CoreTypes::Float3, KRG::Float3 );
        REGISTER_TYPE_RECORD( CoreTypes::Float4, KRG::Float4 );
        REGISTER_TYPE_RECORD( CoreTypes::Vector, KRG::Vector );
        REGISTER_TYPE_RECORD( CoreTypes::Quaternion, KRG::Quaternion );
        REGISTER_TYPE_RECORD( CoreTypes::Matrix, KRG::Matrix );
        REGISTER_TYPE_RECORD( CoreTypes::Transform, KRG::Transform );
        REGISTER_TYPE_RECORD( CoreTypes::Microseconds, KRG::Microseconds );
        REGISTER_TYPE_RECORD( CoreTypes::Milliseconds, KRG::Milliseconds );
        REGISTER_TYPE_RECORD( CoreTypes::Seconds, KRG::Seconds );
        REGISTER_TYPE_RECORD( CoreTypes::Percentage, KRG::Percentage );
        REGISTER_TYPE_RECORD( CoreTypes::Degrees, KRG::Degrees );
        REGISTER_TYPE_RECORD( CoreTypes::Radians, KRG::Radians );
        REGISTER_TYPE_RECORD( CoreTypes::EulerAngles, KRG::EulerAngles );
        REGISTER_TYPE_RECORD( CoreTypes::DataPath, KRG::DataPath );

        REGISTER_TEMPLATE_TYPE_RECORD_GENERIC( CoreTypes::TVector, KRG::TVector );

        REGISTER_TYPE_RECORD( CoreTypes::ResourceID, KRG::ResourceID );
        REGISTER_TYPE_RECORD( CoreTypes::ResourceTypeID, KRG::ResourceTypeID );
        REGISTER_TYPE_RECORD( CoreTypes::ResourcePtr, KRG::Resource::ResourcePtr );

        REGISTER_TEMPLATE_TYPE_RECORD_RESOURCE( CoreTypes::TResourcePtr, KRG::TResourcePtr );

        g_areCoreTypeRecordsInitialized = true;
    }

    void Shutdown()
    {
        KRG_ASSERT( g_areCoreTypeRecordsInitialized );
        CoreTypeRecords.clear();
        g_areCoreTypeRecordsInitialized = false;
    }

    //-------------------------------------------------------------------------

    TypeID GetCoreTypeID( CoreTypes coreType )
    {
        return CoreTypeRecords[(uint8) coreType].m_ID;
    }

    size_t GetCoreTypeSize( CoreTypes coreType )
    {
        return CoreTypeRecords[(uint8) coreType].m_typeSize;
    }

    size_t GetCoreTypeAlignment( CoreTypes coreType )
    {
        return CoreTypeRecords[(uint8) coreType].m_typeAlignment;
    }

    CoreTypes GetCoreType( TypeID typeID )
    {
        KRG_ASSERT( g_areCoreTypeRecordsInitialized );

        for ( int32 i = 0; i < (int32) CoreTypes::NumTypes; i++ )
        {
            if ( CoreTypeRecords[i].m_ID == typeID )
            {
                return (CoreTypes) i;
            }
        }

        KRG_UNREACHABLE_CODE();
        return CoreTypes::NumTypes;
    }

    bool IsCoreType( TypeID typeID )
    {
        KRG_ASSERT( g_areCoreTypeRecordsInitialized );
        return eastl::find( CoreTypeRecords.begin(), CoreTypeRecords.end(), typeID ) != CoreTypeRecords.end();
    }

    size_t GetCoreTypeSize( TypeID typeID )
    {
        KRG_ASSERT( g_areCoreTypeRecordsInitialized );

        auto RecordIter = eastl::find( CoreTypeRecords.begin(), CoreTypeRecords.end(), typeID );
        KRG_ASSERT( RecordIter != CoreTypeRecords.end() );
        return RecordIter->m_typeSize;
    }

    size_t GetCoreTypeAlignment( TypeID typeID )
    {
        KRG_ASSERT( g_areCoreTypeRecordsInitialized );

        auto RecordIter = eastl::find( CoreTypeRecords.begin(), CoreTypeRecords.end(), typeID );
        KRG_ASSERT( RecordIter != CoreTypeRecords.end() );
        return RecordIter->m_typeAlignment;
    }
}