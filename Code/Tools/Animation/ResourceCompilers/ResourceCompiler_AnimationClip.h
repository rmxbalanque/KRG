#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"
#include "Engine/Animation/AnimationSyncTrack.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets { class RawAnimation; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClip;

    //-------------------------------------------------------------------------

    class AnimationClipCompiler : public Resource::Compiler
    {
        static const int32 s_version = 20;

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

        bool ReadEventsData( Resource::CompileContext const& ctx, rapidjson::Document const& document, RawAssets::RawAnimation const& rawAnimData, AnimationEventData& outEventData ) const;
    };
}