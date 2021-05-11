#pragma once

#include "API.h"
#include "Tools/Animation/ResourceCompilers/SkeletonCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationCompiler.h"
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
            compilerRegistry.RegisterCompiler( &m_animationCompiler );
        }

        void UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.UnregisterCompiler( &m_skeletonCompiler );
            compilerRegistry.UnregisterCompiler( &m_animationCompiler );
        }

    private:

        SkeletonCompiler        m_skeletonCompiler;
        AnimationCompiler       m_animationCompiler;
    };
}
