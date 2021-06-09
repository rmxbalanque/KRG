#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "Engine/Animation/AnimationSyncTrack.h"
#include "System/Resource/ResourcePtr.h"
#include "System/TypeSystem/TypeDescriptors.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets { class RawAnimation; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClip;

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_ANIMATION_API AnimationClipResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( AnimationClipResourceDescriptor );

        EXPOSE DataPath                    m_animationDataPath;
        EXPOSE TResourcePtr<Skeleton>      m_pSkeleton = nullptr;
        EXPOSE DataPath                    m_animationEventData; // Optional: not all animations have events
        EXPOSE String                      m_animationName; // Optional: if not set, will use the first animation in the file
    };

    //-------------------------------------------------------------------------

    class AnimationClipCompiler : public Resource::Compiler
    {
        static const int32 s_version = 7;

        struct AnimationEventData
        {
            TypeSystem::TypeDescriptorCollection            m_collection;
            TInlineVector<SyncTrack::EventMarker, 10>       m_syncEventMarkers;
        };

    public:

        AnimationClipCompiler();

    private:

        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;

        void TransferAndCompressAnimationData( RawAssets::RawAnimation const& rawAnimData, AnimationClip& animClip ) const;

        void SetRootMotionData( RawAssets::RawAnimation const& rawAnimData, AnimationClip& animClip ) const;

        bool CreateEventsData( Resource::CompileContext const& ctx, AnimationClipResourceDescriptor const& animResourceDesc, RawAssets::RawAnimation const& rawAnimData, AnimationEventData& outEventData ) const;
    };
}