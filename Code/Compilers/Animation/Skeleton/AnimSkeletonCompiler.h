#if _WIN32
#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        struct SkeletonResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE DataPath         m_skeletonDataPath;

            // Optional value that specifies the name of the skeleton hierarchy to use, if it is unset, we use the first skeleton we find
            EXPOSE String           m_skeletonRootBoneName;
        };

        //-------------------------------------------------------------------------

        class SkeletonCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 0;

        public:

            SkeletonCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;
        };
    }
}

#endif
