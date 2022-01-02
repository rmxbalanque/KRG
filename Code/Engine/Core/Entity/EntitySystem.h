#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/Update/UpdateStage.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SystemRegistry;
    class EntityWorldUpdateContext;
    class EntityComponent;
    template<typename T> struct TEntityToolAccessor;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntitySystem : public IRegisteredType
    {
        KRG_REGISTER_TYPE( EntitySystem );

        friend class Entity;

    public:

        virtual ~EntitySystem() {}
        virtual char const* GetName() const = 0;

        // Called just before we register all the components with this system
        virtual void Initialize() {}

        // Called after all components have been registered with the system
        virtual void Activate() {}

        // Called before we start unregistering components from this system
        virtual void Deactivate() {}

        // Called after all components have been unregistered from the system
        virtual void Shutdown() {}

    protected:

        // Get the required update stages and priorities for this component
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() = 0;

        // Component registration
        virtual void RegisterComponent( EntityComponent* pComponent ) = 0;
        virtual void UnregisterComponent( EntityComponent* pComponent ) = 0;

        // System Update
        virtual void Update( EntityWorldUpdateContext const& ctx ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_ENTITY_SYSTEM( TypeName, ... )\
        KRG_REGISTER_TYPE( TypeName );\
        template<typename T> friend struct TEntityToolAccessor;\
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() override { static UpdatePriorityList const priorityList = UpdatePriorityList( __VA_ARGS__ ); return priorityList; };\
        virtual char const* GetName() const override { return #TypeName; }