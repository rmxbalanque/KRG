#pragma once

#include "Engine/Animation/_Module/API.h"
#include "AnimationComponent.h"
#include "System/Animation/AnimationData.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class KRG_ENGINE_ANIMATION_API SimpleAnimationComponent : public AnimationComponent
        {
            KRG_REGISTER_ENTITY_COMPONENT;

        public:

            virtual Skeleton const* GetSkeleton() const override final;
            virtual Pose const* GetPose() const override final { return m_pPose; }
            virtual void Update( UpdateContext const& ctx ) override final;

        protected:

            virtual void Initialize() override final;
            virtual void Shutdown() override final;

        private:

            EXPOSE TResourcePtr<AnimationData>              m_pAnimation;
            Pose*                                           m_pPose = nullptr;
            Percentage                                      m_animTime;
        };
    }
}