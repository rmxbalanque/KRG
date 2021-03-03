#include "PhysicsMaterialCompiler.h"
#include "Engine/Physics/Physx.h"
#include "Engine/Physics/PhysicsMaterial.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

#include <PxPhysicsAPI.h>

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        PhysicsMaterialCompiler::PhysicsMaterialCompiler()
            : Resource::Compiler( "PhysicsMaterialCompiler", VERSION )
        {
            m_outputTypes.push_back( PhysicsMaterial::GetStaticResourceTypeID() );
        }

        Resource::CompilationResult PhysicsMaterialCompiler::Compile( Resource::CompileContext const& ctx ) const
        {
            // Read material data
            //-------------------------------------------------------------------------

            PhysicsMaterialSettings materialSettings;

            TypeSystem::NativeTypeReader nativeTypeReader( ctx.m_typeRegistry );
            if ( nativeTypeReader.ReadFromFile( ctx.m_inputFilePath ) )
            {
                nativeTypeReader >> materialSettings;
            }
            else
            {
                KRG_LOG_ERROR( "ResourceCompiler", "Failed to read physics material file: %s", ctx.m_inputFilePath.c_str() );
                return CompilationFailed( ctx );
            }

            // Serialize
            //-------------------------------------------------------------------------

            FileSystem::EnsurePathExists( ctx.m_outputFilePath );
            Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
            if ( archive.IsValid() )
            {
                Resource::ResourceHeader hdr( VERSION, PhysicsMaterial::GetStaticResourceTypeID() );
                archive << hdr << materialSettings;
                return CompilationSucceeded( ctx );
            }
            else
            {
                return CompilationFailed( ctx );
            }
        }
    }
}
