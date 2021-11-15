#pragma once

#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class PlayerComponent;
    class SpatialEntityComponent;
    class CameraComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API DefaultPlayerController : public IEntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( DefaultPlayerController, RequiresUpdate( UpdateStage::FrameStart ) );

    public:

        DefaultPlayerController();
        virtual ~DefaultPlayerController();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityUpdateContext const& ctx ) override;

    private:

        PlayerComponent*            m_pPlayerComponent = nullptr;
        SpatialEntityComponent*     m_pRootComponent = nullptr;
        CameraComponent*            m_pCameraComponent = nullptr;
        float                       m_currentMoveSpeed = 0;
    };
}