#pragma once

#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"
#include "System/Render/RenderShader.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class ShaderCompiler : public Resource::Compiler
    {
        static const int32 s_version = 1;

    public:

        ShaderCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}