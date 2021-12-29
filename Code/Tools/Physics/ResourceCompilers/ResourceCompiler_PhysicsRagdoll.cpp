#include "ResourceCompiler_PhysicsRagdoll.h"
#include "Tools/Physics/ResourceDescriptors/ResourceDescriptor_PhysicsRagdoll.h"
#include "Engine/Physics/PhysicsRagdoll.h"
#include "System/TypeSystem/TypeSerialization.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    RagdollCompiler::RagdollCompiler()
        : Resource::Compiler( "PhysicsRagdollCompiler", s_version )
    {
        m_outputTypes.push_back( RagdollDefinition::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult RagdollCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        RagdollResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        if ( !resourceDescriptor.m_definition.IsValid() )
        {
            return Error( "Invalid ragdoll definition: %s", ctx.m_inputFilePath.c_str() );
        }

        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, RagdollDefinition::GetStaticResourceTypeID() );
            hdr.AddInstallDependency( resourceDescriptor.m_definition.m_pSkeleton.GetResourceID() );
            archive << hdr << resourceDescriptor.m_definition;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}