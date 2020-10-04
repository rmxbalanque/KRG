#pragma once
#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "System/Animation/ResourceLoaders/AnimationSkeletonLoader.h"
#include "System/Animation/ResourceLoaders/AnimationLoader.h"

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

            SkeletonLoader           m_skeletonLoader;
            AnimationLoader          m_animationLoader;
            bool                     m_initialized = false;
        };
    }
}