#pragma once

#include "AnimationComponent.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"
#include "Engine/Animation/Graph/AnimationGraphInstance.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API AnimationGraphComponent final : public AnimationComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AnimationGraphComponent );

    public:

        inline AnimationGraphComponent() = default;
        inline AnimationGraphComponent( StringID name ) : AnimationComponent( UUID::GenerateID(), name ) {}

        //-------------------------------------------------------------------------

        virtual Skeleton const* GetSkeleton() const override;
        virtual Pose const* GetPose() const override { return m_pPose; }
        virtual void PrePhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) override;
        virtual void PostPhysicsUpdate( Seconds deltaTime, Transform const& characterTransform ) override;

        //-------------------------------------------------------------------------

        // This function will change the graph and data-set used! Note: this can only be called for unloaded components
        void SetGraphVariation( ResourceID graphResourceID );

    protected:

        virtual void Initialize() override;
        virtual void Shutdown() override;

    private:

        EXPOSE TResourcePtr<AnimationGraphVariation>    m_pGraphVariation = nullptr;

        Graph::GraphInstance*                           m_pGraphInstance = nullptr;
        Graph::TaskSystem*                              m_pTaskSystem = nullptr;
        Graph::GraphContext                             m_graphContext;
        Pose*                                           m_pPose = nullptr;
    };
}