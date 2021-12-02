#include "EntityCollectionDescriptorWriter.h"
#include "Tools/Core/ThirdParty/KRG_RapidJson.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"
#include "Engine/Core/Entity/EntityCollectionDescriptor.h"
#include "Engine/Core/Entity/EntityCollection.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"
#include "Engine/Core/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    namespace
    {
        static bool Error( char const* pFormat, ... )
        {
            va_list args;
            va_start( args, pFormat );
            Log::AddEntryVarArgs( Log::Severity::Error, "Entity Collection Writer", __FILE__, __LINE__, pFormat, args );
            va_end( args );
            return false;
        }

        //-------------------------------------------------------------------------

        static bool WriteComponent( RapidJsonWriter& writer, TypeSystem::TypeRegistry const& typeRegistry, ComponentDescriptor const& componentDesc )
        {
            writer.StartObject();

            writer.Key( "Name" );
            writer.String( componentDesc.m_name.c_str() );

            if ( componentDesc.m_spatialParentName.IsValid() )
            {
                writer.Key( "SpatialParent" );
                writer.String( componentDesc.m_spatialParentName.c_str() );
            }

            if ( componentDesc.m_attachmentSocketID.IsValid() )
            {
                writer.Key( "AttachmentSocketID" );
                writer.String( componentDesc.m_attachmentSocketID.c_str() );
            }

            //-------------------------------------------------------------------------

            writer.Key( "TypeData" );
            writer.StartObject();

            writer.Key( "TypeID" );
            writer.String( componentDesc.m_typeID.c_str() );

            for ( auto const& propertyDesc : componentDesc.m_properties )
            {
                writer.Key( propertyDesc.m_path.ToString().c_str() );
                writer.String( propertyDesc.m_stringValue.c_str() );
            }

            writer.EndObject();

            //-------------------------------------------------------------------------

            writer.EndObject();
            return true;
        }

        static bool WriteSystem( RapidJsonWriter& writer, TypeSystem::TypeRegistry const& typeRegistry, SystemDescriptor const& systemDesc )
        {
            writer.StartObject();
            writer.Key( "TypeID" );
            writer.String( systemDesc.m_typeID.c_str() );
            writer.EndObject();
            return true;
        }

        static bool WriteEntity( RapidJsonWriter& writer, TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc )
        {
            writer.StartObject();

            writer.Key( "Name" );
            writer.String( entityDesc.m_name.c_str() );

            if ( entityDesc.m_attachmentSocketID.IsValid() )
            {
                writer.Key( "AttachmentSocketID" );
                writer.String( entityDesc.m_attachmentSocketID.c_str() );
            }

            //-------------------------------------------------------------------------

            if ( !entityDesc.m_components.empty() )
            {
                writer.Key( "Components" );
                writer.StartArray();
                for ( auto const& component : entityDesc.m_components )
                {
                    if ( !WriteComponent( writer, typeRegistry, component ) )
                    {
                        return false;
                    }
                }
                writer.EndArray();
            }

            //-------------------------------------------------------------------------

            if ( !entityDesc.m_systems.empty() )
            {
                writer.Key( "Systems" );
                writer.StartArray();
                for ( auto const& system : entityDesc.m_systems )
                {
                    if ( !WriteSystem( writer, typeRegistry, system ) )
                    {
                        return false;
                    }
                }
                writer.EndArray();
            }

            //-------------------------------------------------------------------------

            writer.EndObject();
            return true;
        }

        //-------------------------------------------------------------------------

        static bool CreateDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityCollection const& collection, EntityCollectionDescriptor& outCollectionDesc )
        {
            TVector<StringID> entityNameList;
            TVector<StringID> entityComponentList;

            //-------------------------------------------------------------------------

            auto CreateEntityDesc = [&typeRegistry, &collection, &entityNameList, &entityComponentList] ( Entity* pEntity, EntityDescriptor& entityDesc )
            {
                KRG_ASSERT( !entityDesc.IsValid() );
                entityDesc.m_name = pEntity->GetName();

                // Check for unique names
                if ( VectorContains( entityNameList, entityDesc.m_name ) )
                {
                    return false;  // Duplicate name detected!!
                }
                else
                {
                    entityNameList.emplace_back( entityDesc.m_name );
                }

                // Get spatial parent
                if ( pEntity->HasSpatialParent() )
                {
                    Entity const* pSpatialParentEntity = collection.FindEntity( pEntity->GetSpatialParentID() );
                    entityDesc.m_spatialParentName = pSpatialParentEntity->GetName();
                    entityDesc.m_attachmentSocketID = pEntity->GetAttachmentSocketID();
                }

                // Components
                //-------------------------------------------------------------------------

                entityComponentList.clear();

                for ( auto pComponent : pEntity->GetComponents() )
                {
                    ComponentDescriptor componentDesc;
                    componentDesc.m_name = pComponent->GetName();

                    // Check for unique names
                    if ( VectorContains( entityComponentList, componentDesc.m_name ) )
                    {
                        // Duplicate name detected!!
                        return false;
                    }
                    else
                    {
                        entityComponentList.emplace_back( componentDesc.m_name );
                    }

                    // Spatial info
                    auto pSpatialEntityComponent = TryCast<SpatialEntityComponent>( pComponent );
                    if ( pSpatialEntityComponent != nullptr )
                    {
                        if ( pSpatialEntityComponent->HasSpatialParent() )
                        {
                            EntityComponent const* pSpatialParentComponent = pEntity->FindComponent( pSpatialEntityComponent->GetSpatialParentID() );
                            componentDesc.m_spatialParentName = pSpatialParentComponent->GetName();
                            componentDesc.m_attachmentSocketID = pSpatialEntityComponent->GetAttachmentSocketID();
                        }

                        componentDesc.m_isSpatialComponent = true;
                    }

                    // Type descriptor - Properties
                    componentDesc.DescribeTypeInstance( typeRegistry, pComponent, true );

                    // Add component
                    entityDesc.m_components.emplace_back( componentDesc );
                    if ( componentDesc.m_isSpatialComponent )
                    {
                        entityDesc.m_numSpatialComponents++;
                    }
                }

                // Systems
                //-------------------------------------------------------------------------

                for ( auto pSystem : pEntity->GetSystems() )
                {
                    SystemDescriptor systemDesc;
                    entityDesc.m_systems.emplace_back( systemDesc );
                }

                return true;
            };

            //-------------------------------------------------------------------------

            outCollectionDesc.Clear();

            for ( auto pEntity : collection.GetEntities() )
            {
                EntityDescriptor entityDesc;
                if ( !CreateEntityDesc( pEntity, entityDesc ) )
                {
                    return false;
                }
                outCollectionDesc.AddEntity( entityDesc );
            }

            outCollectionDesc.GenerateSpatialAttachmentInfo();

            return true;
        }
    }

    //-------------------------------------------------------------------------

    bool EntityCollectionDescriptorWriter::WriteCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& outFilePath, EntityCollectionDescriptor const& collection )
    {
        KRG_ASSERT( outFilePath.IsValid() );

        RapidJsonStringBuffer stringBuffer;
        RapidJsonWriter writer( stringBuffer );

        // Write collection to document
        //-------------------------------------------------------------------------

        writer.StartObject();
        writer.Key( "Entities" );
        writer.StartArray();

        for ( auto const& entity : collection.GetEntityDescriptors() )
        {
            if ( !WriteEntity( writer, typeRegistry, entity ) )
            {
                return false;
            }
        }

        writer.EndArray();
        writer.EndObject();

        // Write to disk
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( outFilePath );

        FILE* fp = fopen( outFilePath.c_str(), "w" );
        if ( fp == nullptr )
        {
            return Error( "Failed to open files: %s for writing.", outFilePath.c_str() );
        }

        fwrite( stringBuffer.GetString(), sizeof( char ), stringBuffer.GetSize(), fp );
        fclose( fp );
        return true;
    }

    bool EntityCollectionDescriptorWriter::WriteCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& outFilePath, EntityCollection const& collection )
    {
        EntityCollectionDescriptor ecd;
        if ( !CreateDescriptor( typeRegistry, collection, ecd ) )
        {
            return false;
        }

        return WriteCollection( typeRegistry, outFilePath, ecd );
    }
}