#include "ResourceCompiler_RenderTexture.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"
#include "Tools/Render/TextureTools/TextureConverter.h"
#include "System/Render/RenderTexture.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    TextureCompiler::TextureCompiler()
        : Resource::Compiler( "TextureCompiler", s_version )
    {
        m_outputTypes.push_back( Texture::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult TextureCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        TextureResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        //-------------------------------------------------------------------------

        FileSystem::Path textureFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_texturePath, textureFilePath ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_texturePath.c_str() );
        }

        Texture texture;
        texture.m_format = TextureFormat::DDS;

        if ( !ConvertTexture( textureFilePath, texture.m_rawData ) )
        {
            return Error( "Failed to convert texture!" );
        }

        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, Texture::GetStaticResourceTypeID() );
            archive << hdr << texture;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}