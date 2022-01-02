#pragma once

#include "Engine/Animation/_Module/API.h"
#include "Engine/Core/Entity/EntityComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;

    namespace Physics { class Scene; }

    //-------------------------------------------------------------------------

    namespace Animation
    {
        class Pose;
        class Skeleton;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_ANIMATION_API AnimationComponent : public EntityComponent
        {
            KRG_REGISTER_ENTITY_COMPONENT( AnimationComponent );

        public:

            using EntityComponent::EntityComponent;

            // Get the skeleton this component animated
            virtual Skeleton const* GetSkeleton() const = 0;

            // Get the final pose for the last update
            virtual Pose const* GetPose() const = 0;

            // Get the root motion delta for the last update
            inline Transform const& GetRootMotionDelta() const { return m_rootMotionDelta; }

            // Should this component be updated by the animation system or is it manually updated by another system
            inline bool RequiresManualUpdate() const { return m_requiresManualUpdate; }

            // Run the pre-physics animation update
            virtual void PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform, Physics::Scene* pPhysicsScene ) = 0;

            // Run the post-physics animation update
            virtual void PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform, Physics::Scene* pPhysicsScene ) = 0;

        protected:

            Transform                                               m_rootMotionDelta = Transform::Identity;

        private:

            KRG_EXPOSE bool                                         m_requiresManualUpdate = false;
        };
    }
}