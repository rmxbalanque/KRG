#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class EditorGraphDefinition;
    class EditorGraphCompilationContext;

    //-------------------------------------------------------------------------

    class AnimationGraphCompiler final : public Resource::Compiler
    {
        static const int32 s_version = 2;

    public:

        AnimationGraphCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;

    private:

        Resource::CompilationResult CompileDefinition( Resource::CompileContext const& ctx ) const;
        Resource::CompilationResult CompileVariation( Resource::CompileContext const& ctx ) const;
        bool GenerateVirtualDataSetResource( Resource::CompileContext const& ctx, EditorGraphDefinition const& editorGraph, EditorGraphCompilationContext const& compilationContext, StringID const& variationID, ResourcePath const& dataSetPath ) const;
    };
}