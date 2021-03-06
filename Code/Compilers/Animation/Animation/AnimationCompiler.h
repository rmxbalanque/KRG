#ifdef _WIN32
#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets { class RawAnimation; }

    //-------------------------------------------------------------------------

    namespace Animation
    {
        class AnimationData;

        //-------------------------------------------------------------------------

        struct AnimationResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE DataPath                    m_animationDataPath;
            EXPOSE TResourcePtr<Skeleton>      m_pSkeleton = nullptr;
            EXPOSE String                      m_animationName; // Optional: if not set, will use the first animation in the file
        };

        //-------------------------------------------------------------------------

        class AnimationCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 4;

        public:

            AnimationCompiler();

        private:

            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;

            void TransferAndCompressAnimationData( RawAssets::RawAnimation const& rawAnimData, AnimationData& animData ) const;
        };
    }
}

#endif