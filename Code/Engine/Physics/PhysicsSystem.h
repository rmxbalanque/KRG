#pragma once

#include "_Module/API.h"
#include "System/Entity/EntityGlobalSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class PhysicsScene;
        class PhysicsGeometryComponent;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_PHYSICS_API PhysicsSystem : public ISystem, public IGlobalEntitySystem
        {
            friend class PhysicsDebugViewController;

            //-------------------------------------------------------------------------

            struct RegisteredGeometryComponent : public EntityRegistryRecord
            {
                PhysicsGeometryComponent*               m_pComponent = nullptr;
            };

        public:

            KRG_SYSTEM_ID( PhysicsSystem );
            KRG_ENTITY_GLOBAL_SYSTEM( PhysicsSystem );

        public:

            PhysicsSystem() = default;

        private:

            virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
            virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        protected:

            EntityRegistry<RegisteredGeometryComponent> m_geometryComponents;
        };
    }
}