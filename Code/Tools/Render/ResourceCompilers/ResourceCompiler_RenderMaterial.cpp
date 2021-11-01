#include "ResourceCompiler_RenderMaterial.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderMaterial.h"
#include "Engine/Render/Material/RenderMaterial.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    MaterialCompiler::MaterialCompiler()
        : Resource::Compiler( "MaterialCompiler", s_version )
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

        // Create Material
        //-------------------------------------------------------------------------

        Material material;
        material.m_pAlbedoTexture = resourceDescriptor.m_albedoTexture;
        material.m_pMetalnessTexture = resourceDescriptor.m_metalnessTexture;
        material.m_pRoughnessTexture = resourceDescriptor.m_roughnessTexture;
        material.m_pNormalMapTexture = resourceDescriptor.m_normalMapTexture;
        material.m_pSpecularMapTexture = resourceDescriptor.m_specularMapTexture;
        material.m_metalness = Math::Clamp( resourceDescriptor.m_metalness, 0.0f, 1.0f );
        material.m_roughness = Math::Clamp( resourceDescriptor.m_roughness, 0.0f, 1.0f );
        material.m_specular = Math::Clamp( resourceDescriptor.m_specular, 0.0f, 1.0f );

        // Install dependencies
        //-------------------------------------------------------------------------

        Resource::ResourceHeader hdr( s_version, Material::GetStaticResourceTypeID() );
        hdr.m_installDependencies.push_back( material.m_pAlbedoTexture.GetResourceID() );
        
        if ( material.HasMetalnessTexture() )
        {
            hdr.m_installDependencies.push_back( material.m_pMetalnessTexture.GetResourceID() );
        }

        if ( material.HasRoughnessTexture() )
        {
            hdr.m_installDependencies.push_back( material.m_pRoughnessTexture.GetResourceID() );
        }

        if ( material.HasNormalMapTexture() )
        {
            hdr.m_installDependencies.push_back( material.m_pNormalMapTexture.GetResourceID() );
        }

        if ( material.HasSpecularMapTexture() )
        {
            hdr.m_installDependencies.push_back( material.m_pSpecularMapTexture.GetResourceID() );
        }

        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
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