#pragma once

#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------
// Development Player System
//-------------------------------------------------------------------------
// A free cam player controller - used for debug and in editor viewports

namespace KRG
{
    class SpatialEntityComponent;
    class FreeLookCameraComponent;
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API DevelopmentPlayerController : public EntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( DevelopmentPlayerController, RequiresUpdate( UpdateStage::FrameStart ) );

    public:

        DevelopmentPlayerController();
        virtual ~DevelopmentPlayerController();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

    private:

        PlayerComponent*            m_pPlayerComponent = nullptr;
        SpatialEntityComponent*     m_pRootComponent = nullptr;
        FreeLookCameraComponent*    m_pCameraComponent = nullptr;
        float                       m_currentMoveSpeed = 0;
    };
}