#pragma once

#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class TextureCompiler : public Resource::Compiler
    {
        static const int32 s_version = 1;

    public:

        TextureCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}