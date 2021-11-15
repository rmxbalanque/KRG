#pragma once
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    class EntityComponent;
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct ActivationContext
    {
        ActivationContext() = default;

    public:

        // World system registration
        Threading::LockFreeQueue<TPair<Entity*, EntityComponent*>>  m_componentsToRegister;
        Threading::LockFreeQueue<TPair<Entity*, EntityComponent*>>  m_componentsToUnregister;

        // Entity update registration
        Threading::LockFreeQueue<Entity*>                           m_registerForEntityUpdate;
        Threading::LockFreeQueue<Entity*>                           m_unregisterForEntityUpdate;
    };
}