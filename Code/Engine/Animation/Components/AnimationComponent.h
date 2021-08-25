#pragma once

#include "Engine/Animation/_Module/API.h"
#include "System/Entity/EntityComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;

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
            virtual Transform const& GetRootMotionDelta() const { return Transform::Identity; }

            // Run the animation update
            virtual void PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) = 0;
            virtual void PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) = 0;
        };
    }
}