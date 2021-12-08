#pragma once

#include "Engine/Animation/_Module/API.h"
#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationComponent;
    class AnimatedMeshComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationSystem : public EntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( AnimationSystem, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        virtual ~AnimationSystem();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

    private:

        AnimationComponent*                     m_pAnimComponent = nullptr;
        TVector<AnimatedMeshComponent*>         m_meshComponents;
    };
}