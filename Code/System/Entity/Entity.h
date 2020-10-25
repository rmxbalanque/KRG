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

    public:

        enum class Status
        {
            Deactivated = 0,
            Activated,
        };

    public:

        Entity() = default;
        ~Entity();

        // Entity Info
        inline UUID GetID() const { return m_ID; }
        inline StringID GetName() const { return m_name; }
        inline UUID GetCollectionID() const { return m_collectionID; }
        inline U32 GetNumComponents() const { return (U32) m_components.size(); }
        inline U32 GetNumSystems() const { return (U32) m_systems.size(); }

        // Spatial Info
        inline bool IsSpatialEntity() const { return m_pRootSpatialComponent != nullptr; }
        inline SpatialEntityComponent const* GetRootSpatialComponent() const { return m_pRootSpatialComponent; }
        inline Transform const& GetWorldTransform() const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetLocalTransform(); }
        inline void SetWorldTransform( Transform const& worldTransform ) const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->SetLocalTransform( worldTransform ); }
        inline OBB const& GetWorldBounds() const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetWorldBounds(); }
        inline bool HasSpatialParent() const { return m_pParentSpatialEntity != nullptr; }
        inline Entity const* GetSpatialParent() const { return m_pParentSpatialEntity; }
        inline Transform GetAttachmentSocketTransform( StringID socketID ) const { KRG_ASSERT( IsSpatialEntity() ); return m_pRootSpatialComponent->GetAttachmentSocketTransform( socketID ); }

        // Status
        inline bool IsInCollection() const { return m_collectionID.IsValid(); }
        inline bool IsActivated() const { return m_status == Status::Activated; }
        inline bool IsDeactivated() const { return m_status == Status::Deactivated; }

        // Systems and components
        inline bool RequiresUpdate( UpdateStage stage ) const { return !m_systemUpdateLists[(S8) stage].empty(); }
        inline TVector<IEntitySystem*> const& GetSystems() const { return m_systems; }
        inline TVector<EntityComponent*> const& GetComponents() const { return m_components; }

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
        bool UpdateComponentLoading( EntityModel::LoadingContext const& loadingContext );

    protected:

        UUID                                            m_ID;                                   // The unique ID of this entity
        UUID                                            m_collectionID;                         // The ID of the collection that this entity is part of
        StringID                                        m_name;
        Status                                          m_status = Status::Deactivated;

        TVector<IEntitySystem*>                         m_systems;
        TVector<EntityComponent*>                       m_components;
        SystemUpdateList                                m_systemUpdateLists[(S8) UpdateStage::NumStages];

        SpatialEntityComponent*                         m_pRootSpatialComponent = nullptr;      // This spatial component defines our world position
        TVector<Entity*>                                m_attachedEntities;                     // The list of entities that are attached to this entity
        Entity*                                         m_pParentSpatialEntity = nullptr;       // The parent entity we are attached to
        StringID                                        m_parentAttachmentSocketID;             // The socket that we are attached to on the parent
        bool                                            m_isAttachedToParent = false;           // Has the actual entity-to-entity component attachment been created
    };
 }