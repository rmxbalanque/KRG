#include "CoreTypeIDs.h"

//-------------------------------------------------------------------------

#define REGISTER_TYPE_RECORD( coreTypeEnum, fullyQualifiedTypeName ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName ), alignof( fullyQualifiedTypeName ) };

#define REGISTER_TEMPLATE_TYPE_RECORD_GENERIC( coreTypeEnum, fullyQualifiedTypeName, baseSpecialization ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName<baseSpecialization> ), alignof( fullyQualifiedTypeName<baseSpecialization> ) };

#define REGISTER_TEMPLATE_TYPE_RECORD_RESOURCE( coreTypeEnum, fullyQualifiedTypeName ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName<KRG::Resource::IResource> ), alignof( fullyQualifiedTypeName<KRG::Resource::IResource> ) };

#define REGISTER_TEMPLATE_TYPE_RECORD_OBJECT( coreTypeEnum, fullyQualifiedTypeName ) \
ID = TypeID( #fullyQualifiedTypeName );\
KRG_ASSERT( !CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum].m_ID.IsValid() );\
CoreTypeRegistry::s_coreTypeRecords[(uint8)coreTypeEnum] = { ID, sizeof( fullyQualifiedTypeName<KRG::Entity> ), alignof( fullyQualifiedTypeName<KRG::Entity> ) };

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    TArray<CoreTypeRegistry::CoreTypeRecord, (uint8) CoreTypes::NumTypes> CoreTypeRegistry::s_coreTypeRecords;
    bool CoreTypeRegistry::s_areCoreTypeRecordsInitialized = false;

    //-------------------------------------------------------------------------

    void CoreTypeRegistry::Initialize()
    {
        KRG_ASSERT( !s_areCoreTypeRecordsInitialized );

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
        REGISTER_TYPE_RECORD( CoreTypes::TypeID, KRG::TypeSystem::TypeID );
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
        REGISTER_TYPE_RECORD( CoreTypes::IntRange, KRG::IntRange );
        REGISTER_TYPE_RECORD( CoreTypes::FloatRange, KRG::FloatRange );

        REGISTER_TYPE_RECORD( CoreTypes::BitFlags, KRG::BitFlags );
        REGISTER_TEMPLATE_TYPE_RECORD_GENERIC( CoreTypes::TBitFlags, KRG::TBitFlags, enum class TempEnum );

        REGISTER_TEMPLATE_TYPE_RECORD_GENERIC( CoreTypes::TVector, KRG::TVector, uint8 );

        // Resources
        REGISTER_TYPE_RECORD( CoreTypes::ResourcePath, KRG::ResourcePath );
        REGISTER_TYPE_RECORD( CoreTypes::ResourceID, KRG::ResourceID );
        REGISTER_TYPE_RECORD( CoreTypes::ResourceTypeID, KRG::ResourceTypeID );
        REGISTER_TYPE_RECORD( CoreTypes::ResourcePtr, KRG::Resource::ResourcePtr );
        REGISTER_TEMPLATE_TYPE_RECORD_RESOURCE( CoreTypes::TResourcePtr, KRG::TResourcePtr );

        s_areCoreTypeRecordsInitialized = true;
    }

    void CoreTypeRegistry::Shutdown()
    {
        KRG_ASSERT( s_areCoreTypeRecordsInitialized );
        s_areCoreTypeRecordsInitialized = false;
    }

    //-------------------------------------------------------------------------
 
    bool CoreTypeRegistry::IsCoreType( TypeID typeID )
    {
        KRG_ASSERT( s_areCoreTypeRecordsInitialized );
        return eastl::find( s_coreTypeRecords.begin(), s_coreTypeRecords.end(), typeID ) != s_coreTypeRecords.end();
    }

    CoreTypes CoreTypeRegistry::GetType( TypeID typeID )
    {
        KRG_ASSERT( s_areCoreTypeRecordsInitialized );

        for ( int32 i = 0; i < (int32) CoreTypes::NumTypes; i++ )
        {
            if ( CoreTypeRegistry::s_coreTypeRecords[i].m_ID == typeID )
            {
                return (CoreTypes) i;
            }
        }

        return CoreTypes::Invalid;
    }

    size_t CoreTypeRegistry::GetTypeSize( TypeID typeID )
    {
        KRG_ASSERT( s_areCoreTypeRecordsInitialized );

        auto recordIter = eastl::find( s_coreTypeRecords.begin(), s_coreTypeRecords.end(), typeID );
        KRG_ASSERT( recordIter != s_coreTypeRecords.end() );
        return recordIter->m_typeSize;
    }

    size_t CoreTypeRegistry::GetTypeAlignment( TypeID typeID )
    {
        KRG_ASSERT( s_areCoreTypeRecordsInitialized );

        auto recordIter = eastl::find( s_coreTypeRecords.begin(), s_coreTypeRecords.end(), typeID );
        KRG_ASSERT( recordIter != s_coreTypeRecords.end() );
        return recordIter->m_typeAlignment;
    }
}