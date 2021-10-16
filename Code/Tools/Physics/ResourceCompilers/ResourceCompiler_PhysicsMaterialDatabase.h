#pragma  once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsMaterialDatabaseCompiler : public Resource::Compiler
    {
        static const int32 s_version = 0;

    public:

        PhysicsMaterialDatabaseCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}