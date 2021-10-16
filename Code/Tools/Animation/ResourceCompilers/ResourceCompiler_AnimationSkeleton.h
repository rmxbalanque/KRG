#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class SkeletonCompiler : public Resource::Compiler
    {
        static const int32 s_version = 1;

    public:

        SkeletonCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;
    };
}