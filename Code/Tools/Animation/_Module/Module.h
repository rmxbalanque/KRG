#pragma once

#include "API.h"
#include "Tools/Animation/ResourceCompilers/AnimationSkeletonCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationClipCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationGraphCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationGraphDataSetCompiler.h"
#include "Tools/Resource/Compilers/ResourceCompilerRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_TOOLS_ANIMATION_API ToolsModule
    {
        KRG_REGISTER_MODULE;

    public:

        void RegisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.RegisterCompiler( &m_skeletonCompiler );
            compilerRegistry.RegisterCompiler( &m_animationClipCompiler );
            compilerRegistry.RegisterCompiler( &m_graphCompiler );
            compilerRegistry.RegisterCompiler( &m_graphDataSetCompiler );
        }

        void UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.UnregisterCompiler( &m_skeletonCompiler );
            compilerRegistry.UnregisterCompiler( &m_animationClipCompiler );
            compilerRegistry.UnregisterCompiler( &m_graphCompiler );
            compilerRegistry.UnregisterCompiler( &m_graphDataSetCompiler );
        }

    private:

        SkeletonCompiler                m_skeletonCompiler;
        AnimationClipCompiler           m_animationClipCompiler;
        AnimationGraphCompiler          m_graphCompiler;
        AnimationGraphDataSetCompiler   m_graphDataSetCompiler;
    };
}
