#pragma once

#include "API.h"
#include "Tools/Animation/ResourceCompilers/AnimationSkeletonCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationClipCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationGraphCompiler.h"
#include "Tools/Core/Resource/Compilers/ResourceCompilerRegistry.h"

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
        }

        void UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.UnregisterCompiler( &m_skeletonCompiler );
            compilerRegistry.UnregisterCompiler( &m_animationClipCompiler );
            compilerRegistry.UnregisterCompiler( &m_graphCompiler );
        }

    private:

        SkeletonCompiler                m_skeletonCompiler;
        AnimationClipCompiler           m_animationClipCompiler;
        AnimationGraphCompiler          m_graphCompiler;
    };
}
