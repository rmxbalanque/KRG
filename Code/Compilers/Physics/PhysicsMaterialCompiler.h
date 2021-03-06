#ifdef _WIN32
#pragma  once

#include "_Module/API.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class PhysicsMaterialCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 0;

        public:

            PhysicsMaterialCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        };
    }
}

#endif