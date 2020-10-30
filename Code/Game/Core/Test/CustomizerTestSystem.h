#pragma once

#include "System/Entity/EntitySystem.h"
#include "System/Entity/Map/EntityMap.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class CustomizerTestComponent;

    //-------------------------------------------------------------------------

    class CustomizerTestSystem : public IEntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( CustomizerTestSystem );

        struct CustomizedCharacter
        {
            Entity*         m_pCharacter = nullptr;
            UUID            m_meshComponentID;
            UUID            m_armorComponentID;
            UUID            m_hairComponentID;
            Milliseconds    m_lastCustomizedTime;
            Milliseconds    m_cooldown;
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
    };
}