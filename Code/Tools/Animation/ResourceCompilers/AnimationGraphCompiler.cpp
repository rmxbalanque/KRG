#include "AnimationGraphCompiler.h"
#include "Engine/Animation/Graph/AnimationGraph.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphCompiler::AnimationGraphCompiler()
        : Resource::Compiler( "GraphCompiler", VERSION )
    {
        m_outputTypes.push_back( AnimationGraph::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult AnimationGraphCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        //-------------------------------------------------------------------------

        AnimationGraph graph;

        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( VERSION, AnimationGraph::GetStaticResourceTypeID() ) << graph;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}