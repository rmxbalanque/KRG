#pragma once

#include "Engine/Core/Entity/EntitySystem.h"
#include "Engine/Core/Entity/Map/EntityMap.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class CustomizerTestComponent;

    //-------------------------------------------------------------------------

    class CustomizerTestSystem : public IEntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( CustomizerTestSystem, RequiresUpdate( UpdateStage::FrameStart ) );

        struct CustomizedCharacter
        {
            Entity*         m_pCharacter = nullptr;
            UUID            m_meshComponentID;
            UUID            m_armorComponentID;
            UUID            m_hairComponentID;
            EngineTimer     m_cooldownTimer;
            Milliseconds    m_cooldown;
            bool            m_createSystem = false;
        };

    public:

        CustomizerTestSystem() = default;
        virtual ~CustomizerTestSystem();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( UpdateContext const& ctx ) override;

    private:

        void SpawnCharacter( EntityModel::EntityMap* pMap, Transform const& characterTransform );
        void RecustomizeCharacter( CustomizedCharacter& character );

    private:

        CustomizerTestComponent*        m_pComponent = nullptr;

        TVector<CustomizedCharacter>    m_spawnedEntities;
        bool                            charactersSpawned = false;
        bool                            attach = false;
    };
}