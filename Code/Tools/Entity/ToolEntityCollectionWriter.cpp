#include "ToolEntityCollectionWriter.h"
#include "ToolEntityCollection.h"

#include "Tools/Core/TypeSystem/DynamicTypeInstance.h"
#include "Tools/Core/TypeSystem/DynamicTypeSerialization.h"
#include "System/Entity/Entity.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/error/en.h"
#include "System/Core/Time/Timers.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
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

        ToolEntityCollectionWriter::ToolEntityCollectionWriter( TypeSystem::TypeRegistry const& typeRegistry )
            : m_typeRegistry( typeRegistry )
        {}

        bool ToolEntityCollectionWriter::WriteCollection( ToolEntityCollection const& collection, FileSystemPath const& outFilePath ) const
        {
            KRG_ASSERT( outFilePath.IsValid() );

            Timer timer;
            Progress progress;
            RapidJsonStringBuffer stringBuffer;
            rapidjson::PrettyWriter<RapidJsonStringBuffer> writer( stringBuffer );

            // Write collection to document
            //-------------------------------------------------------------------------

            progress.m_stage = Stage::GenerateJSON;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }
            timer.Reset();

            writer.StartObject();
            writer.Key( "Entities" );
            writer.StartArray();

            for ( auto const& entity : collection.GetEntities() )
            {
                if( !WriteEntity( writer, entity ) )
                {
                    return false;
                }
            }

            writer.EndArray();
            writer.EndObject();

            progress.m_GenerateJSONTime = timer.GetElapsedTimeMilliseconds();

            // Write to disk
            //-------------------------------------------------------------------------
            
            progress.m_stage = Stage::WriteFile;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }
            timer.Reset();

            FileSystem::EnsurePathExists( outFilePath );

            FILE* fp = fopen( outFilePath.c_str(), "w" );
            if ( fp == nullptr )
            {
                return Error( "Failed to open files: %s for writing.", outFilePath.c_str() );
            }

            fwrite( stringBuffer.GetString(), sizeof( char ), stringBuffer.GetSize(), fp );
            fclose( fp );

            progress.m_writeTime = timer.GetElapsedTimeMilliseconds();

            //-------------------------------------------------------------------------

            progress.m_stage = Stage::Complete;
            if ( m_progressUpdateNotifier != nullptr )
            {
                m_progressUpdateNotifier( progress );
            }

            return true;
        }

        //-------------------------------------------------------------------------

        bool ToolEntityCollectionWriter::WritePropertyInstance( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, TypeSystem::PropertyPath& currentPath, DynamicPropertyInstance const& propertyInstance ) const
        {
            if ( propertyInstance.IsDefaultValue() )
            {
                return true;
            }

            //-------------------------------------------------------------------------

            if ( !propertyInstance.IsArrayElementProperty() )
            {
                currentPath.Append( propertyInstance.GetID() );
            }

            //-------------------------------------------------------------------------

            if ( propertyInstance.IsArrayProperty() )
            {
                S32 const numElements = propertyInstance.GetNumArrayElements();
                for ( auto i = 0; i < numElements; i++ )
                {
                    auto const& arrayElement = propertyInstance.GetArrayElement( i );
                    currentPath.ReplaceLastElement( currentPath.GetLastElement().m_propertyID, i );
                    WritePropertyInstance( writer, currentPath, arrayElement );
                    currentPath.ReplaceLastElement( currentPath.GetLastElement().m_propertyID );
                }
            }
            else if ( propertyInstance.IsStructureProperty() )
            {
                auto const& structureProperties = propertyInstance.GetProperties();
                S32 const numProperties = propertyInstance.GetNumProperties();
                for ( auto i = 0; i < numProperties; i++ )
                {
                    WritePropertyInstance( writer, currentPath, structureProperties[i] );
                }
            }
            else
            {
                writer.Key( currentPath.ToString().c_str() );
                writer.String( propertyInstance.GetStringValue().c_str() );
            }

            //-------------------------------------------------------------------------

            if ( !propertyInstance.IsArrayElementProperty() )
            {
                currentPath.RemoveLastElement();
            }

            return true;
        }

        bool ToolEntityCollectionWriter::WriteTypeInstance( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, TypeSystem::PropertyPath& currentPath, DynamicTypeInstance const& typeInstance ) const
        {
            writer.Key( "TypeData" );
            writer.StartObject();

            writer.Key( "TypeID" );
            writer.String( typeInstance.GetTypeID().GetAsStringID().c_str() );

            for ( auto const& propertyInstance : typeInstance.GetProperties() )
            {
                WritePropertyInstance( writer, currentPath, propertyInstance );
            }

            writer.EndObject();
            return true;
        }

        bool ToolEntityCollectionWriter::WriteComponent( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, ToolEntityComponent const& component ) const
        {
            writer.StartObject();

            writer.Key( "ID" );
            writer.String( component.m_ID.ToString().c_str() );

            writer.Key( "Name" );
            writer.String( component.m_name.ToString() );

            if ( component.m_attachmentSocketID.IsValid() )
            {
                writer.Key( "AttachmentSocketID" );
                writer.String( component.m_attachmentSocketID.ToString() );
            }

            //-------------------------------------------------------------------------

            TypeSystem::PropertyPath propertyPath;
            if ( !WriteTypeInstance( writer, propertyPath, component.m_typeInstance ) )
            {
                return false;
            }

            //-------------------------------------------------------------------------

            if ( !component.m_childComponents.empty() )
            {
                writer.Key( "Components" );
                writer.StartArray();
                for ( auto const& childComponent : component.m_childComponents )
                {
                    if ( !WriteComponent( writer, childComponent ) )
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

        bool ToolEntityCollectionWriter::WriteSystem( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, ToolEntitySystem const& system ) const
        {
            writer.StartObject();

            writer.Key( "TypeID" );
            writer.String( system.m_typeInstance.GetTypeID().GetAsStringID().c_str() );

            writer.EndObject();
            return true;
        }

        bool ToolEntityCollectionWriter::WriteEntity( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, ToolEntity const& entity ) const
        {
            writer.StartObject();

            writer.Key( "ID" );
            writer.String( entity.m_ID.ToString().c_str() );
            
            writer.Key( "Name" );
            writer.String( entity.m_name.ToString() );

            if ( entity.m_attachmentSocketID.IsValid() )
            {
                writer.Key( "AttachmentSocketID" );
                writer.String( entity.m_attachmentSocketID.ToString() );
            }

            //-------------------------------------------------------------------------

            if ( !entity.m_components.empty() )
            {
                writer.Key( "Components" );
                writer.StartArray();
                for ( auto const& component : entity.m_components )
                {
                    if ( !WriteComponent( writer, component ) )
                    {
                        return false;
                    }
                }
                writer.EndArray();
            }

            //-------------------------------------------------------------------------

            if ( !entity.m_systems.empty() )
            {
                writer.Key( "Systems" );
                writer.StartArray();
                for ( auto const& system : entity.m_systems )
                {
                    if ( !WriteSystem( writer, system ) )
                    {
                        return false;
                    }
                }
                writer.EndArray();
            }

            //-------------------------------------------------------------------------

            if ( !entity.m_childEntities.empty() )
            {
                writer.Key( "Entities" );
                writer.StartArray();
                for ( auto const& childEntity : entity.m_childEntities )
                {
                    if ( !WriteEntity( writer, childEntity ) )
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
}