#pragma once

#include "Tools/Entity/ToolEntityCollection.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMapDescriptor;

    //-------------------------------------------------------------------------

    class EntityMapCompiler : public Resource::Compiler
    {
        static const S32 VERSION = 1;

    public:

        EntityMapCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}
