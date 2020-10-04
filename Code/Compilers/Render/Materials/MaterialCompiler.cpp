#ifdef _WIN32
#include "MaterialCompiler.h"
#include "System/Render/RenderMaterial.h"
#include "System/Render/RenderTexture.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        bool MaterialResourceDescriptor::IsValid()
        {
            return m_diffuseTexture.GetResourceID().IsValid();
        }

        //-------------------------------------------------------------------------

        MaterialCompiler::MaterialCompiler()
            : Resource::Compiler( "MaterialCompiler", VERSION )
        {
            m_outputTypes.push_back( Material::GetStaticResourceTypeID() );
        }

        Resource::CompilationResult MaterialCompiler::Compile( Resource::CompileContext const& ctx ) const
        {
            MaterialResourceDescriptor resourceDescriptor;
            if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
            {
                return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
            }

            if ( !resourceDescriptor.IsValid() )
            {
                return Error( "Incomplete or invalid material descriptor" );
            }

            Resource::ResourceHeader hdr( VERSION, resourceDescriptor.m_resourceTypeID );
            Material material;
            material.m_pDiffuseTexture = resourceDescriptor.m_diffuseTexture;
            hdr.m_installDependencies.push_back( material.m_pDiffuseTexture.GetResourceID() );

            // Serialize
            //-------------------------------------------------------------------------

            FileSystem::EnsurePathExists( ctx.m_outputFilePath );
            Serialization::BinaryArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
            if ( archive.IsValid() )
            {
                archive << hdr << material;
                return CompilationSucceeded( ctx );
            }
            else
            {
                return CompilationFailed( ctx );
            }
        }
    }
}
#endif