#if _WIN32
#include "Module.h"
#include "Compilers/Animation/Skeleton/AnimSkeletonCompiler.h"
#include "Compilers/Animation/Animation/AnimationCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        ResourceCompilerModule::ResourceCompilerModule()
            : KRG::ResourceCompilerModule( "Resource.Animation" )
        {
            AddCompiler( KRG::New<SkeletonCompiler>() );
            AddCompiler( KRG::New<AnimationCompiler>() );
        }
    }
}

#endif