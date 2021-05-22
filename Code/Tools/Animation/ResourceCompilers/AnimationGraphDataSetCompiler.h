#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphDataSetCompiler final : public Resource::Compiler
    {
        static const int32 s_version = 1;

    public:

        AnimationGraphDataSetCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;
    };
}