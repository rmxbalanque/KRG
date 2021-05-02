#ifdef _WIN32
#pragma once

#include "Compilers/Animation/_Module/API.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "Engine/Animation/AnimationSyncTrack.h"
#include "System/Resource/ResourcePtr.h"
#include "System/TypeSystem/Serialization/ImmutableTypeCollection.h"
#include "System/Core/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets { class RawAnimation; }

    //-------------------------------------------------------------------------

    namespace Animation
    {
        class AnimationClip;

        //-------------------------------------------------------------------------

        struct KRG_RESOURCECOMPILERS_ANIMATION_API AnimationResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE DataPath                    m_animationDataPath;
            EXPOSE TResourcePtr<Skeleton>      m_pSkeleton = nullptr;
            EXPOSE String                      m_animationName; // Optional: if not set, will use the first animation in the file
        };

        //-------------------------------------------------------------------------

        class AnimationCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 5;

            struct AnimationEventData
            {
                TypeSystem::ImmutableTypeCollectionHeader       m_collectionHeader;
                TVector<TypeSystem::ImmutableTypeDescriptor>    m_eventTypeDescriptors;
                TInlineVector<SyncTrack::EventMarker, 10>       m_syncEventMarkers;
            };

        public:

            AnimationCompiler();

        private:

            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;

            void TransferAndCompressAnimationData( RawAssets::RawAnimation const& rawAnimData, AnimationClip& animClip ) const;

            void CreateEventsData( Resource::CompileContext const& ctx, AnimationEventData& outEventData ) const;
        };
    }
}

#endif