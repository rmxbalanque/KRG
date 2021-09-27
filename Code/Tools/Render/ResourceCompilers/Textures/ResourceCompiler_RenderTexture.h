#pragma once

#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct TextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( TextureResourceDescriptor );

        KRG_EXPOSE DataPath m_textureDataPath;
    };

    //-------------------------------------------------------------------------

    class TextureCompiler : public Resource::Compiler
    {
        static const int32 s_version = 1;

    public:

        TextureCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}