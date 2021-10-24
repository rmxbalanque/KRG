#pragma once

#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SpatialEntityComponent;
    class CameraComponent;

    //-------------------------------------------------------------------------

    class DefaultPlayerController : public IEntitySystem
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

        SpatialEntityComponent*     m_pRootComponent = nullptr;
        CameraComponent*            m_pCameraComponent = nullptr;
        float                       m_currentMoveSpeed = 0;
    };
}