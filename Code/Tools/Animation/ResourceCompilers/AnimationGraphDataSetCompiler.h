#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphDataSetCompiler final : public Resource::Compiler
    {
        static const int32 VERSION = 0;

    public:

        AnimationGraphDataSetCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;
    };
}