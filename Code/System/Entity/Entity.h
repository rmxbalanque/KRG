#pragma once

#include "System/Entity/_Module/API.h"
#include "EntitySpatialComponent.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Update/UpdateStage.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SystemRegistry;
    class IEntitySystem;
    class UpdateContext;
    class Entity;

    namespace EntityModel
    {
        struct LoadingContext;
        class EntityMap;
        class EntityCollection;

        //-------------------------------------------------------------------------

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

    //-------------------------------------------------------------------------
    // Entity
    //-------------------------------------------------------------------------
    // This is container for a set of components and systems
    //
    //  * Owns and is responsible for its systems and components and their memory
    //
    //  * Any runtime changes to an entity's internal state (components/systems) 
    //  * will be deferred to the next entity world load phase to ensure that we
    //  * dont interfere with any current operations for this frame
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API Entity : public IRegisteredType
    {
        KRG_REGISTER_TYPE( Entity );

        friend class EntityWorld;
        friend EntityModel::EntityMap;
        friend EntityModel::EntityCollection;

        using SystemUpdateList = TInlineVector<IEntitySystem*, 5>;

        // Entity internal state change actions
        struct EntityInternalStateAction
        {
            enum class Type
            {
                Unknown,
                CreateSystem,
                DestroySystem,
                AddComponent,
                DestroyComponent,
                WaitForComponentUnregistration,
            };

            void const*                         m_ptr = nullptr;            // Can either be ptr to a system or a component
            UUID                                m_ID;                       // Contains the ID of the parent spatial component
            Type                                m_type = Type::Unknown;     // Type of action
        };

        // Event that's fired whenever a component/system is added or removed
        static TMultiUserEventInternal<Entity*> EntityStateUpdatedEvent;

    public:

        enum class Status
        {
            Unloaded = 0,
            Loaded,
            Activated,
        };

        static TMultiUserEvent<Entity*> OnEntityStateUpdated() { return EntityStateUpdatedEvent; }

    public:

        Entity() = default;
        Entity( StringID name ) : m_ID( UUID::GenerateID() ), m_name( name ) {}
        ~Entity();

        // Entity Info
        //-------------------------------------------------------------------------

        inline UUID GetID() const { return m_ID; }
        inline StringID GetName() const { return m_name; }
        inline UUID GetCollectionID() const { return m_collectionID; }
        inline uint32 GetNumComponents() const { return (uint32) m_components.size(); }
        inline uint32 GetNumSystems() const { return (uint32) m_systems.size(); }

        // Spatial Info
        //-------------------------------------------------------------------------

        inline bool IsSpatialEntity() const { return m_pRootSpatialComponent != nullptr; }
        inline SpatialEntityComponent const* GetRootSpatialComponent() const { return m_pRootSpatialComponent; }
        inline UUID GetRootSpatialComponentID() const { return m_pRootSpatialComponent->GetID(); }
        inline Transform const& GetWorldTransform() const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetLocalTransform(); }
        inline void SetWorldTransform( Transform const& worldTransform ) const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->SetLocalTransform( worldTransform ); }
        inline OBB const& GetWorldBounds() const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetWorldBounds(); }
        inline bool HasSpatialParent() const { return m_pParentSpatialEntity != nullptr; }
        inline bool HasAttachedEntities() const { return !m_attachedEntities.empty(); }
        inline Transform GetAttachmentSocketTransform( StringID socketID ) const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetAttachmentSocketTransform( socketID ); }

        // Status
        //-------------------------------------------------------------------------

        inline bool IsInCollection() const { return m_collectionID.IsValid(); }
        inline bool IsActivated() const { return m_status == Status::Activated; }
        inline bool HasRequestedComponentLoad() const { return m_status != Status::Unloaded; }
        inline bool IsLoaded() const { return m_status == Status::Loaded; }
        inline bool IsUnloaded() const { return m_status == Status::Unloaded; }

        // Components
        //-------------------------------------------------------------------------
        // NB!!! Add and remove operations execute immediately for unloaded entities BUT will be deferred to the next loading phase for loaded entities

        inline TVector<EntityComponent*> const& GetComponents() const { return m_components; }

        // Add a new component. For spatial component, you can optionally specify a component to attach to. 
        // If this is unset, the component will be attached to the root component (or will become the root component if one doesnt exist)
        void AddComponent( EntityComponent* pComponent, UUID const& parentSpatialComponentID = UUID() );

        // Destroys a component on this entity
        void DestroyComponent( UUID const& componentID );

        // Systems
        //-------------------------------------------------------------------------
        // NB!!! Add and remove operations execute immediately for unloaded entities BUT will be deferred to the next loading phase for loaded entities

        inline TVector<IEntitySystem*> const& GetSystems() const { return m_systems; }

        template<typename T>
        T* GetSystem()
        {
            static_assert( std::is_base_of<IEntitySystem, T>::value, "T has to derive from IEntitySystem" );
            for ( auto pSystem : m_systems )
            {
                if ( pSystem->GetTypeInfo()->m_ID == T::GetStaticTypeID() )
                {
                    return static_cast<T*>( pSystem );
                }
            }

            return nullptr;
        }

        template<typename T> 
        inline void CreateSystem() 
        {
            static_assert( std::is_base_of<KRG::IEntitySystem, T>::value, "Invalid system type detected" );
            KRG_ASSERT( !VectorContains( m_systems, T::s_pTypeInfo->m_ID, [] ( IEntitySystem* pSystem, TypeSystem::TypeID systemTypeID ) { return pSystem->GetTypeInfo()->m_ID == systemTypeID; } ) );

            if ( IsUnloaded() )
            {
                CreateSystemImmediate( T::s_pTypeInfo );
            }
            else
            {
                Threading::ScopeLock lock( m_internalStateMutex );

                auto& action = m_deferredActions.emplace_back( EntityInternalStateAction() );
                action.m_type = EntityInternalStateAction::Type::CreateSystem;
                action.m_ptr = T::s_pTypeInfo;

                // Send notification that the internal state changed
                EntityStateUpdatedEvent.Execute( this );
            }
        }

        template<typename T>
        inline void DestroySystem()
        {
            static_assert( std::is_base_of<KRG::IEntitySystem, T>::value, "Invalid system type detected" );
            KRG_ASSERT( VectorContains( m_systems, T::s_pTypeInfo->m_ID, [] ( IEntitySystem* pSystem, TypeSystem::TypeID systemTypeID ) { return pSystem->GetTypeInfo()->m_ID == systemTypeID; } ) );

            if ( IsUnloaded() )
            {
                DestroySystemImmediate( T::s_pTypeInfo );
            }
            else
            {
                Threading::ScopeLock lock( m_internalStateMutex );

                auto& action = m_deferredActions.emplace_back( EntityInternalStateAction() );
                action.m_type = EntityInternalStateAction::Type::DestroySystem;
                action.m_ptr = T::s_pTypeInfo;

                // Send notification that the internal state changed
                EntityStateUpdatedEvent.Execute( this );
            }
        }

    private:

        // This function will search through the spatial hierarchy of this entity and return the first component it finds that contains a socket with the specified socket ID
        SpatialEntityComponent* FindSocketAttachmentComponent( SpatialEntityComponent* pComponentToSearch, StringID socketID ) const;
        inline SpatialEntityComponent* FindSocketAttachmentComponent( StringID socketID ) const { KRG_ASSERT( IsSpatialEntity() ); return FindSocketAttachmentComponent( m_pRootSpatialComponent, socketID ); }

        // Set attachment info - This will set the ptr to the parent entity and add this entity to the parent entity's attached entity list
        void SetSpatialParent( Entity* pParentEntity, StringID socketID );

        // Clear attachment info - Assumes spatial attachment has not been created
        void ClearSpatialParent();

        // Return the spatial parent if set
        inline Entity const* GetSpatialParent() const { return m_pParentSpatialEntity; }

        // Create the component-to-component attachment between this entity and the parent entity
        void CreateSpatialAttachment();

        // Destroy the component-to-component attachment between this entity and the parent entity
        void DestroySpatialAttachment();

        // Update the attachment hierarchy, required when we have made changes to this entity's spatial components or the spatial component hierarchy
        void RefreshChildSpatialAttachments();

        // Removes an internal component from the current hierarchy while awaiting destruction
        void RemoveComponentFromSpatialHierarchy( SpatialEntityComponent* pComponent );

        //-------------------------------------------------------------------------

        // Generate the per-stage update lists for this entity
        void GenerateSystemUpdateList();

        // Run Entity Systems
        void UpdateSystems( UpdateContext const& context );

        // Registers a component with all the local entity systems
        void RegisterComponentWithLocalSystems( EntityComponent* pComponent );

        // Unregister a component from all the local entity systems
        void UnregisterComponentFromLocalSystems( EntityComponent* pComponent );

        //-------------------------------------------------------------------------

        // Update internal entity state and execute all deferred actions
        bool UpdateEntityState( EntityModel::LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );

        // Request initial load of all components
        void LoadComponents( EntityModel::LoadingContext const& loadingContext );

        // Request final unload of all components
        void UnloadComponents( EntityModel::LoadingContext const& loadingContext );

        // Called when an entity finishes Loading successfully - Registers components with system, creates spatial attachments. Will attempt to activate all attached entities
        void Activate( EntityModel::LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );

        // Called just before an entity fully unloads - Unregisters components from systems, breaks spatial attachments. Will attempt to deactivate all attached entities
        void Deactivate( EntityModel::LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );

        // Immediate functions can be executed immediately for unloaded entities allowing us to skip the deferral of the operation
        void CreateSystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo );
        void DestroySystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo );
        void AddComponentImmediate( EntityComponent* pComponent, SpatialEntityComponent* pParentSpatialComponent );
        void DestroyComponentImmediate( EntityComponent* pComponent );

        // Deferred functions are execute as part of the internal state update and will execute the immediate version as well as perform additional operations ( e.g. load/unload/etc. )
        void CreateSystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo );
        void DestroySystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo );
        void AddComponentDeferred( EntityModel::LoadingContext const& loadingContext, EntityComponent* pComponent, SpatialEntityComponent* pParentSpatialComponent );
        void DestroyComponentDeferred( EntityModel::LoadingContext const& loadingContext, EntityComponent* pComponent );

    protected:

        UUID                                            m_ID;                                   // The unique ID of this entity
        UUID                                            m_collectionID;                         // The ID of the collection/map that this entity is part of
        StringID                                        m_name;
        Status                                          m_status = Status::Unloaded;
        bool                                            m_isRegisteredForUpdates = false;       // Is this entity registered for frame updates

        TVector<IEntitySystem*>                         m_systems;
        TVector<EntityComponent*>                       m_components;
        SystemUpdateList                                m_systemUpdateLists[(int8) UpdateStage::NumStages];

        SpatialEntityComponent*                         m_pRootSpatialComponent = nullptr;      // This spatial component defines our world position
        TVector<Entity*>                                m_attachedEntities;                     // The list of entities that are attached to this entity
        Entity*                                         m_pParentSpatialEntity = nullptr;       // The parent entity we are attached to
        StringID                                        m_parentAttachmentSocketID;             // The socket that we are attached to on the parent
        bool                                            m_isSpatialAttachmentCreated = false;   // Has the actual component-to-component attachment been created

        TVector<EntityInternalStateAction>              m_deferredActions;                      // The set of internal entity state changes that need to be executed
        Threading::Mutex                                m_internalStateMutex;                   // A mutex that needs to be lock due to internal state changes
    };
 }