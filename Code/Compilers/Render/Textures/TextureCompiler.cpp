#ifdef _WIN32
#include "TextureCompiler.h"
#include "TextureConverter.h"
#include "System/Render/RenderTexture.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        TextureCompiler::TextureCompiler()
            : Resource::Compiler( "TextureCompiler", VERSION )
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
            if ( !ctx.ConvertDataPathToFilePath( resourceDescriptor.m_textureDataPath, textureFilePath ) )
            {
                return Error( "Invalid texture data path: %s", resourceDescriptor.m_textureDataPath.c_str() );
            }

            Texture texture;
            if ( !ConvertTexture( textureFilePath, texture.m_rawData ) )
            {
                return Error( "Failed to convert texture!" );
            }

            //-------------------------------------------------------------------------

            FileSystem::EnsurePathExists( ctx.m_outputFilePath );
            Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
            if ( archive.IsValid() )
            {
                Resource::ResourceHeader hdr( VERSION, resourceDescriptor.m_resourceTypeID );
                archive << hdr << texture;
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