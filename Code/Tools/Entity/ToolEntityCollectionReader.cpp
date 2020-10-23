#include "ToolEntityCollectionReader.h"
#include "ToolEntityCollection.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "Tools/Core/TypeSystem/DynamicTypeInstance.h"
#include "Tools/Core/TypeSystem/DynamicTypeSerialization.h"
#include "Tools/Core/TypeSystem/ToolSerializationCommon.h"
#include "System/Entity/Entity.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/error/en.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        static bool Error( char const* pFormat, ... )
        {
            va_list args;
            va_start( args, pFormat );
            Log::AddEntryVarArgs( Log::Severity::Error, "Entity Collection Reader", __FILE__, __LINE__, pFormat, args );
            va_end( args );
            return false;
        }

        static bool Warning( char const* pFormat, ... )
        {
            va_list args;
            va_start( args, pFormat );
            Log::AddEntryVarArgs( Log::Severity::Warning, "Entity Collection Reader", __FILE__, __LINE__, pFormat, args );
            va_end( args );
            return false;
        }

        //-------------------------------------------------------------------------
        // Parsing
        //-------------------------------------------------------------------------

        struct RawPropertyData
        {
            TypeSystem::PropertyPath                m_path;
            String                                  m_value;
        };

        struct RawComponentData
        {
            UUID                                    m_ID;
            UUID                                    m_spatialParentID;
            TypeSystem::TypeID                      m_typeID;
            StringID                                m_attachmentSocketID;
            StringID                                m_name;
            TInlineVector<RawPropertyData, 10>      m_propertyData;
        };

        struct RawSystemData
        {
            TypeSystem::TypeID                      m_typeID;
        };

        struct RawEntityData
        {
            UUID                                    m_ID;
            UUID                                    m_spatialParentID;
            StringID                                m_name;
            StringID                                m_attachmentSocketID;
            TInlineVector<RawSystemData, 5>         m_systems;
            TInlineVector<RawComponentData,6>       m_components;
        };

        //-------------------------------------------------------------------------

        static bool ReadRawComponentArray( rapidjson::Value const& objectValue, THashMap<UUID, bool>& globalComponentIDMap, UUID const& parentID, TVector<RawComponentData*>& outComponents );

        static bool ReadRawComponentData( rapidjson::Value const& componentObject, THashMap<UUID, bool>& globalComponentIDMap, UUID const& parentEntityID, RawComponentData* pOutComponentData )
        {
            KRG_ASSERT( pOutComponentData != nullptr );

            // Validate type ID
            rapidjson::Value const& componentTypeDataObject = componentObject["TypeData"];

            if ( !componentTypeDataObject.IsObject() || !componentTypeDataObject.HasMember( Serialization::TypeSerializationSettings::TypeKey ) || strlen( componentTypeDataObject[Serialization::TypeSerializationSettings::TypeKey].GetString() ) == 0 )
            {
                Error( "Invalid type data found for component: '%s' on entity %s!", componentObject["Name"].GetString(), parentEntityID.ToString().c_str() );
                return false;
            }

            auto typeIDMember = componentTypeDataObject.FindMember( Serialization::TypeSerializationSettings::TypeKey );

            //-------------------------------------------------------------------------

            pOutComponentData->m_typeID = StringID( typeIDMember->value.GetString() );
            pOutComponentData->m_ID = UUID( componentObject["ID"].GetString() );
            pOutComponentData->m_name = StringID( componentObject["Name"].GetString() );

            if ( componentObject.HasMember( "AttachmentSocketID" ) )
            {
                pOutComponentData->m_attachmentSocketID = StringID( componentObject["AttachmentSocketID"].GetString() );
            }

            //-------------------------------------------------------------------------

            S32 const numPropertyValues = componentTypeDataObject.Size() - 1;
            pOutComponentData->m_propertyData.resize( numPropertyValues );

            S32 propertyIdx = 0;
            for ( auto itr = componentTypeDataObject.MemberBegin(); itr != componentTypeDataObject.MemberEnd(); ++itr )
            {
                if ( strcmp( itr->name.GetString(), TypeSerializationSettings::TypeKey ) == 0 )
                {
                    continue;
                }

                pOutComponentData->m_propertyData[propertyIdx].m_path = TypeSystem::PropertyPath( itr->name.GetString() );
                pOutComponentData->m_propertyData[propertyIdx].m_value = itr->value.GetString();
                propertyIdx++;
            }

            //-------------------------------------------------------------------------

            return ReadRawComponentArray( componentObject, globalComponentIDMap, parentEntityID, pOutComponentData->m_childComponents );
        }

        static bool ReadRawComponentArray( rapidjson::Value const& objectValue, THashMap<UUID, bool>& globalComponentIDMap, UUID const& parentID, TVector<RawComponentData*>& outComponents )
        {
            if ( objectValue.HasMember( "Components" ) )
            {
                rapidjson::Value const& componentsArrayValue = objectValue["Components"];

                if ( !componentsArrayValue.IsArray() )
                {
                    return Error( "Invalid format: 'Components' value must be an array" );
                }

                S32 const numComponents = (S32) componentsArrayValue.Size();
                KRG_ASSERT( outComponents.empty() );
                outComponents.resize( numComponents );

                for ( S32 i = 0; i < numComponents; i++ )
                {
                    rapidjson::Value const& componentObject = componentsArrayValue[i];

                    outComponents[i] = KRG::New<RawComponentData>();

                    // Read Component Data
                    //-------------------------------------------------------------------------

                    if ( !( componentObject.HasMember( "ID" ) && componentObject["ID"].IsString() && UUID::IsValidUUIDString( componentObject["ID"].GetString() ) ) )
                    {
                        return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", parentID.ToString().c_str() );
                    }
                    else if ( !( componentObject.HasMember( "Name" ) && componentObject["Name"].IsString() && strlen( componentObject["Name"].GetString() ) > 0 ) )
                    {
                        return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", parentID.ToString().c_str() );
                    }
                    else if ( !( componentObject.HasMember( "TypeData" ) && componentObject["TypeData"].IsObject() ) )
                    {
                        return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", parentID.ToString().c_str() );
                    }

                    if ( !ReadRawComponentData( componentObject, globalComponentIDMap, parentID, outComponents[i] ) )
                    {
                        return false;
                    }

                    // Ensure component IDs are unique
                    //-------------------------------------------------------------------------

                    if ( globalComponentIDMap.find( outComponents[i]->m_ID ) != globalComponentIDMap.end() )
                    {
                        return Error( "Duplicate component ID (%s) detected for entity (%s)", outComponents[i]->m_ID.ToString().c_str(), parentID.ToString().c_str() );
                    }
                    else
                    {
                        globalComponentIDMap.insert( TPair<UUID, bool>( outComponents[i]->m_ID, true ) );
                    }
                }
            }

            return true;
        }

        //-------------------------------------------------------------------------

        static bool ReadRawSystemData( rapidjson::Value const& systemObject, UUID const& parentEntityID, RawSystemData& outSystemData )
        {
            if ( !systemObject.HasMember( Serialization::TypeSerializationSettings::TypeKey ) || !systemObject[Serialization::TypeSerializationSettings::TypeKey].IsString() || strlen( systemObject[Serialization::TypeSerializationSettings::TypeKey].GetString() ) == 0 )
            {
                return Error( "Invalid entity system format (systems must have a TypeID string value set) on entity %s", parentEntityID.ToString().c_str() );
            }

            outSystemData.m_typeID = StringID( systemObject[Serialization::TypeSerializationSettings::TypeKey].GetString() );
            return true;
        }

        //-------------------------------------------------------------------------

        static bool ReadRawEntityArray( rapidjson::Value const& objectValue, THashMap<UUID, RawEntityData*>& globalEntityMap, THashMap<UUID, bool>& rawComponentIDMap, TVector<RawEntityData*>& outEntities );

        static bool ReadRawEntityData( rapidjson::Value const& entityObject, THashMap<UUID, RawEntityData*>& globalEntityMap, THashMap<UUID, bool>& globalComponentIDMap, RawEntityData* pOutEntityData )
        {
            bool const validID = entityObject.HasMember( "ID" ) && entityObject["ID"].IsString() && UUID::IsValidUUIDString( entityObject["ID"].GetString() );
            bool const validName = entityObject.HasMember( "Name" ) && entityObject["Name"].IsString() && strlen( entityObject["Name"].GetString() ) > 0;
            if ( !validID || !validName )
            {
                Error( "Invalid entity format: entities must have value ID and Name string values set" );
                return false;
            }

            //-------------------------------------------------------------------------

            pOutEntityData->m_ID = UUID( entityObject["ID"].GetString() );
            pOutEntityData->m_name = StringID( entityObject["Name"].GetString() );

            if ( entityObject.HasMember( "AttachmentSocketID" ) )
            {
                pOutEntityData->m_attachmentSocketID = StringID( entityObject["AttachmentSocketID"].GetString() );
            }

            // Ensure unique entity UUID
            //-------------------------------------------------------------------------

            if ( globalEntityMap.find( pOutEntityData->m_ID ) != globalEntityMap.end() )
            {
                return Error( "Duplicate entity ID detected: %s", pOutEntityData->m_ID.ToString().c_str() );
            }
            else
            {
                globalEntityMap.insert( TPair<UUID, RawEntityData*>( pOutEntityData->m_ID, pOutEntityData ) );
            }

            // Read components
            //-------------------------------------------------------------------------

            if ( !ReadRawComponentArray( entityObject, globalComponentIDMap, pOutEntityData->m_ID, pOutEntityData->m_components ) )
            {
                return false;
            }

            // Read systems
            //-------------------------------------------------------------------------

            if ( entityObject.HasMember( "Systems" ) )
            {
                rapidjson::Value const& systemsArrayValue = entityObject["Systems"];

                if ( !systemsArrayValue.IsArray() )
                {
                    Error( "Invalid entity format: systems values must be an array" );
                    return false;
                }

                pOutEntityData->m_systems.resize( systemsArrayValue.Size() );

                for ( rapidjson::SizeType i = 0; i < systemsArrayValue.Size(); i++ )
                {
                    rapidjson::Value const& systemObject = systemsArrayValue[i];

                    bool const validSystemTypeID = systemObject.HasMember( Serialization::TypeSerializationSettings::TypeKey ) && systemObject[Serialization::TypeSerializationSettings::TypeKey].IsString() && strlen( systemObject[Serialization::TypeSerializationSettings::TypeKey].GetString() ) > 0;
                    if ( !validSystemTypeID )
                    {
                        Error( "Invalid entity system format detected for entity (%s): systems must have TypeID string values set", pOutEntityData->m_name.ToString() );
                        return false;
                    }

                    if ( !ReadRawSystemData( systemObject, pOutEntityData->m_ID, pOutEntityData->m_systems[i] ) )
                    {
                        return false;
                    }
                }
            }

            // Read child entities
            //-------------------------------------------------------------------------

            return ReadRawEntityArray( entityObject, globalEntityMap, globalComponentIDMap, pOutEntityData->m_childEntities );
        }

        static bool ReadRawEntityArray( rapidjson::Value const& objectValue, THashMap<UUID, RawEntityData*>& globalEntityMap, THashMap<UUID, bool>& globalComponentIDMap, TVector<RawEntityData*>& outEntities )
        {
            if ( objectValue.HasMember( "Entities" )  )
            {
                rapidjson::Value const& entitiesArrayValue = objectValue["Entities"];

                if ( !entitiesArrayValue.IsArray() )
                {
                    return Error( "Invalid format: 'Entities' value must be an array" );
                }

                outEntities.resize( entitiesArrayValue.Size() );

                for ( rapidjson::SizeType i = 0; i < entitiesArrayValue.Size(); i++ )
                {
                    if ( !entitiesArrayValue[i].IsObject() )
                    {
                        return Error( "Malformed map file, entities array can only contain objects" );
                    }

                    outEntities[i] = KRG::New<RawEntityData>();

                    if ( !ReadRawEntityData( entitiesArrayValue[i], globalEntityMap, globalComponentIDMap, outEntities[i] ) )
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        //-------------------------------------------------------------------------
        // Serializer
        //-------------------------------------------------------------------------

        ToolEntityCollectionReader::ToolEntityCollectionReader( TypeSystem::TypeRegistry const& typeRegistry )
            : m_typeRegistry( typeRegistry )
        {}

        bool ToolEntityCollectionReader::ReadCollection( FileSystemPath const& filePath, ToolEntityCollection& outCollection ) const
        {
            KRG_ASSERT( filePath.IsValid() );

            //-------------------------------------------------------------------------

            if ( !filePath.Exists() )
            {
                return Error( "Cant read source file %s", filePath.GetFullPath().c_str() );
            }

            //-------------------------------------------------------------------------

            Timer timer;
            Progress progress;
            outCollection.Reset();

            //-------------------------------------------------------------------------

            progress.m_stage = Stage::LoadFile;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }
            timer.Reset();

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

            progress.m_loadTime = timer.GetElapsedTimeMilliseconds();

            // Parse JSON
            //-------------------------------------------------------------------------

            progress.m_stage = Stage::ParseJSON;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }
            timer.Reset();

            rapidjson::Document mapDocument;
            rapidjson::ParseResult result = mapDocument.ParseInsitu( (char*) fileBuffer.data() );
            if ( result.IsError() )
            {
                return Error( "Failed to parse JSON: %s", rapidjson::GetParseError_En( result.Code() ) );
            }

            if ( !mapDocument.HasMember( "Entities" ) || !mapDocument["Entities"].IsArray() )
            {
                return Error( "Invalid format for map file, missing root entities array" );
            }

            progress.m_parseTime = timer.GetElapsedTimeMilliseconds();

            // Read raw data
            //-------------------------------------------------------------------------

            progress.m_stage = Stage::ReadRawData;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }
            timer.Reset();

            THashMap<UUID, RawEntityData*> rawEntityMap;
            THashMap<UUID, bool> rawComponentIDMap;
            TVector<RawEntityData*> rawEntities;

            auto clearRawEntityArray = [&rawEntities] ()
            {
                for ( auto pRawEntity : rawEntities )
                {
                    KRG::Delete( pRawEntity );
                }

                rawEntities.clear();
            };

            if ( !ReadRawEntityArray( mapDocument, rawEntityMap, rawComponentIDMap, rawEntities ) )
            {
                clearRawEntityArray();
                return false;
            }

            progress.m_readDataTime = timer.GetElapsedTimeMilliseconds();

            // Create collection
            //-------------------------------------------------------------------------

            progress.m_stage = Stage::CreateCollection;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }
            timer.Reset();

            if ( !CreateCollection( rawEntities, rawEntityMap, outCollection ) )
            {
                clearRawEntityArray();
                return false;
            }

            progress.m_createCollectionTime = timer.GetElapsedTimeMilliseconds();

            //-------------------------------------------------------------------------

            progress.m_stage = Stage::Complete;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }

            clearRawEntityArray();

            //-------------------------------------------------------------------------

            return true;
        }

        //-------------------------------------------------------------------------

        bool ToolEntityCollectionReader::SetToolEntityComponentData( RawComponentData const* pRawComponent, ToolEntityComponent& toolComponent ) const
        {
            KRG_ASSERT( pRawComponent != nullptr );

            // Set component data
            //-------------------------------------------------------------------------

            toolComponent.m_attachmentSocketID = pRawComponent->m_attachmentSocketID;

            for ( auto const& rawProperty : pRawComponent->m_propertyData )
            {
                auto pPropertyInstance = toolComponent.m_typeInstance.GetProperty( rawProperty.m_path, true );
                if ( pPropertyInstance != nullptr )
                {
                    pPropertyInstance->SetStringValue( rawProperty.m_value );
                }
                else
                {
                    Warning( "Cant find property (%s) on component (%s) of type (%s)", rawProperty.m_path.ToString().c_str(), pRawComponent->m_ID.ToString().c_str(), pRawComponent->m_typeID.GetAsStringID().c_str() );
                }
            }

            // Child Components
            //-------------------------------------------------------------------------

            for ( auto const& pRawChildComponent : pRawComponent->m_childComponents )
            {
                auto pChildComponentTypeInfo = m_typeRegistry.GetTypeInfo( pRawChildComponent->m_typeID );
                if ( pChildComponentTypeInfo == nullptr )
                {
                    return Error( "Error unknown type ID (%s) encountered for entity component: %s", pRawChildComponent->m_typeID.GetAsStringID().ToString(), pRawChildComponent->m_ID.ToString().c_str() );
                }

                if ( !pChildComponentTypeInfo->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntityComponent ) )
                {
                    return Error( "Encountered invalid type ID (%s) for entity component: %s, this type is not an entity component", pRawChildComponent->m_typeID.GetAsStringID().ToString(), pRawChildComponent->m_ID.ToString().c_str() );
                }

                auto& newChildToolComponent = toolComponent.CreateChildComponent( m_typeRegistry, pChildComponentTypeInfo, pRawChildComponent->m_ID, pRawChildComponent->m_name );
                SetToolEntityComponentData( pRawChildComponent, newChildToolComponent );
            }

            //-------------------------------------------------------------------------

            return true;
        }

        bool ToolEntityCollectionReader::SetToolEntityData( RawEntityData const* pRawEntity, ToolEntity& toolEntity ) const
        {
            KRG_ASSERT( pRawEntity != nullptr );

            // Set Entity Data
            //-------------------------------------------------------------------------

            toolEntity.m_attachmentSocketID = pRawEntity->m_attachmentSocketID;

            // Create Components
            //-------------------------------------------------------------------------

            toolEntity.m_components.reserve( pRawEntity->m_components.size() );

            for ( auto const& pRawComponentData : pRawEntity->m_components )
            {
                auto pComponentTypeInfo = m_typeRegistry.GetTypeInfo( pRawComponentData->m_typeID );
                if ( pComponentTypeInfo == nullptr )
                {
                    return Error( "Error unknown type ID (%s) encountered for entity component: %s", pRawComponentData->m_typeID.GetAsStringID().ToString(), pRawComponentData->m_ID.ToString().c_str() );
                }

                if ( !pComponentTypeInfo->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntityComponent ) )
                {
                    return Error( "Encountered invalid type ID (%s) for entity component: %s, this type is not an entity component", pRawComponentData->m_typeID.GetAsStringID().ToString(), pRawComponentData->m_ID.ToString().c_str() );
                }

                auto& newComponent = toolEntity.m_components.emplace_back( ToolEntityComponent( DynamicTypeInstance( m_typeRegistry, pComponentTypeInfo ), pRawComponentData->m_ID, pRawComponentData->m_name ) );
                SetToolEntityComponentData( pRawComponentData, newComponent );

                if ( newComponent.IsSpatialComponent() )
                {
                    if ( toolEntity.m_rootComponentID.IsValid() )
                    {
                        // Only one spatial component allowed per entity
                        return Error( "More than one spatial component found for entity: %s", pRawEntity->m_ID.ToString().c_str() );
                    }

                    toolEntity.m_isSpatialEntity = true;
                    toolEntity.m_rootComponentID = pRawComponentData->m_ID;
                }
            }

            // Create systems
            //-------------------------------------------------------------------------

            toolEntity.m_systems.reserve( pRawEntity->m_systems.size() );
            for ( auto const& rawSystemData : pRawEntity->m_systems )
            {
                auto pSystemTypeInfo = m_typeRegistry.GetTypeInfo( rawSystemData.m_typeID );
                if ( pSystemTypeInfo == nullptr )
                {
                    return Error( "Error unknown type ID encountered for entity system: %s", rawSystemData.m_typeID.GetAsStringID().ToString() );
                }

                if ( !pSystemTypeInfo->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntitySystem ) )
                {
                    return Error( "Encountered invalid type ID (%s) for entity system in entity (%s), this type is not an entity system", rawSystemData.m_typeID.GetAsStringID().ToString(), pRawEntity->m_name.ToString() );
                }

                toolEntity.m_systems.emplace_back( ToolEntitySystem( DynamicTypeInstance( m_typeRegistry, pSystemTypeInfo ) ) );
            }

            // Child Entities
            //-------------------------------------------------------------------------

            for ( auto const& pRawChildEntity : pRawEntity->m_childEntities )
            {
                auto& newChildEntity = toolEntity.CreateChildEntity( pRawChildEntity->m_ID, pRawChildEntity->m_name );
                SetToolEntityData( pRawChildEntity, newChildEntity );
            }

            //-------------------------------------------------------------------------

            return true;
        }

        bool ToolEntityCollectionReader::CreateCollection( TVector<RawEntityData*>& rawEntities, THashMap<UUID, RawEntityData*>& rawEntityMap, ToolEntityCollection& outCollection ) const
        {
            auto const pEntityTypeInfo = m_typeRegistry.GetTypeInfo( Entity::GetStaticTypeID() );
            if ( pEntityTypeInfo == nullptr )
            {
                return Error( "Cant find the type info for the core entity type" );
            }

            // Sort and validate spatial parents
            //-------------------------------------------------------------------------

            S32 const numEntities = (S32) rawEntities.size();
            outCollection.m_entities.reserve( numEntities );
            
            for ( auto const& pRawEntityData : rawEntities )
            {
                auto& newEntity = outCollection.m_entities.emplace_back( ToolEntity( m_typeRegistry, pEntityTypeInfo, pRawEntityData->m_ID, pRawEntityData->m_name ) );
                if ( !SetToolEntityData( pRawEntityData, newEntity ) )
                {
                    return false;
                }
            }

            return true;
        }
    }
}