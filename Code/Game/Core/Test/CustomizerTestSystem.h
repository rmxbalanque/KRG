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

    public:

        CustomizerTestSystem() = default;
        virtual ~CustomizerTestSystem();

    protected:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( UpdateContext const& ctx ) override;

    private:

        void SpawnCharacter( EntityModel::EntityMap* pMap, Transform const& characterTransform );
        void SpawnCharacterAttachments( Entity* pCharacter );

    private:

        CustomizerTestComponent*    m_pComponent = nullptr;

        TVector<Entity*>            m_spawnedEntities;
        bool                        charactersSpawned = false;
        bool                        attachmentsSpawned = false;
    };
}