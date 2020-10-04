#include "EntityMapCompiler.h"
#include "Tools/Entity/ToolEntityCollectionReader.h"
#include "Tools/Entity/ToolEntityCollection.h"
#include "System/Entity/Map/EntityMap.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Platform/Platform/Platform_Win32.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityMapCompiler::EntityMapCompiler()
        : Resource::Compiler( "EntityMapCompiler", VERSION )
    {
        m_outputTypes.push_back( EntityMap::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult EntityMapCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        //-------------------------------------------------------------------------
        // Read map file
        //-------------------------------------------------------------------------

        auto progressNotifier = [this] ( Serialization::ToolEntityCollectionReader::Progress const& progress )
        {
            switch ( progress.m_stage )
            {
                case Serialization::ToolEntityCollectionReader::Stage::LoadFile:
                break;

                case Serialization::ToolEntityCollectionReader::Stage::ParseJSON:
                {
                    Message( "File loaded in: %.2f ms", progress.m_loadTime.ToFloat() );
                }
                break;

                case Serialization::ToolEntityCollectionReader::Stage::ReadRawData:
                {
                    Message( "JSON parsed in: %.2f ms", progress.m_parseTime.ToFloat() );
                }
                break;

                case Serialization::ToolEntityCollectionReader::Stage::CreateCollection:
                {
                    Message( "Raw data read in: %.2f ms", progress.m_readDataTime.ToFloat() );
                }
                break;

                case Serialization::ToolEntityCollectionReader::Stage::Complete:
                {
                    Message( "Collection created in: %.2f ms", progress.m_createCollectionTime.ToFloat() );
                }
                break;
            }
        };

        Serialization::ToolEntityCollectionReader entityCollectionSerializer( ctx.m_typeRegistry );
        entityCollectionSerializer.SetProgressNotifier( progressNotifier );

        ToolEntityCollection entityCollection( ctx.m_typeRegistry );
        if ( !entityCollectionSerializer.ReadCollection( ctx.m_inputFilePath, entityCollection ) )
        {
            return Resource::CompilationResult::Failure;
        }

        //-------------------------------------------------------------------------
        // Reflect collection
        //-------------------------------------------------------------------------

        EntityMap map;
        map.m_ID = UUID::GenerateID();

        TVector<Byte> propertyDataBuffer;

        Timer timer;
        for ( auto const& toolEntity : entityCollection.GetEntities() )
        {
            KRG_ASSERT( toolEntity.IsValid() );

            if ( !ConvertEntity( ctx.m_typeRegistry, map.m_entityArchive, propertyDataBuffer, toolEntity, UUID() ) )
            {
                return CompilationFailed( ctx );
            }
        }

        auto const conversionTime = timer.GetElapsedTimeMilliseconds();
        Message( "Conversion time: %.2f ms", conversionTime.ToFloat() );
        timer.Reset();

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( VERSION, EntityMap::GetStaticResourceTypeID() ) << map;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}
