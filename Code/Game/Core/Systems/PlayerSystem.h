#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class PlayerComponent;
    class SpatialEntityComponent;
    class OrbitCameraComponent;
    namespace Physics { class CapsuleComponent; }

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API PlayerSystem : public IEntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( PlayerSystem, RequiresUpdate( UpdateStage::FrameStart ) );

    public:

        PlayerSystem();
        virtual ~PlayerSystem();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

    private:

        PlayerComponent*                        m_pPlayerComponent = nullptr;
        Physics::CapsuleComponent*              m_pCapsuleComponent = nullptr;
        SpatialEntityComponent*                 m_pRootComponent = nullptr;
        OrbitCameraComponent*                   m_pCameraComponent = nullptr;
        float                                   m_currentMoveSpeed = 0;
    };
}