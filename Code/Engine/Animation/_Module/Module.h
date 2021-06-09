#pragma once
#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "Engine/Animation/ResourceLoaders/AnimationSkeletonLoader.h"
#include "Engine/Animation/ResourceLoaders/AnimationClipLoader.h"
#include "Engine/Animation/ResourceLoaders/AnimationGraphLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class KRG_ENGINE_ANIMATION_API EngineModule : public IModule
        {
            KRG_REGISTER_MODULE;

        public:

            virtual bool Initialize( ModuleContext& context ) override final;
            virtual void Shutdown( ModuleContext& context ) override final;

        private:

            SkeletonLoader                  m_skeletonLoader;
            AnimationClipLoader             m_animationClipLoader;
            AnimationGraphLoader            m_graphLoader;
            bool                            m_initialized = false;
        };
    }
}