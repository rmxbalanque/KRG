#pragma once

#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class MaterialCompiler : public Resource::Compiler
    {
        static const int32 s_version = 0;

    public:

        MaterialCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}