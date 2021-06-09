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

            // Get the final pose
            virtual Pose const* GetPose() const = 0;

            // Run the animation update
            virtual void PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) = 0;
            virtual void PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) = 0;
        };
    }
}