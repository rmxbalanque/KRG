#pragma once

#include "../_Module/API.h"
#include "System/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class AnimationComponent;
        class AnimatedMeshComponent;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_ANIMATION_API AnimationSystem : public IEntitySystem
        {
            KRG_REGISTER_ENTITY_SYSTEM( AnimationSystem, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

        public:

            virtual ~AnimationSystem();

        protected:

            virtual void RegisterComponent( EntityComponent* pComponent ) override;
            virtual void UnregisterComponent( EntityComponent* pComponent ) override;
            virtual void Update( UpdateContext const& ctx ) override;

        private:

            AnimationComponent*                     m_pAnimComponent = nullptr;
            TVector<AnimatedMeshComponent*>         m_meshComponents;
        };
    }
}