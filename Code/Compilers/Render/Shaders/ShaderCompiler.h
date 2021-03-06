#ifdef _WIN32
#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        struct ShaderResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE DataPath m_shaderDataPath;
        };

        //-------------------------------------------------------------------------

        class ShaderCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 1;

        public:

            ShaderCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        };
    }
}

#endif