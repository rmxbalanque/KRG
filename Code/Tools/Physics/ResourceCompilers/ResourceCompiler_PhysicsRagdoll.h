#pragma  once

#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class RagdollCompiler : public Resource::Compiler
    {
        static const int32 s_version = 1;

    public:

        RagdollCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}