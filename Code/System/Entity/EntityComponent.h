#pragma once

#include "System/Entity/_Module/API.h"
#include "System/Core/Types/UUID.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct EntityLoadingContext
    {
        EntityLoadingContext() = default;

        EntityLoadingContext( TypeSystem::TypeRegistry const* pTypeRegistry, Resource::ResourceSystem* pResourceSystem )
            : m_pTypeRegistry( pTypeRegistry )
            , m_pResourceSystem( pResourceSystem )
        {}

        inline bool IsValid() const
        {
            return m_pTypeRegistry != nullptr && m_pResourceSystem != nullptr;
        }

    public:

        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityComponent
    {
        KRG_REGISTER_TYPE;

        friend class Entity;
        friend class EntityCollection;

    public:

        enum class Status
        {
            Unloaded = 0,
            Loading,
            Loaded,
            LoadingFailed,
            Initialized,
        };

    public:

        virtual ~EntityComponent();

        inline UUID GetID() const { return m_ID; }
        inline StringID GetName() const { return m_name; }
        inline TypeSystem::TypeID GetTypeID() const { return GetTypeInfo()->m_ID; }
        virtual TypeSystem::TypeInfo const* GetTypeInfo() const { return EntityComponent::TypeInfoPtr; }

        // Status
        inline bool HasLoadingFailed() const { return m_status == Status::LoadingFailed; }
        inline bool IsUnloaded() const { return m_status == Status::Unloaded; }
        inline bool IsLoading() const { return m_status == Status::Loading; }
        inline bool IsLoaded() const { return m_status == Status::Loaded; }
        inline bool IsInitialized() const { return m_status == Status::Initialized; }
        inline Status GetStatus() const { return m_status; }

    protected:

        EntityComponent() = default;

        // Request load of all component data - loading takes time
        virtual void Load( EntityLoadingContext const& context, UUID requesterID ) = 0;

        // Request unload of component data, unloading is instant
        virtual void Unload( EntityLoadingContext const& context, UUID requesterID ) = 0;

        // Update loading state, this will check all dependencies
        virtual void UpdateLoading() = 0;

        // Called when an component finishes loading all its resources
        // Note: this is only called if the loading succeeds and you are guaranteed all resources to be valid and so should assert on that
        virtual void Initialize() {}

        // Called just before a component begins unloading
        virtual void Shutdown() {}

    protected:

        UUID                        m_ID;
        StringID                    m_name;
        Status                      m_status = Status::Unloaded;
    };
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_ENTITY_COMPONENT \
        KRG_REGISTER_TYPE \
        public:\
        virtual TypeSystem::TypeInfo const* GetTypeInfo() const override;\
        protected:\
        virtual void Load( EntityLoadingContext const& context, UUID requesterID ) override;\
        virtual void Unload( EntityLoadingContext const& context, UUID requesterID ) override;\
        virtual void UpdateLoading() override;