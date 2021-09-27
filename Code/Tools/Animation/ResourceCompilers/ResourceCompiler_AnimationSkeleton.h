#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    struct KRG_TOOLS_ANIMATION_API SkeletonResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( SkeletonResourceDescriptor );

        KRG_EXPOSE DataPath         m_skeletonDataPath;

        // Optional value that specifies the name of the skeleton hierarchy to use, if it is unset, we use the first skeleton we find
        KRG_EXPOSE String           m_skeletonRootBoneName;
    };

    //-------------------------------------------------------------------------

    class SkeletonCompiler : public Resource::Compiler
    {
        static const int32 s_version = 0;

    public:

        SkeletonCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;
    };
}