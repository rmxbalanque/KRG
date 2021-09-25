#pragma once

#include "Engine/Core/_Module/API.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/Event.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class TaskSystem;
    class Entity;
    class EntityComponent;
    class EntityWorld;

    //-------------------------------------------------------------------------

    namespace EntityModel 
    {
        class EntityCollection;
        class EntityMap;
    
        //-------------------------------------------------------------------------

        struct LoadingContext
        {
            LoadingContext() = default;

            LoadingContext( TaskSystem* pTaskSystem, TypeSystem::TypeRegistry const* pTypeRegistry, Resource::ResourceSystem* pResourceSystem )
                : m_pTaskSystem( pTaskSystem )
                , m_pTypeRegistry( pTypeRegistry )
                , m_pResourceSystem( pResourceSystem )
            {}

            inline bool IsValid() const
            {
                return m_pTypeRegistry != nullptr && m_pResourceSystem != nullptr;
            }

        public:

            TaskSystem*                                                     m_pTaskSystem = nullptr;
            TypeSystem::TypeRegistry const*                                 m_pTypeRegistry = nullptr;
            Resource::ResourceSystem*                                       m_pResourceSystem = nullptr;
        };
    }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityComponent : public IRegisteredType
    {
        KRG_REGISTER_TYPE( EntityComponent );

        friend Entity;
        friend EntityModel::EntityCollection;
        friend EntityModel::EntityMap;
        friend EntityWorld;

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
        inline UUID GetEntityID() const { return m_entityID; }
        inline StringID GetName() const { return m_name; }

        // Status
        inline bool HasLoadingFailed() const { return m_status == Status::LoadingFailed; }
        inline bool IsUnloaded() const { return m_status == Status::Unloaded; }
        inline bool IsLoading() const { return m_status == Status::Loading; }
        inline bool IsLoaded() const { return m_status == Status::Loaded; }
        inline bool IsInitialized() const { return m_status == Status::Initialized; }
        inline Status GetStatus() const { return m_status; }

    protected:

        EntityComponent() = default;
        EntityComponent( UUID ID, StringID name ) : m_ID( ID ), m_name( name ) {}

        // Request load of all component data - loading takes time
        virtual void Load( EntityModel::LoadingContext const& context, UUID requesterID ) = 0;

        // Request unload of component data, unloading is instant
        virtual void Unload( EntityModel::LoadingContext const& context, UUID requesterID ) = 0;

        // Update loading state, this will check all dependencies
        virtual void UpdateLoading() = 0;

        // Called when an component finishes loading all its resources
        // Note: this is only called if the loading succeeds and you are guaranteed all resources to be valid and so should assert on that
        virtual void Initialize() { KRG_ASSERT( m_entityID.IsValid() && m_status == Status::Loaded ); m_status = Status::Initialized; }

        // Called just before a component begins unloading
        virtual void Shutdown() { KRG_ASSERT( m_entityID.IsValid() && m_status == Status::Initialized ); m_status = Status::Loaded; }

    protected:

        UUID                        m_ID;                               // The unique ID for this component
        UUID                        m_entityID;                         // The ID of the entity that contains this component
        StringID                    m_name;                             // The name of the component
        Status                      m_status = Status::Unloaded;        // Component status
        bool                        m_isRegisteredWithEntity = false;   // Registered with its parent entity's local systems
        bool                        m_isRegisteredWithWorld = false;    // Registered with the global systems in it's parent world
    };

    //-------------------------------------------------------------------------

    template <typename To, typename From>
    inline To const* ComponentCast( From const* pFromComponent )
    {
        static_assert( std::is_base_of<KRG::EntityComponent, From>::value, "Invalid component cast detected, you may only cast from classes derived from EntityComponent" );
        static_assert( std::is_base_of<KRG::EntityComponent, To>::value, "Invalid component cast detected, you may only cast to classes derived from EntityComponent" );

        To const* pCastComponent = nullptr;

        if ( pFromComponent != nullptr )
        {
            // Get TypeInfo for the From component
            auto pFromTypeInfo = pFromComponent->GetTypeInfo();
            KRG_ASSERT( pFromTypeInfo != nullptr );

            // Check up-cast
            if ( pFromTypeInfo->IsDerivedFrom( To::s_pTypeInfo->m_ID ) )
            {
                pCastComponent = static_cast<To const*>( pFromComponent );
            }
        }

        return pCastComponent;
    }

    template <typename To, typename From>
    KRG_FORCE_INLINE To* ComponentCast( From* pComponent )
    {
        To const* pConstCastComponent = ComponentCast<To>( const_cast<From const*>( pComponent ) );
        return const_cast<To*>( pConstCastComponent );
    }
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_ENTITY_COMPONENT( TypeName ) \
        KRG_REGISTER_TYPE( TypeName );\
        protected:\
        virtual void Load( EntityModel::LoadingContext const& context, UUID requesterID ) override;\
        virtual void Unload( EntityModel::LoadingContext const& context, UUID requesterID ) override;\
        virtual void UpdateLoading() override;