#include "AnimationGraphCompiler.h"
#include "Engine/Animation/Graph/AnimationGraph.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using namespace Graph;

    //-------------------------------------------------------------------------

    AnimationGraphCompiler::AnimationGraphCompiler()
        : Resource::Compiler( "GraphCompiler", s_version )
    {
        m_outputTypes.push_back( AnimationGraph::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult AnimationGraphCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        TVector<GraphNode::Settings const*> settings;
        auto pNodeSettings = KRG::New<AnimationClipNode::Settings>();
        pNodeSettings->m_nodeIdx = 0;
        pNodeSettings->m_nodeID = StringID( "Animation" );
        pNodeSettings->m_animationID = 0;
        pNodeSettings->m_allowLooping = true;
        pNodeSettings->m_shouldSampleRootMotion = true;

        settings.emplace_back( pNodeSettings );

        //-------------------------------------------------------------------------

        TypeSystem::TypeDescriptorCollection settingsTypeDescriptors;
        auto& desc = settingsTypeDescriptors.m_descriptors.emplace_back();
        TypeSystem::Serialization::CreateTypeDescriptorFromNativeType( ctx.m_typeRegistry, pNodeSettings->GetTypeID(), pNodeSettings, desc );

        //-------------------------------------------------------------------------

        AnimationGraph graph;
        graph.m_persistentNodes.emplace_back( 0 );
        graph.m_instanceNodeStartOffsets.emplace_back( 0 );
        graph.m_instanceRequiredMemory = sizeof( AnimationClipNode );
        graph.m_instanceRequiredAlignment = alignof( AnimationClipNode );
        graph.m_numControlParameters = 0;
        graph.m_rootNodeIdx = 0;

        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( s_version, AnimationGraph::GetStaticResourceTypeID() );
            archive << graph;
            archive << settingsTypeDescriptors;

            cereal::BinaryOutputArchive& settingsArchive = *archive.GetOutputArchive();
            for ( auto pSettings : settings )
            {
                pSettings->Save( settingsArchive );
                KRG::Delete( pSettings );
            }

            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}
