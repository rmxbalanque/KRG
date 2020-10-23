#pragma once

#include "System/Entity/_Module/API.h"
#include "System/Core/Update/UpdateStage.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SystemRegistry;
    class UpdateContext;
    class EntityComponent;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API IEntitySystem
    {
        KRG_REGISTER_TYPE;

        friend class Entity;

        static UpdatePriorityList const PriorityList;

    public:

        virtual ~IEntitySystem() {}

        #if KRG_DEBUG_INSTRUMENTATION
        virtual char const* GetName() const = 0;
        #endif

    protected:

        // Get the required update stages and priorities for this component
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() { return IEntitySystem::PriorityList; };

        // Component registration
        virtual void RegisterComponent( EntityComponent* pComponent ) = 0;
        virtual void UnregisterComponent( EntityComponent* pComponent ) = 0;

        // System Update
        virtual void Update( UpdateContext const& ctx ) = 0;
    };
}

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION

#define KRG_REGISTER_ENTITY_SYSTEM( Type ) \
        KRG_REGISTER_TYPE \
        static UpdatePriorityList const PriorityList;\
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() override { return Type::PriorityList; };\
        virtual char const* GetName() const override { return #Type; }\

#else

#define KRG_REGISTER_ENTITY_SYSTEM( Type ) \
        KRG_REGISTER_TYPE \
        static UpdatePriorityList const PriorityList;\
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() override { return Type::PriorityList; };\

#endif