#include "EntityCollectionModelReader.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/Collections/EntityCollectionDescriptor.h"
#include "System/TypeSystem/CoreTypeConversions.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/error/en.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace EntityModel
    {
        static bool Error( char const* pFormat, ... )
        {
            va_list args;
            va_start( args, pFormat );
            Log::AddEntryVarArgs( Log::Severity::Error, "Entity Collection Reader", __FILE__, __LINE__, pFormat, args );
            va_end( args );
            return false;
        }

        //-------------------------------------------------------------------------
        // Parsing
        //-------------------------------------------------------------------------

        namespace 
        {
            struct ParsingContext
            {
                ParsingContext( TypeSystem::TypeRegistry const& typeRegistry ) : m_typeRegistry( typeRegistry ) {}

                inline bool DoesComponentExist( UUID componentID ) const
                {
                    return m_componentIDs.find( componentID ) != m_componentIDs.end();
                }

                inline bool DoesEntityExist( UUID entityID ) const
                {
                    return m_entityIDs.find( entityID ) != m_entityIDs.end();
                }

            public:

                TypeSystem::TypeRegistry const&             m_typeRegistry;

                // Parsing context ID - Entity/Component/etc...
                UUID                                        m_parsingContextID;

                // Maps to allow for fast lookups of entityIDs for validation
                THashMap<UUID, bool>                        m_entityIDs;
                THashMap<UUID, bool>                        m_componentIDs;
            };

            //-------------------------------------------------------------------------

            static void ReadPropertyValue( ParsingContext& ctx, RapidJsonValue::ConstMemberIterator memberIter, TypeSystem::PropertyDescriptor& outPropertyDesc )
            {
                KRG_ASSERT( !memberIter->value.IsArray() ); // TODO: arrays not supported yet
                outPropertyDesc = TypeSystem::PropertyDescriptor( TypeSystem::PropertyPath( memberIter->name.GetString() ), memberIter->value.GetString() );
            }

            static bool ReadAndConvertPropertyValue( ParsingContext& ctx, TypeSystem::TypeInfo const* pTypeInfo, RapidJsonValue::ConstMemberIterator memberIter, TypeSystem::PropertyDescriptor& outPropertyDesc )
            {
                ReadPropertyValue( ctx, memberIter, outPropertyDesc );

                //-------------------------------------------------------------------------

                auto const pPropertyInfo = ctx.m_typeRegistry.ResolvePropertyPath( pTypeInfo, outPropertyDesc.m_path );
                if ( pPropertyInfo == nullptr )
                {
                    return false;
                }

                if ( TypeSystem::IsCoreType( pPropertyInfo->m_typeID ) || pPropertyInfo->IsEnumProperty() || pPropertyInfo->IsBitFlagsProperty() )
                {
                    TypeSystem::Conversion::ConvertStringToBinary( ctx.m_typeRegistry, *pPropertyInfo, outPropertyDesc.m_stringValue, outPropertyDesc.m_byteValue );
                }

                return true;
            }

            static bool ReadComponent( ParsingContext& ctx, RapidJsonValue const& componentObject, EntityComponentDescriptor& outComponentDesc )
            {
                // Read name and ID
                //-------------------------------------------------------------------------

                auto IDIter = componentObject.FindMember( "ID" );
                if ( IDIter == componentObject.MemberEnd() || !IDIter->value.IsString() || !UUID::IsValidUUIDString( IDIter->value.GetString() ) )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                outComponentDesc.m_ID = UUID( IDIter->value.GetString() );

                auto nameIter = componentObject.FindMember( "Name" );
                if ( nameIter == componentObject.MemberEnd() || !nameIter->value.IsString() || nameIter->value.GetStringLength() == 0 )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                outComponentDesc.m_name = StringID( nameIter->value.GetString() );

                // Validate type ID
                //-------------------------------------------------------------------------

                auto typeDataIter = componentObject.FindMember( "TypeData" );
                if ( typeDataIter == componentObject.MemberEnd() || !typeDataIter->value.IsObject() )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and Type Data values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                RapidJsonValue const& componentTypeDataObject = typeDataIter->value;

                auto typeIDIter = componentTypeDataObject.FindMember( TypeSystem::Serialization::Constants::s_typeID );
                if ( typeIDIter == componentTypeDataObject.MemberEnd() || !typeIDIter->value.IsString() )
                {
                    Error( "Invalid type data found for component: '%s' on entity %s!", nameIter->value.GetString(), ctx.m_parsingContextID.ToString().c_str() );
                    return false;
                }

                outComponentDesc.m_typeID = StringID( typeIDIter->value.GetString() );

                // Spatial component info
                //-------------------------------------------------------------------------

                auto pTypeInfo = ctx.m_typeRegistry.GetTypeInfo( outComponentDesc.m_typeID );
                if ( pTypeInfo == nullptr )
                {
                    return Error( "Invalid entity component type ID detected for entity (%s): %s", ctx.m_parsingContextID.ToString().c_str(), outComponentDesc.m_typeID.c_str() );
                }

                outComponentDesc.m_isSpatialComponent = pTypeInfo->IsDerivedFrom<SpatialEntityComponent>();

                if ( outComponentDesc.m_isSpatialComponent )
                {
                    auto spatialParentIter = componentObject.FindMember( "SpatialParent" );
                    if ( spatialParentIter != componentObject.MemberEnd() && spatialParentIter->value.IsString() )
                    {
                        outComponentDesc.m_spatialParentID = UUID( spatialParentIter->value.GetString() );
                    }

                    auto attachmentSocketIter = componentObject.FindMember( "AttachmentSocketID" );
                    if ( attachmentSocketIter != componentObject.MemberEnd() && attachmentSocketIter->value.IsString() )
                    {
                        outComponentDesc.m_attachmentSocketID = StringID( attachmentSocketIter->value.GetString() );
                    }
                }

                // Read property overrides
                //-------------------------------------------------------------------------

                // Reserve memory for all property (+1 extra slot) and create an empty desc
                outComponentDesc.m_properties.reserve( componentTypeDataObject.Size() );
                outComponentDesc.m_properties.push_back( TypeSystem::PropertyDescriptor() );

                // Read all properties
                for ( auto itr = componentTypeDataObject.MemberBegin(); itr != componentTypeDataObject.MemberEnd(); ++itr )
                {
                    // Skip Type ID
                    if ( strcmp( itr->name.GetString(), TypeSystem::Serialization::Constants::s_typeID ) == 0 )
                    {
                        continue;
                    }

                    // If we successfully read the property value add a new property value
                    if ( ReadAndConvertPropertyValue( ctx, pTypeInfo, itr, outComponentDesc.m_properties.back() ) )
                    {
                        outComponentDesc.m_properties.push_back( TypeSystem::PropertyDescriptor() );
                    }
                }

                // Remove last entry as it will always be empty
                outComponentDesc.m_properties.pop_back();

                //-------------------------------------------------------------------------

                if ( ctx.DoesComponentExist( outComponentDesc.m_ID ) )
                {
                    Error( "Duplicate component UUID detected: '%s' on entity %s!", outComponentDesc.m_ID.ToString().c_str(), ctx.m_parsingContextID.ToString().c_str() );
                    return false;
                }
                else
                {
                    ctx.m_componentIDs.insert( TPair<UUID, bool>( outComponentDesc.m_ID, true ) );
                    return true;
                }
            }

            //-------------------------------------------------------------------------

            static bool ReadSystemData( ParsingContext& ctx, RapidJsonValue const& systemObject, EntitySystemDescriptor& outSystemDesc )
            {
                auto typeIDIter = systemObject.FindMember( TypeSystem::Serialization::Constants::s_typeID );
                if ( typeIDIter == systemObject.MemberEnd() || !typeIDIter->value.IsString() )
                {
                    return Error( "Invalid entity system format (systems must have a TypeID string value set) on entity %s", ctx.m_parsingContextID.ToString().c_str() );
                }

                if ( typeIDIter->value.GetStringLength() == 0 )
                {
                    return Error( "Invalid entity system format (systems must have a TypeID string value set) on entity %s", ctx.m_parsingContextID.ToString().c_str() );
                }

                outSystemDesc.m_typeID = StringID( typeIDIter->value.GetString() );
                return true;
            }

            //-------------------------------------------------------------------------

            static bool ReadEntityData( ParsingContext& ctx, RapidJsonValue const& entityObject, EntityDescriptor& outEntityDesc )
            {
                // Read name and ID
                //-------------------------------------------------------------------------

                auto IDIter = entityObject.FindMember( "ID" );
                if ( IDIter == entityObject.MemberEnd() || !IDIter->value.IsString() || !UUID::IsValidUUIDString( IDIter->value.GetString() ) )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                outEntityDesc.m_ID = UUID( IDIter->value.GetString() );

                auto nameIter = entityObject.FindMember( "Name" );
                if ( nameIter == entityObject.MemberEnd() || !nameIter->value.IsString() || nameIter->value.GetStringLength() == 0 )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                outEntityDesc.m_name = StringID( nameIter->value.GetString() );

                // Read spatial info
                //-------------------------------------------------------------------------

                auto spatialParentIter = entityObject.FindMember( "SpatialParent" );
                if ( spatialParentIter != entityObject.MemberEnd() && spatialParentIter->value.IsString() )
                {
                    outEntityDesc.m_spatialParentID = UUID( spatialParentIter->value.GetString() );
                }

                auto attachmentSocketIter = entityObject.FindMember( "AttachmentSocketID" );
                if ( attachmentSocketIter != entityObject.MemberEnd() && attachmentSocketIter->value.IsString() )
                {
                    outEntityDesc.m_attachmentSocketID = StringID( attachmentSocketIter->value.GetString() );
                }

                // Set parsing ctx ID
                //-------------------------------------------------------------------------

                ctx.m_parsingContextID = outEntityDesc.m_ID;

                //-------------------------------------------------------------------------
                // Read components
                //-------------------------------------------------------------------------

                auto componentsArrayIter = entityObject.FindMember( "Components" );
                if ( componentsArrayIter != entityObject.MemberEnd() && componentsArrayIter->value.IsArray() )
                {
                    // Read component data
                    //-------------------------------------------------------------------------

                    int32 const numComponents = (int32) componentsArrayIter->value.Size();
                    KRG_ASSERT( outEntityDesc.m_components.empty() && outEntityDesc.m_numSpatialComponents == 0 );
                    outEntityDesc.m_components.resize( numComponents );

                    for ( int32 i = 0; i < numComponents; i++ )
                    {
                        if ( !ReadComponent( ctx, componentsArrayIter->value[i], outEntityDesc.m_components[i] ) )
                        {
                            return false;
                        }

                        if ( outEntityDesc.m_components[i].IsSpatialComponent() )
                        {
                            outEntityDesc.m_numSpatialComponents++;
                        }
                    }

                    // Sort Components
                    //-------------------------------------------------------------------------

                    auto comparator = [] ( EntityComponentDescriptor const& componentDescA, EntityComponentDescriptor const& componentDescB )
                    {
                        // If they are both spatial components, ensure that the root component is first
                        if ( componentDescA.IsSpatialComponent() && componentDescB.IsSpatialComponent() )
                        {
                            return componentDescA.IsRootComponent();
                        }

                        // Do not sort regular components
                        if ( !componentDescA.IsSpatialComponent() && !componentDescB.IsSpatialComponent() )
                        {
                            return false;
                        }

                        // If comparing a spatial and regular component, spatial components take precedence
                        return componentDescA.IsSpatialComponent();
                    };

                    eastl::sort( outEntityDesc.m_components.begin(), outEntityDesc.m_components.end(), comparator );
                }

                //-------------------------------------------------------------------------
                // Read systems
                //-------------------------------------------------------------------------

                auto systemsArrayIter = entityObject.FindMember( "Systems" );
                if ( systemsArrayIter != entityObject.MemberEnd() && systemsArrayIter->value.IsArray() )
                {
                    KRG_ASSERT( outEntityDesc.m_systems.empty() );

                    outEntityDesc.m_systems.resize( systemsArrayIter->value.Size() );

                    for ( rapidjson::SizeType i = 0; i < systemsArrayIter->value.Size(); i++ )
                    {
                        if ( !ReadSystemData( ctx, systemsArrayIter->value[i], outEntityDesc.m_systems[i] ) )
                        {
                            return false;
                        }
                    }
                }

                //-------------------------------------------------------------------------

                ctx.m_parsingContextID.Reset();

                //-------------------------------------------------------------------------

                if ( ctx.DoesEntityExist( outEntityDesc.m_ID ) )
                {
                    return Error( "Duplicate entity ID detected: %s", outEntityDesc.m_ID.ToString().c_str() );
                }
                else
                {
                    ctx.m_entityIDs.insert( TPair<UUID, bool>( outEntityDesc.m_ID, true ) );
                    return true;
                }
            }

            static bool ReadEntityArray( ParsingContext& ctx, RapidJsonValue const& entitiesArrayValue, EntityCollectionDescriptor& outCollection )
            {
                int32 const numEntities = (int32) entitiesArrayValue.Size();
                outCollection.Reserve( numEntities );
                for ( int32 i = 0; i < numEntities; i++ )
                {
                    if ( !entitiesArrayValue[i].IsObject() )
                    {
                        return Error( "Malformed collection file, entities array can only contain objects" );
                    }

                    EntityDescriptor entityDesc;
                    if ( !ReadEntityData( ctx, entitiesArrayValue[i], entityDesc ) )
                    {
                        return false;
                    }

                    outCollection.AddEntity( entityDesc );
                }

                return true;
            }
        }

        //-------------------------------------------------------------------------
        // Serializer
        //-------------------------------------------------------------------------

        bool EntityCollectionModelReader::ReadCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& filePath, EntityCollectionDescriptor& outCollection )
        {
            KRG_ASSERT( filePath.IsValid() );

            //-------------------------------------------------------------------------

            if ( !FileSystem::Exists( filePath ) )
            {
                return Error( "Cant read source file %s", filePath.GetFullPath().c_str() );
            }

            //-------------------------------------------------------------------------

            outCollection.Clear();

            //-------------------------------------------------------------------------

            // Load file into memory buffer
            FILE* fp = fopen( filePath.c_str(), "r" );
            fseek( fp, 0, SEEK_END );
            size_t filesize = (size_t) ftell( fp );
            fseek( fp, 0, SEEK_SET );

            TVector<Byte> fileBuffer;
            fileBuffer.resize( filesize + 1 );
            size_t readLength = fread( fileBuffer.data(), 1, filesize, fp );
            fileBuffer[readLength] = '\0';
            fclose( fp );

            // Parse JSON
            //-------------------------------------------------------------------------

            rapidjson::Document entityCollectionDocument;
            rapidjson::ParseResult result = entityCollectionDocument.ParseInsitu( (char*) fileBuffer.data() );
            if ( result.IsError() )
            {
                return Error( "Failed to parse JSON: %s", rapidjson::GetParseError_En( result.Code() ) );
            }

            if ( !entityCollectionDocument.HasMember( "Entities" ) || !entityCollectionDocument["Entities"].IsArray() )
            {
                return Error( "Invalid format for entity collection file, missing root entities array" );
            }

            // Read Entities
            //-------------------------------------------------------------------------

            ParsingContext ctx( typeRegistry );

            if ( !ReadEntityArray( ctx, entityCollectionDocument["Entities"], outCollection ) )
            {
                return false;
            }

            outCollection.GenerateSpatialAttachmentInfo();

            //-------------------------------------------------------------------------

            return true;
        }
    }
}