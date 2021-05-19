#pragma once

#include "Engine/Animation/_Module/API.h"
#include "AnimationComponent.h"
#include "Engine/Animation/Graph/AnimationGraph.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API AnimationGraphComponent : public AnimationComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AnimationGraphComponent );

    public:

        inline AnimationGraphComponent() = default;
        inline AnimationGraphComponent( StringID name ) : AnimationComponent( UUID::GenerateID(), name ) {}

        //-------------------------------------------------------------------------

        virtual void Update( UpdateContext const& ctx ) override final;

        //-------------------------------------------------------------------------

        virtual Skeleton const* GetSkeleton() const override final;
        virtual Pose const* GetPose() const override final { return m_pPose; }

    protected:

        virtual void Initialize() override final;
        virtual void Shutdown() override final;

    private:

        EXPOSE TResourcePtr<AnimationGraph>             m_pGraph = nullptr;
        EXPOSE TResourcePtr<AnimationGraphDataSet>      m_pDataSet = nullptr;

        Pose*                                           m_pPose = nullptr;

    };
}