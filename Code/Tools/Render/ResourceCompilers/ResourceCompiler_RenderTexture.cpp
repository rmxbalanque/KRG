#include "ResourceCompiler_RenderTexture.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"
#include "Tools/Render/TextureTools/TextureTools.h"
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
        m_outputTypes.push_back( CubemapTexture::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult TextureCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        if ( ctx.m_resourceID.GetResourceTypeID() == Texture::GetStaticResourceTypeID() )
        {
            return CompileTexture( ctx );
        }
        else if ( ctx.m_resourceID.GetResourceTypeID() == CubemapTexture::GetStaticResourceTypeID() )
        {
            return CompileCubemapTexture( ctx );
        }
        else
        {
            return Resource::CompilationResult::Failure;
        }
    }

    Resource::CompilationResult TextureCompiler::CompileTexture( Resource::CompileContext const& ctx ) const
    {
        TextureResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        //-------------------------------------------------------------------------

        FileSystem::Path textureFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_path, textureFilePath ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_path.c_str() );
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

    Resource::CompilationResult TextureCompiler::CompileCubemapTexture( Resource::CompileContext const& ctx ) const
    {
        CubemapTextureResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Cubemap textures ( +x, -x, +y, -y, +z, -z )
        //-------------------------------------------------------------------------

        TVector<FileSystem::Path> texturePaths;
        texturePaths.resize( 6 );

        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_texturePathPositiveX, texturePaths[0] ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_texturePathPositiveX.c_str() );
        }

        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_texturePathNegativeX, texturePaths[1] ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_texturePathNegativeX.c_str() );
        }

        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_texturePathPositiveY, texturePaths[2] ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_texturePathPositiveY.c_str() );
        }

        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_texturePathNegativeY, texturePaths[3] ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_texturePathNegativeY.c_str() );
        }

        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_texturePathPositiveZ, texturePaths[4] ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_texturePathPositiveZ.c_str() );
        }

        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_texturePathNegativeZ, texturePaths[5] ) )
        {
            return Error( "Invalid texture data path: %s", resourceDescriptor.m_texturePathNegativeZ.c_str() );
        }

        // Create cubemap
        //-------------------------------------------------------------------------

        CubemapTexture texture;
        texture.m_format = TextureFormat::DDS;

        if ( !CreateCubemapTexture( texturePaths, texture.m_rawData ) )
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