#include "EntityCollectionDescriptorWriter.h"
#include "Tools/Core/ThirdParty/KRG_RapidJson.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"
#include "Engine/Core/Entity/Collections/EntityCollectionDescriptor.h"
#include "Engine/Core/Entity/Collections/EntityCollection.h"
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

            writer.Key( "ID" );
            writer.String( componentDesc.m_ID.ToString().c_str() );

            writer.Key( "Name" );
            writer.String( componentDesc.m_name.ToString() );

            if ( componentDesc.m_spatialParentID.IsValid() )
            {
                writer.Key( "SpatialParent" );
                writer.String( componentDesc.m_spatialParentID.ToString().c_str() );
            }

            if ( componentDesc.m_attachmentSocketID.IsValid() )
            {
                writer.Key( "AttachmentSocketID" );
                writer.String( componentDesc.m_attachmentSocketID.ToString() );
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

            writer.Key( "ID" );
            writer.String( entityDesc.m_ID.ToString().c_str() );

            writer.Key( "Name" );
            writer.String( entityDesc.m_name.ToString() );

            if ( entityDesc.m_attachmentSocketID.IsValid() )
            {
                writer.Key( "AttachmentSocketID" );
                writer.String( entityDesc.m_attachmentSocketID.ToString() );
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

        static void CreateDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityCollection const& collection, EntityCollectionDescriptor& outCollectionDesc )
        {
            auto CreateEntityDesc = [&typeRegistry] ( Entity* pEntity )
            {
                EntityDescriptor entityDesc;
                entityDesc.m_ID = pEntity->GetID();
                entityDesc.m_name = pEntity->GetName();

                if ( pEntity->HasSpatialParent() )
                {
                    entityDesc.m_spatialParentID = pEntity->GetSpatialParentID();
                    entityDesc.m_attachmentSocketID = pEntity->GetAttachmentSocketID();
                }

                // Components
                //-------------------------------------------------------------------------

                for ( auto pComponent : pEntity->GetComponents() )
                {
                    ComponentDescriptor componentDesc;
                    componentDesc.m_ID = pComponent->GetID();
                    componentDesc.m_name = pComponent->GetName();

                    // Spatial info
                    auto pSpatialEntityComponent = ComponentCast<SpatialEntityComponent>( pComponent );
                    if ( pSpatialEntityComponent != nullptr )
                    {
                        if ( pSpatialEntityComponent->HasSpatialParent() )
                        {
                            componentDesc.m_spatialParentID = pSpatialEntityComponent->GetSpatialParentID();
                            componentDesc.m_attachmentSocketID = pSpatialEntityComponent->GetAttachmentSocketID();
                        }

                        componentDesc.m_isSpatialComponent = true;
                    }

                    // Properties
                    TypeSystem::Serialization::CreateTypeDescriptorFromNativeType( typeRegistry, pComponent, componentDesc, true );

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

                return entityDesc;
            };

            //-------------------------------------------------------------------------

            outCollectionDesc.Clear();

            for ( auto pEntity : collection.GetEntities() )
            {
                EntityDescriptor entityDesc = CreateEntityDesc( pEntity );
                outCollectionDesc.AddEntity( entityDesc );
            }

            outCollectionDesc.GenerateSpatialAttachmentInfo();
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
        CreateDescriptor( typeRegistry, collection, ecd );
        return WriteCollection( typeRegistry, outFilePath, ecd );
    }
}