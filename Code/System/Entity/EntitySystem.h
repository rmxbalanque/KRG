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

    public:

        virtual ~IEntitySystem() {}

        virtual TypeSystem::TypeInfo const* GetTypeInfo() const { return IEntitySystem::s_pTypeInfo; }
        virtual char const* GetName() const = 0;

    protected:

        // Get the required update stages and priorities for this component
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() = 0;

        // Component registration
        virtual void RegisterComponent( EntityComponent* pComponent ) = 0;
        virtual void UnregisterComponent( EntityComponent* pComponent ) = 0;

        // System Update
        virtual void Update( UpdateContext const& ctx ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_ENTITY_SYSTEM( Type, ... )\
        KRG_REGISTER_TYPE;\
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() override { static UpdatePriorityList const priorityList = UpdatePriorityList( __VA_ARGS__ ); return priorityList; };\
        virtual TypeSystem::TypeInfo const* GetTypeInfo() const override { return Type::s_pTypeInfo; }\
        virtual char const* GetName() const override { return #Type; }