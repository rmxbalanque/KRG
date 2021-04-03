#include "EntityCollectionReader.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerializationCommon.h"
#include "System/Entity/Entity.h"
#include "System/Entity/Collections/EntityCollectionDescriptor.h"
#include "System/TypeSystem/CoreTypeConversions.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/error/en.h"
#include "System/Core/Logging/Log.h"

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

            static void ReadPropertyValue( ParsingContext& ctx, rapidjson::Value::ConstMemberIterator memberIter, TypeSystem::PropertyDescriptor& outPropertyDesc )
            {
                outPropertyDesc = TypeSystem::PropertyDescriptor( TypeSystem::PropertyPath( memberIter->name.GetString() ), memberIter->value.GetString() );
            }

            static bool ReadAndConvertPropertyValue( ParsingContext& ctx, TypeSystem::TypeInfo const* pTypeInfo, rapidjson::Value::ConstMemberIterator memberIter, TypeSystem::PropertyDescriptor& outPropertyDesc )
            {
                ReadPropertyValue( ctx, memberIter, outPropertyDesc );

                //-------------------------------------------------------------------------

                auto const resolvedPropertyInfo = ctx.m_typeRegistry.ResolvePropertyPath( pTypeInfo, outPropertyDesc.m_path );
                if ( !resolvedPropertyInfo.IsValid() )
                {
                    return false;
                }

                if ( TypeSystem::IsCoreType( resolvedPropertyInfo.m_pPropertyInfo->m_typeID ) || resolvedPropertyInfo.m_pPropertyInfo->IsEnumProperty() || resolvedPropertyInfo.m_pPropertyInfo->IsBitFlagsProperty() )
                {
                    TypeSystem::Conversion::ConvertStringToBinary( ctx.m_typeRegistry, *resolvedPropertyInfo.m_pPropertyInfo, outPropertyDesc.m_stringValue, outPropertyDesc.m_byteValue );
                }

                return true;
            }

            static bool ReadComponent( ParsingContext& ctx, rapidjson::Value const& componentObject, EntityComponentDescriptor& outComponentDesc )
            {
                // Read name and ID
                //-------------------------------------------------------------------------

                if ( !( componentObject.HasMember( "ID" ) && componentObject["ID"].IsString() && UUID::IsValidUUIDString( componentObject["ID"].GetString() ) ) )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                outComponentDesc.m_ID = UUID( componentObject["ID"].GetString() );

                if ( !( componentObject.HasMember( "Name" ) && componentObject["Name"].IsString() && strlen( componentObject["Name"].GetString() ) > 0 ) )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                outComponentDesc.m_name = StringID( componentObject["Name"].GetString() );

                // Validate type ID
                //-------------------------------------------------------------------------

                if ( !( componentObject.HasMember( "TypeData" ) && componentObject["TypeData"].IsObject() ) )
                {
                    return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextID.ToString().c_str() );
                }

                rapidjson::Value const& componentTypeDataObject = componentObject["TypeData"];
                if ( !componentTypeDataObject.HasMember( Serialization::TypeSerialization::Key_TypeID ) )
                {
                    Error( "Invalid type data found for component: '%s' on entity %s!", componentObject["Name"].GetString(), ctx.m_parsingContextID.ToString().c_str() );
                    return false;
                }

                outComponentDesc.m_typeID = StringID( componentTypeDataObject[Serialization::TypeSerialization::Key_TypeID].GetString() );

                // Spatial component info
                //-------------------------------------------------------------------------

                auto pTypeInfo = ctx.m_typeRegistry.GetTypeInfo( outComponentDesc.m_typeID );
                if ( pTypeInfo == nullptr )
                {
                    return Error( "Invalid entity component type ID detected for entity (%s): %s", ctx.m_parsingContextID.ToString().c_str(), outComponentDesc.m_typeID.GetAsStringID().c_str() );
                }

                outComponentDesc.m_isSpatialComponent = pTypeInfo->IsDerivedFrom<SpatialEntityComponent>();

                if ( outComponentDesc.m_isSpatialComponent )
                {
                    if ( componentObject.HasMember( "SpatialParent" ) )
                    {
                        outComponentDesc.m_spatialParentID = UUID( componentObject["SpatialParent"].GetString() );
                    }

                    if ( componentObject.HasMember( "AttachmentSocketID" ) )
                    {
                        outComponentDesc.m_attachmentSocketID = StringID( componentObject["AttachmentSocketID"].GetString() );
                    }
                }

                // Read property overrides
                //-------------------------------------------------------------------------

                // Reserve memory for all property (+1 extra slot) and create an empty desc
                outComponentDesc.m_propertyValues.reserve( componentTypeDataObject.Size() );
                outComponentDesc.m_propertyValues.push_back( TypeSystem::PropertyDescriptor() );

                // Read all properties
                for ( auto itr = componentTypeDataObject.MemberBegin(); itr != componentTypeDataObject.MemberEnd(); ++itr )
                {
                    // Skip Type ID
                    if ( strcmp( itr->name.GetString(), Serialization::TypeSerialization::Key_TypeID ) == 0 )
                    {
                        continue;
                    }

                    // If we successfully read the property value add a new property value
                    if ( ReadAndConvertPropertyValue( ctx, pTypeInfo, itr, outComponentDesc.m_propertyValues.back() ) )
                    {
                        outComponentDesc.m_propertyValues.push_back( TypeSystem::PropertyDescriptor() );
                    }
                }

                // Remove last entry as it will always be empty
                outComponentDesc.m_propertyValues.pop_back();

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

            static bool ReadSystemData( ParsingContext& ctx, rapidjson::Value const& systemObject, EntitySystemDescriptor& outSystemDesc )
            {
                if ( !systemObject.HasMember( Serialization::TypeSerialization::Key_TypeID ) || !systemObject[Serialization::TypeSerialization::Key_TypeID].IsString() || strlen( systemObject[Serialization::TypeSerialization::Key_TypeID].GetString() ) == 0 )
                {
                    return Error( "Invalid entity system format (systems must have a TypeID string value set) on entity %s", ctx.m_parsingContextID.ToString().c_str() );
                }

                outSystemDesc.m_typeID = StringID( systemObject[Serialization::TypeSerialization::Key_TypeID].GetString() );
                return true;
            }

            //-------------------------------------------------------------------------

            static bool ReadEntityData( ParsingContext& ctx, rapidjson::Value const& entityObject, EntityDescriptor& outEntityDesc )
            {
                bool const validID = entityObject.HasMember( "ID" ) && entityObject["ID"].IsString() && UUID::IsValidUUIDString( entityObject["ID"].GetString() );
                bool const validName = entityObject.HasMember( "Name" ) && entityObject["Name"].IsString() && strlen( entityObject["Name"].GetString() ) > 0;
                if ( !validID || !validName )
                {
                    Error( "Invalid entity format: entities must have value ID and Name string values set" );
                    return false;
                }

                //-------------------------------------------------------------------------

                outEntityDesc.m_ID = UUID( entityObject["ID"].GetString() );
                outEntityDesc.m_name = StringID( entityObject["Name"].GetString() );

                if ( entityObject.HasMember( "SpatialParent" ) )
                {
                    outEntityDesc.m_spatialParentID = UUID( entityObject["SpatialParent"].GetString() );
                }

                if ( entityObject.HasMember( "AttachmentSocketID" ) )
                {
                    outEntityDesc.m_attachmentSocketID = StringID( entityObject["AttachmentSocketID"].GetString() );
                }

                // Set parsing ctx ID
                //-------------------------------------------------------------------------

                ctx.m_parsingContextID = outEntityDesc.m_ID;

                //-------------------------------------------------------------------------
                // Read components
                //-------------------------------------------------------------------------

                if ( entityObject.HasMember( "Components" ) )
                {
                    rapidjson::Value const& componentsArrayValue = entityObject["Components"];
                    if ( !componentsArrayValue.IsArray() )
                    {
                        return Error( "Invalid format: 'Components' value must be an array" );
                    }

                    // Read component data
                    //-------------------------------------------------------------------------

                    int32 const numComponents = (int32) componentsArrayValue.Size();
                    KRG_ASSERT( outEntityDesc.m_components.empty() && outEntityDesc.m_numSpatialComponents == 0 );
                    outEntityDesc.m_components.resize( numComponents );

                    for ( int32 i = 0; i < numComponents; i++ )
                    {
                        if ( !ReadComponent( ctx, componentsArrayValue[i], outEntityDesc.m_components[i] ) )
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

                if ( entityObject.HasMember( "Systems" ) )
                {
                    KRG_ASSERT( outEntityDesc.m_systems.empty() );

                    rapidjson::Value const& systemsArrayValue = entityObject["Systems"];
                    if ( !systemsArrayValue.IsArray() )
                    {
                        Error( "Invalid entity format: systems values must be an array" );
                        return false;
                    }

                    outEntityDesc.m_systems.resize( systemsArrayValue.Size() );

                    for ( rapidjson::SizeType i = 0; i < systemsArrayValue.Size(); i++ )
                    {
                        if ( !ReadSystemData( ctx, systemsArrayValue[i], outEntityDesc.m_systems[i] ) )
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

            static bool ReadEntityArray( ParsingContext& ctx, rapidjson::Value const& entitiesArrayValue, EntityCollectionDescriptor& outCollection )
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

        bool EntityCollectionReader::ReadCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& filePath, EntityCollectionDescriptor& outCollection )
        {
            KRG_ASSERT( filePath.IsValid() );

            //-------------------------------------------------------------------------

            if ( !filePath.Exists() )
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