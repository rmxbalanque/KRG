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
    TArray<CoreTypeRegistry::CoreTypeRecord, (uint8) CoreTypeID::NumTypes> CoreTypeRegistry::s_coreTypeRecords;
    bool CoreTypeRegistry::s_areCoreTypeRecordsInitialized = false;

    //-------------------------------------------------------------------------

    void CoreTypeRegistry::Initialize()
    {
        KRG_ASSERT( !s_areCoreTypeRecordsInitialized );

        TypeID ID;

        REGISTER_TYPE_RECORD( CoreTypeID::Bool, bool );
        REGISTER_TYPE_RECORD( CoreTypeID::Uint8, KRG::uint8 );
        REGISTER_TYPE_RECORD( CoreTypeID::Int8, KRG::int8 );
        REGISTER_TYPE_RECORD( CoreTypeID::Uint16, KRG::uint16 );
        REGISTER_TYPE_RECORD( CoreTypeID::Int16, KRG::int16 );
        REGISTER_TYPE_RECORD( CoreTypeID::Uint32, KRG::uint32 );
        REGISTER_TYPE_RECORD( CoreTypeID::Int32, KRG::int32 );
        REGISTER_TYPE_RECORD( CoreTypeID::Uint64, KRG::uint64 );
        REGISTER_TYPE_RECORD( CoreTypeID::Int64, KRG::int64 );
        REGISTER_TYPE_RECORD( CoreTypeID::Float, float );
        REGISTER_TYPE_RECORD( CoreTypeID::Double, double );
        REGISTER_TYPE_RECORD( CoreTypeID::UUID, KRG::UUID );
        REGISTER_TYPE_RECORD( CoreTypeID::StringID, KRG::StringID );
        REGISTER_TYPE_RECORD( CoreTypeID::Tag, KRG::Tag );
        REGISTER_TYPE_RECORD( CoreTypeID::TypeID, KRG::TypeSystem::TypeID );
        REGISTER_TYPE_RECORD( CoreTypeID::String, KRG::String );
        REGISTER_TYPE_RECORD( CoreTypeID::Color, KRG::Color );
        REGISTER_TYPE_RECORD( CoreTypeID::Float2, KRG::Float2 );
        REGISTER_TYPE_RECORD( CoreTypeID::Float3, KRG::Float3 );
        REGISTER_TYPE_RECORD( CoreTypeID::Float4, KRG::Float4 );
        REGISTER_TYPE_RECORD( CoreTypeID::Vector, KRG::Vector );
        REGISTER_TYPE_RECORD( CoreTypeID::Quaternion, KRG::Quaternion );
        REGISTER_TYPE_RECORD( CoreTypeID::Matrix, KRG::Matrix );
        REGISTER_TYPE_RECORD( CoreTypeID::Transform, KRG::Transform );
        REGISTER_TYPE_RECORD( CoreTypeID::Microseconds, KRG::Microseconds );
        REGISTER_TYPE_RECORD( CoreTypeID::Milliseconds, KRG::Milliseconds );
        REGISTER_TYPE_RECORD( CoreTypeID::Seconds, KRG::Seconds );
        REGISTER_TYPE_RECORD( CoreTypeID::Percentage, KRG::Percentage );
        REGISTER_TYPE_RECORD( CoreTypeID::Degrees, KRG::Degrees );
        REGISTER_TYPE_RECORD( CoreTypeID::Radians, KRG::Radians );
        REGISTER_TYPE_RECORD( CoreTypeID::EulerAngles, KRG::EulerAngles );
        REGISTER_TYPE_RECORD( CoreTypeID::IntRange, KRG::IntRange );
        REGISTER_TYPE_RECORD( CoreTypeID::FloatRange, KRG::FloatRange );
        REGISTER_TYPE_RECORD( CoreTypeID::FloatCurve, KRG::FloatCurve );

        REGISTER_TYPE_RECORD( CoreTypeID::BitFlags, KRG::BitFlags );
        REGISTER_TEMPLATE_TYPE_RECORD_GENERIC( CoreTypeID::TBitFlags, KRG::TBitFlags, enum class TempEnum );

        REGISTER_TEMPLATE_TYPE_RECORD_GENERIC( CoreTypeID::TVector, KRG::TVector, uint8 );

        // Resources
        REGISTER_TYPE_RECORD( CoreTypeID::ResourcePath, KRG::ResourcePath );
        REGISTER_TYPE_RECORD( CoreTypeID::ResourceID, KRG::ResourceID );
        REGISTER_TYPE_RECORD( CoreTypeID::ResourceTypeID, KRG::ResourceTypeID );
        REGISTER_TYPE_RECORD( CoreTypeID::ResourcePtr, KRG::Resource::ResourcePtr );
        REGISTER_TEMPLATE_TYPE_RECORD_RESOURCE( CoreTypeID::TResourcePtr, KRG::TResourcePtr );

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

    CoreTypeID CoreTypeRegistry::GetType( TypeID typeID )
    {
        KRG_ASSERT( s_areCoreTypeRecordsInitialized );

        for ( int32 i = 0; i < (int32) CoreTypeID::NumTypes; i++ )
        {
            if ( CoreTypeRegistry::s_coreTypeRecords[i].m_ID == typeID )
            {
                return (CoreTypeID) i;
            }
        }

        return CoreTypeID::Invalid;
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