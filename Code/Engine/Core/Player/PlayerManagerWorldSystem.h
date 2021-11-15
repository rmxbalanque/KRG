#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class PlayerComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API PlayerManagerSystem : public IWorldEntitySystem
    {
        struct RegisteredPlayer
        {
            RegisteredPlayer( UUID entityID, PlayerComponent* pComponent );

        public:

            UUID                                m_entityID;
            PlayerComponent*                    m_pComponent = nullptr;
        };

    public:

        KRG_REGISTER_TYPE( PlayerManagerSystem );
        KRG_ENTITY_WORLD_SYSTEM( PlayerManagerSystem, RequiresUpdate( UpdateStage::PrePhysics ) );

        #if KRG_DEVELOPMENT_TOOLS
        void SetPlayerControllerState( bool isEnabled );
        #endif

    private:

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) override;

    private:

        TVector<RegisteredPlayer>               m_players;
    };
} 