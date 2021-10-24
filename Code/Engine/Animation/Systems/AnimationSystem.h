#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "Engine/Animation/Components/AnimatedMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationSystem : public IEntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( AnimationSystem, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        virtual ~AnimationSystem();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

        inline Transform const& GetCharacterWorldTransform() const
        {
            if ( m_meshComponents.empty() )
            {
                return Transform::Identity;
            }
            else
            {
                return m_meshComponents[0]->GetWorldTransform();
            }
        }

    private:

        AnimationComponent*                     m_pAnimComponent = nullptr;
        TVector<AnimatedMeshComponent*>         m_meshComponents;
    };
}