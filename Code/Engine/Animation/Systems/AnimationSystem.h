#pragma once

#include "System/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class AnimationComponent;
        class AnimatedMeshComponent;

        //-------------------------------------------------------------------------

        class AnimationSystem : public IEntitySystem
        {
            KRG_REGISTER_ENTITY_SYSTEM( AnimationSystem );

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