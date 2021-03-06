#pragma once

#include "Tools/Entity/ToolEntityCollection.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMapDescriptor;

    //-------------------------------------------------------------------------

    class EntityMapCompiler final : public Resource::Compiler
    {
        static const int32 VERSION = 1;

    public:

        EntityMapCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}
