#pragma once

#include "Engine/Animation/_Module/API.h"
#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class SkeletalMeshComponent;
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;
    class AnimationClipPlayerComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationSystem : public EntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( AnimationSystem, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        virtual ~AnimationSystem();

    private:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityWorldUpdateContext const& ctx ) override;

        void UpdateAnimPlayers( EntityWorldUpdateContext const& ctx, Transform const& characterWorldTransform );
        void UpdateAnimGraphs( EntityWorldUpdateContext const& ctx, Transform const& characterWorldTransform );

    private:

        TVector<AnimationClipPlayerComponent*>          m_animPlayers;
        TVector<AnimationGraphComponent*>               m_animGraphs;
        TVector<Render::SkeletalMeshComponent*>         m_meshComponents;
    };
}