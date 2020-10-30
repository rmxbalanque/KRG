#include "EntityCollectionWriter.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/Entity/Collections/EntityCollectionDescriptor.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace EntityModel
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

            static bool WriteComponent( rapidjson::PrettyWriter<Serialization::RapidJsonStringBuffer>& writer, TypeSystem::TypeRegistry const& typeRegistry, EntityComponentDescriptor const& componentDesc )
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
                writer.String( componentDesc.m_typeID.GetAsStringID().c_str() );

                for ( auto const& propertyDesc : componentDesc.m_propertyValues )
                {
                    writer.Key( propertyDesc.m_path.ToString().c_str() );
                    writer.String( propertyDesc.m_stringValue.c_str() );
                }

                writer.EndObject();

                //-------------------------------------------------------------------------

                writer.EndObject();
                return true;
            }

            static bool WriteSystem( rapidjson::PrettyWriter<Serialization::RapidJsonStringBuffer>& writer, TypeSystem::TypeRegistry const& typeRegistry, EntitySystemDescriptor const& systemDesc )
            {
                writer.StartObject();
                writer.Key( "TypeID" );
                writer.String( systemDesc.m_typeID.GetAsStringID().c_str() );
                writer.EndObject();
                return true;
            }

            static bool WriteEntity( rapidjson::PrettyWriter<Serialization::RapidJsonStringBuffer>& writer, TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc )
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
        }

        bool EntityCollectionWriter::WriteCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystemPath const& outFilePath, EntityCollectionDescriptor const& collection )
        {
            KRG_ASSERT( outFilePath.IsValid() );

            Serialization::RapidJsonStringBuffer stringBuffer;
            rapidjson::PrettyWriter<Serialization::RapidJsonStringBuffer> writer( stringBuffer );

            // Write collection to document
            //-------------------------------------------------------------------------

            writer.StartObject();
            writer.Key( "Entities" );
            writer.StartArray();

            for ( auto const& entity : collection.GetEntityDescriptors() )
            {
                if( !WriteEntity( writer, typeRegistry, entity ) )
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
    }
}