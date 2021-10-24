#pragma once

#include "Engine/Core/_Module/API.h"
#include "System/Core/Update/UpdateStage.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/TypeSystem/ITypeHelper.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SystemRegistry;
    class EntityUpdateContext;
    class EntityComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API IEntitySystem : public IRegisteredType
    {
        KRG_REGISTER_TYPE( IEntitySystem );

        friend class Entity;

    public:

        virtual ~IEntitySystem() {}
        virtual char const* GetName() const = 0;

    protected:

        // Get the required update stages and priorities for this component
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() = 0;

        // Component registration
        virtual void RegisterComponent( EntityComponent* pComponent ) = 0;
        virtual void UnregisterComponent( EntityComponent* pComponent ) = 0;

        // System Update
        virtual void Update( EntityUpdateContext const& ctx ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_ENTITY_SYSTEM( TypeName, ... )\
        KRG_REGISTER_TYPE( TypeName );\
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() override { static UpdatePriorityList const priorityList = UpdatePriorityList( __VA_ARGS__ ); return priorityList; };\
        virtual char const* GetName() const override { return #TypeName; }