#include "EntityMapCompiler.h"
#include "Tools/Entity/Serialization/EntityCollectionReader.h"
#include "System/Entity/Map/EntityMapDescriptor.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapCompiler::EntityMapCompiler()
        : Resource::Compiler( "EntityMapCompiler", VERSION )
    {
        m_outputTypes.push_back( EntityMapDescriptor::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult EntityMapCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        EntityMapDescriptor map;
        map.m_ID = UUID::GenerateID();

        //-------------------------------------------------------------------------
        // Read collection
        //-------------------------------------------------------------------------

        if ( !EntityCollectionReader::ReadCollection( ctx.m_typeRegistry, ctx.m_inputFilePath, map.m_collectionTemplate ) )
        {
            return Resource::CompilationResult::Failure;
        }

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( VERSION, EntityMapDescriptor::GetStaticResourceTypeID() ) << map;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}
