#pragma once

#include "System/Entity/_Module/API.h"
#include "EntitySpatialComponent.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Update/UpdateStage.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SystemRegistry;
    class IEntitySystem;
    class UpdateContext;

    namespace EntityModel
    {
        struct LoadingContext;
        class EntityMap;
        class EntityCollection;
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

    class KRG_SYSTEM_ENTITY_API Entity
    {
        KRG_REGISTER_TYPE;

        friend class EntityWorld;
        friend EntityModel::EntityMap;
        friend EntityModel::EntityCollection;

        using SystemUpdateList = TInlineVector<IEntitySystem*, 5>;

        // Create a spatial attachment between an entity and a new parent entity (socket is optional)
        static void CreateSpatialAttachment( Entity* pParentEntity, Entity* pChildEntity, StringID socketID = StringID::InvalidID );

        // Detach the specified entity from it's parent
        static void BreakSpatialAttachment( Entity* pChildEntity );

        // Remove all spatial attachments both from this entity to others and from others to this entity
        static void BreakAllEntitySpatialAttachments( Entity* pSpatialEntity );

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
        inline U32 GetNumComponents() const { return (U32) m_components.size(); }
        inline U32 GetNumSystems() const { return (U32) m_systems.size(); }

        // Spatial Info
        //-------------------------------------------------------------------------

        inline bool IsSpatialEntity() const { return m_pRootSpatialComponent != nullptr; }
        inline SpatialEntityComponent const* GetRootSpatialComponent() const { return m_pRootSpatialComponent; }
        inline UUID GetRootSpatialComponentID() const { return m_pRootSpatialComponent->GetID(); }
        inline Transform const& GetWorldTransform() const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetLocalTransform(); }
        inline void SetWorldTransform( Transform const& worldTransform ) const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->SetLocalTransform( worldTransform ); }
        inline OBB const& GetWorldBounds() const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetWorldBounds(); }
        inline bool HasSpatialParent() const { return m_pParentSpatialEntity != nullptr; }
        inline Entity const* GetSpatialParent() const { return m_pParentSpatialEntity; }
        inline Transform GetAttachmentSocketTransform( StringID socketID ) const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetAttachmentSocketTransform( socketID ); }

        // Status
        //-------------------------------------------------------------------------

        inline bool IsInCollection() const { return m_collectionID.IsValid(); }
        inline bool IsActivated() const { return m_status == Status::Activated; }
        inline bool IsDeactivated() const { return m_status != Status::Activated; }
        inline bool HasRequestedComponentLoad() const { return m_status != Status::Unloaded; }
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

        inline bool RequiresUpdate( UpdateStage stage ) const { return !m_systemUpdateLists[(S8) stage].empty(); }
        inline TVector<IEntitySystem*> const& GetSystems() const { return m_systems; }

        template<typename T> 
        inline void CreateSystem() 
        {
            static_assert( std::is_base_of<KRG::IEntitySystem, T>::value, "Invalid system type detected" );
            KRG_ASSERT( !VectorContains( m_systems, T::StaticTypeInfo->m_ID, [] ( IEntitySystem* pSystem, TypeSystem::TypeID systemTypeID ) { return pSystem->GetTypeInfo()->m_ID == systemTypeID; } ) );

            if ( IsUnloaded() )
            {
                CreateSystemImmediate( T::StaticTypeInfo );
            }
            else
            {
                auto& action = m_deferredActions.emplace_back( EntityInternalStateAction() );
                action.m_type = EntityInternalStateAction::Type::CreateSystem;
                action.m_ptr = T::StaticTypeInfo;

                // Send notification that the internal state changed
                EntityStateUpdatedEvent.Execute( this );
            }
        }

        template<typename T>
        inline void DestroySystem()
        {
            static_assert( std::is_base_of<KRG::IEntitySystem, T>::value, "Invalid system type detected" );
            KRG_ASSERT( VectorContains( m_systems, T::StaticTypeInfo->m_ID, [] ( IEntitySystem* pSystem, TypeSystem::TypeID systemTypeID ) { return pSystem->GetTypeInfo()->m_ID == systemTypeID; } ) );

            if ( IsUnloaded() )
            {
                DestroySystemImmediate( T::StaticTypeInfo );
            }
            else
            {
                auto& action = m_deferredActions.emplace_back( EntityInternalStateAction() );
                action.m_type = EntityInternalStateAction::Type::DestroySystem;
                action.m_ptr = T::StaticTypeInfo;

                // Send notification that the internal state changed
                EntityStateUpdatedEvent.Execute( this );
            }
        }

    protected:

        // Called when an entity finishes Loading successfully
        void Activate( EntityModel::LoadingContext const& loadingContext );

        // Called just before an entity fully unloads
        void Deactivate( EntityModel::LoadingContext const& loadingContext );

        // Update Entity Systems
        void UpdateSystems( UpdateContext const& context );

    private:

        // This function will search through the spatial hierarchy of this entity and return the first component it finds that contains a socket with the specified socket ID
        SpatialEntityComponent* FindSocketAttachmentComponent( SpatialEntityComponent* pComponentToSearch, StringID socketID ) const;
        inline SpatialEntityComponent* FindSocketAttachmentComponent( StringID socketID ) const { KRG_ASSERT( IsSpatialEntity() ); return FindSocketAttachmentComponent( m_pRootSpatialComponent, socketID ); }

        // Create the component-to-component attachment between this entity and the parent entity
        void AttachToParent();

        // Destroy the component-to-component attachment between this entity and the parent entity
        void DetachFromParent();

        // Update the attachment hierarchy, required when we have changes to this entity's spatial components or the spatial component hierarchy
        void RefreshEntityAttachments();

        //-------------------------------------------------------------------------

        void GenerateSystemUpdateList();

        void LoadComponents( EntityModel::LoadingContext const& loadingContext );
        void UnloadComponents( EntityModel::LoadingContext const& loadingContext );

        bool UpdateLoadingAndEntityState( EntityModel::LoadingContext const& loadingContext );

        // Immediate functions can be executed immediately for unloaded entities allowing us to skip the deferral of the operation
        void CreateSystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo );
        void CreateSystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo );
        void DestroySystemImmediate( TypeSystem::TypeInfo const* pSystemTypeInfo );
        void DestroySystemDeferred( EntityModel::LoadingContext const& loadingContext, TypeSystem::TypeInfo const* pSystemTypeInfo );

        // Immediate functions can be executed immediately for unloaded entities allowing us to skip the deferral of the operation
        void AddComponentImmediate( EntityComponent* pComponent, SpatialEntityComponent* pParentSpatialComponent );
        void AddComponentDeferred( EntityModel::LoadingContext const& loadingContext, EntityComponent* pComponent, SpatialEntityComponent* pParentSpatialComponent );
        void DestroyComponentImmediate( EntityComponent* pComponent );
        void DestroyComponentDeferred( EntityModel::LoadingContext const& loadingContext, EntityComponent* pComponent );

    protected:

        UUID                                            m_ID;                                   // The unique ID of this entity
        UUID                                            m_collectionID;                         // The ID of the collection that this entity is part of
        StringID                                        m_name;
        Status                                          m_status = Status::Unloaded;

        TVector<IEntitySystem*>                         m_systems;
        TVector<EntityComponent*>                       m_components;
        SystemUpdateList                                m_systemUpdateLists[(S8) UpdateStage::NumStages];

        SpatialEntityComponent*                         m_pRootSpatialComponent = nullptr;      // This spatial component defines our world position
        TVector<Entity*>                                m_attachedEntities;                     // The list of entities that are attached to this entity
        Entity*                                         m_pParentSpatialEntity = nullptr;       // The parent entity we are attached to
        StringID                                        m_parentAttachmentSocketID;             // The socket that we are attached to on the parent
        bool                                            m_isAttachedToParent = false;           // Has the actual entity-to-entity component attachment been created

        TVector<EntityInternalStateAction>              m_deferredActions;                      // The set of internal entity state changes that need to be executed
    };
 }