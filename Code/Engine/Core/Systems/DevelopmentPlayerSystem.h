#pragma once

#include "Engine/Core/Entity/EntitySystem.h"

//-------------------------------------------------------------------------
// Development Player System
//-------------------------------------------------------------------------
// A free cam player controller - used for debug and in editor viewports

namespace KRG
{
    class PlayerComponent;
    class SpatialEntityComponent;
    class FreeLookCameraComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API DevelopmentPlayerSystem : public IEntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( DevelopmentPlayerSystem, RequiresUpdate( UpdateStage::FrameStart ) );

    public:

        DevelopmentPlayerSystem();
        virtual ~DevelopmentPlayerSystem();

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