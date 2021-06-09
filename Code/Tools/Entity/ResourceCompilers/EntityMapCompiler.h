#pragma once

#include "Tools/Entity/EntityCollectionModel.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMapDescriptor;

    //-------------------------------------------------------------------------

    class EntityMapCompiler final : public Resource::Compiler
    {
        static const int32 s_version = 1;

    public:

        EntityMapCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}
