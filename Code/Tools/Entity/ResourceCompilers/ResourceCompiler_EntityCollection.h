#pragma once

#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMapDescriptor;

    //-------------------------------------------------------------------------

    class EntityCollectionCompiler final : public Resource::Compiler
    {
        static const int32 s_version = 7;

    public:

        EntityCollectionCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    private:

        Resource::CompilationResult CompileCollection( Resource::CompileContext const& ctx ) const;
        Resource::CompilationResult CompileMap( Resource::CompileContext const& ctx ) const;
    };
}
