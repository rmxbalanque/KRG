#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/Entity/EntityIDs.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "Engine/Core/Update/UpdateStage.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------
// World Entity System
//-------------------------------------------------------------------------
// This is a global system that exists once per world and tracks/updates all components of certain types in the world!

namespace KRG
{
    class SystemRegistry;
    class EntityUpdateContext;
    class Entity;
    class EntityComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API IWorldEntitySystem : public IRegisteredType
    {
        KRG_REGISTER_TYPE( IWorldEntitySystem );

        friend class EntityWorld;

    public:

        virtual uint32 GetSystemID() const = 0;

    protected:

        // Get the required update stages and priorities for this component
        virtual UpdatePriorityList const& GetRequiredUpdatePriorities() = 0;

        // Called when the system is registered with the world - using explicit "EntitySystem" name to allow for a standalone initialize function
        virtual void InitializeSystem( SystemRegistry const& systemRegistry ) {};

        // Called when the system is removed from the world - using explicit "EntitySystem" name to allow for a standalone shutdown function
        virtual void ShutdownSystem() {};

        // System Update - using explicit "EntitySystem" name to allow for a standalone update functions
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) {};

        // Called whenever a new component is activated (i.e. added to the world)
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) = 0;

        // Called immediately before an component is deactivated
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_ENTITY_WORLD_SYSTEM( Type, ... )\
    constexpr static uint32 const s_entitySystemID = Hash::FNV1a::GetHash32( #Type );\
    virtual uint32 GetSystemID() const override final { return Type::s_entitySystemID; }\
    static UpdatePriorityList const PriorityList;\
    virtual UpdatePriorityList const& GetRequiredUpdatePriorities() override { static UpdatePriorityList const priorityList = UpdatePriorityList( __VA_ARGS__ ); return priorityList; };\
