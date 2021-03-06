#ifdef _WIN32
#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        struct TextureResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE DataPath m_textureDataPath;
        };

        //-------------------------------------------------------------------------

        class TextureCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 0;

        public:

            TextureCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        };
    }
}

#endif