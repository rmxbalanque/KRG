#include "EntityCollectionModel.h"
#include "System/Entity/Collections/EntityCollectionDescriptor.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct EntityCollectionConverter
    {
        static bool Error( char const* pFormat, ... )
        {
            va_list args;
            va_start( args, pFormat );
            Log::AddEntryVarArgs( Log::Severity::Error, "Entity Collection Converter", __FILE__, __LINE__, pFormat, args );
            va_end( args );
            return false;
        }

        static bool Warning( char const* pFormat, ... )
        {
            va_list args;
            va_start( args, pFormat );
            Log::AddEntryVarArgs( Log::Severity::Warning, "Entity Collection Converter", __FILE__, __LINE__, pFormat, args );
            va_end( args );
            return false;
        }

        // Conversion to Model
        //-------------------------------------------------------------------------

        static EntityComponentModel* CreateComponentModel( TypeSystem::TypeRegistry const& typeRegistry, EntityComponentDescriptor const& componentDesc )
        {
            auto pComponentTypeInfo = typeRegistry.GetTypeInfo( componentDesc.m_typeID );
            if ( pComponentTypeInfo == nullptr )
            {
                Error( "Tried to create component of unknown type: %s, for component: %s (%s)", componentDesc.m_typeID.GetAsStringID().c_str(), componentDesc.m_ID.ToString().c_str(), componentDesc.m_name.c_str() );
                return nullptr;
            }

            return KRG::New<EntityComponentModel>( typeRegistry, pComponentTypeInfo, componentDesc );
        }

        static EntitySystemModel* CreateSystemModel( TypeSystem::TypeRegistry const& typeRegistry, EntitySystemDescriptor const& systemDesc )
        {
            auto pSystemTypeInfo = typeRegistry.GetTypeInfo( systemDesc.m_typeID );
            if ( pSystemTypeInfo == nullptr )
            {
                Error( "Tried to create system of unknown type: %s", systemDesc.m_typeID.GetAsStringID().c_str() );
                return nullptr;
            }

            return KRG::New<EntitySystemModel>( typeRegistry, pSystemTypeInfo );
        }

        static EntityInstanceModel* CreateEntityModel( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc )
        {
            struct CreatedComponent
            {
                CreatedComponent( EntityComponentModel* pComponent, EntityComponentDescriptor const* pDesc )
                    : m_pComponent( pComponent )
                    , m_pDesc( pDesc )
                {}

                EntityComponentModel*                m_pComponent;
                EntityComponentDescriptor const*    m_pDesc;
            };

            //-------------------------------------------------------------------------

            auto pCreatedEntity = KRG::New<EntityInstanceModel>( typeRegistry, entityDesc.m_ID, entityDesc.m_name );

            if ( entityDesc.HasSpatialParent() )
            {
                pCreatedEntity->SetAttachmentSocketID( entityDesc.m_attachmentSocketID );
            }

            //-------------------------------------------------------------------------

            TVector<CreatedComponent> components;

            auto Cleanup = [&pCreatedEntity, &components] ()
            {
                for ( auto& createdComponent : components )
                {
                    KRG::Delete( createdComponent.m_pComponent );
                }

                KRG::Delete( pCreatedEntity );
            };

            // Create Components
            //-------------------------------------------------------------------------

            for ( auto const& componentDesc : entityDesc.m_components )
            {
                auto pCreatedComponent = CreateComponentModel( typeRegistry, componentDesc );
                if ( pCreatedComponent == nullptr )
                {
                    Error( "Failed to create component: %s (%s), for entity: %s (%s)", componentDesc.m_ID.ToString().c_str(), componentDesc.m_name.c_str(), pCreatedEntity->GetID().ToString().c_str(), pCreatedEntity->GetNameAsCStr() );
                    Cleanup();
                    return nullptr;
                }

                components.emplace_back( CreatedComponent( pCreatedComponent, &componentDesc ) );
            }

            // Fix component parenting
            //-------------------------------------------------------------------------

            for ( int32 i = (int32) components.size() - 1; i >= 0; i-- )
            {
                if ( !components[i].m_pComponent->IsSpatialComponent() )
                {
                    pCreatedEntity->AddComponent( components[i].m_pComponent );
                    components.erase_unsorted( components.begin() + i );
                }
                else // Spatial component
                {
                    if ( components[i].m_pDesc->IsRootComponent() )
                    {
                        continue;
                    }

                    // Try to find parent component
                    for ( int32 p = 0; p < (int32) components.size(); p++ )
                    {
                        // if we found the parent component, add this component to its parent and delete its entry
                        if ( components[p].m_pComponent->GetID() == components[i].m_pDesc->m_spatialParentID )
                        {
                            components[p].m_pComponent->AddChildComponent( components[i].m_pComponent );
                            components.erase_unsorted( components.begin() + i );
                            break;
                        }
                    }
                }
            }

            // We should either have no components left or a single spatial component, if we have more then we've detected a circular dependency
            if ( components.size() > 1 )
            {
                // Free allocated memory
                Cleanup();
                Error( "Circular component dependency detected for entity: %s (%s)", pCreatedEntity->GetID().ToString().c_str(), pCreatedEntity->GetNameAsCStr() );
                return nullptr;
            }
            else if ( components.size() == 1 )
            {
                KRG_ASSERT( components[0].m_pComponent->IsSpatialComponent() );
                pCreatedEntity->AddComponent( components[0].m_pComponent );
            }

            // Systems
            //-------------------------------------------------------------------------

            for ( auto const& systemDesc : entityDesc.m_systems )
            {
                auto pCreatedSystem = CreateSystemModel( typeRegistry, systemDesc );
                if ( pCreatedSystem == nullptr )
                {
                    Cleanup();
                    return nullptr;
                }

                pCreatedEntity->AddSystem( pCreatedSystem );
            }

            return pCreatedEntity;
        }

        static bool CreateModelFromDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& inCollection, EntityCollectionModel& outCollection )
        {
            outCollection.Clear();

            //-------------------------------------------------------------------------

            struct CreatedEntity
            {
                CreatedEntity( EntityInstanceModel* pEntity, EntityDescriptor const* pDesc )
                    : m_pEntity( pEntity )
                    , m_pDesc( pDesc )
                {}

                EntityInstanceModel*                 m_pEntity;
                EntityDescriptor const*     m_pDesc;
            };

            //-------------------------------------------------------------------------

            TVector<CreatedEntity> entities;

            auto Cleanup = [&entities] ()
            {
                for ( auto& createdEntity : entities )
                {
                    KRG::Delete( createdEntity.m_pEntity );
                }
            };

            // Create entities
            //-------------------------------------------------------------------------

            for ( auto const& entityDesc : inCollection.GetEntityDescriptors() )
            {
                auto pCreatedEntity = CreateEntityModel( typeRegistry, entityDesc );
                if ( pCreatedEntity == nullptr )
                {
                    Cleanup();
                    return false;
                }

                entities.emplace_back( CreatedEntity( pCreatedEntity, &entityDesc ) );
            }

            // Fix parenting and add to collection
            //-------------------------------------------------------------------------

            for ( int32 i = (int32) entities.size() - 1; i >= 0; i-- )
            {
                if ( entities[i].m_pEntity->IsSpatialEntity() )
                {
                    if ( entities[i].m_pDesc->HasSpatialParent() )
                    {
                        bool parentEntityFound = false;

                        // Try to find parent component
                        for ( int32 p = 0; p < (int32) entities.size(); p++ )
                        {
                            // If we found the parent entity, add this entity to its parent and delete its entry
                            if ( entities[p].m_pEntity->GetID() == entities[i].m_pDesc->m_spatialParentID )
                            {
                                entities[p].m_pEntity->AddChildEntity( entities[i].m_pEntity );
                                entities.erase_unsorted( entities.begin() + i );
                                parentEntityFound = true;
                                break;
                            }
                        }

                        //-------------------------------------------------------------------------

                        if ( !parentEntityFound )
                        {
                            Warning( "Cant find parent:  %s, for entity %s (%s). This mean you either have a circular dependency or bad source data.", entities[i].m_pDesc->m_spatialParentID.ToString().c_str(), entities[i].m_pEntity->GetID().ToString().c_str(), entities[i].m_pEntity->GetNameAsCStr() );
                        }
                    }
                }
                else
                {
                    outCollection.AddEntity( entities[i].m_pEntity );
                    entities.erase_unsorted( entities.begin() + i );
                }
            }

            // Add all root spatial entities to the collection
            //-------------------------------------------------------------------------

            for ( auto const& createdEntity : entities )
            {
                outCollection.AddEntity( createdEntity.m_pEntity );
            }

            // Update all world transforms
            //-------------------------------------------------------------------------

            for ( auto pEntity : outCollection.m_entities )
            {
                if ( pEntity->IsSpatialEntity() )
                {
                    pEntity->UpdateWorldTransforms();
                }
            }

            return true;
        }

        // Conversion to Descriptor
        //-------------------------------------------------------------------------

        static void CreateComponentDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor& entityDesc, EntityComponentModel const* pComponent, UUID const& parentComponentID = UUID() )
        {
            KRG_ASSERT( pComponent != nullptr );

            EntityComponentDescriptor componentDesc;
            componentDesc.m_ID = pComponent->GetID();
            componentDesc.m_name = pComponent->GetName();
            componentDesc.m_typeID = pComponent->GetTypeID();
            componentDesc.m_spatialParentID = parentComponentID;
            componentDesc.m_attachmentSocketID = pComponent->GetAttachmentSocketID();
            componentDesc.m_isSpatialComponent = pComponent->IsSpatialComponent();

            // Properties
            //-------------------------------------------------------------------------

            auto const toolTypeDescriptor = pComponent->GetTypeInstance().GetDescriptor();
            for ( auto const& prop : toolTypeDescriptor.m_properties )
            {
                componentDesc.m_properties.emplace_back( TypeSystem::PropertyDescriptor( typeRegistry, prop.m_path, prop.m_typeID, prop.m_templatedArgumentTypeID, prop.m_stringValue ) );
            }

            // Add component to entity
            //-------------------------------------------------------------------------

            entityDesc.m_components.emplace_back( componentDesc );

            // Child Components
            //-------------------------------------------------------------------------

            for ( auto pChildComponent : pComponent->GetChildComponents() )
            {
                CreateComponentDescriptor( typeRegistry, entityDesc, pChildComponent, pComponent->GetID() );
            }
        }

        static void CreateEntityDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor& outCollection, EntityInstanceModel const* pEntity )
        {
            KRG_ASSERT( pEntity != nullptr );

            EntityDescriptor entityDesc;
            entityDesc.m_ID = pEntity->GetID();
            entityDesc.m_name = pEntity->GetName();
            entityDesc.m_spatialParentID = pEntity->GetParentEntityID();
            entityDesc.m_attachmentSocketID = pEntity->GetAttachmentSocketID();

            // Systems
            //-------------------------------------------------------------------------

            for ( auto pSystem : pEntity->GetSystems() )
            {
                auto& systemDesc = entityDesc.m_systems.emplace_back( EntitySystemDescriptor() );
                systemDesc.m_typeID = pSystem->GetTypeInfo()->m_ID;
            }

            // Components
            //-------------------------------------------------------------------------

            for ( auto pComponent : pEntity->GetComponents() )
            {
                CreateComponentDescriptor( typeRegistry, entityDesc, pComponent );
            }

            // Add entity to collection
            //-------------------------------------------------------------------------

            outCollection.AddEntity( entityDesc );

            // Child Entities
            //-------------------------------------------------------------------------

            for ( auto pChildEntity : pEntity->GetChildEntities() )
            {
                CreateEntityDescriptor( typeRegistry, outCollection, pChildEntity );
            }
        }

        static void CreateDescriptorFromModel( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionModel const& inCollection, EntityCollectionDescriptor& outCollection )
        {
            outCollection.Clear();

            for ( auto pEntity : inCollection.GetEntities() )
            {
                CreateEntityDescriptor( typeRegistry, outCollection, pEntity );
            }
        }
    };
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    bool EntityCollectionModel::FromDescriptor( EntityCollectionDescriptor const& inCollectionDesc, EntityCollectionModel& outModel )
    {
        return EntityCollectionConverter::CreateModelFromDescriptor( outModel.m_typeRegistry, inCollectionDesc, outModel );
    }

    //-------------------------------------------------------------------------

    EntityCollectionModel::EntityCollectionModel( TypeSystem::TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    TVector<EntityComponentModel const*> EntityCollectionModel::GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes ) const
    {
        TVector<EntityComponentModel const*> foundComponents;

        for ( auto const& pEntity : m_entities )
        {
            KRG_ASSERT( pEntity != nullptr );
            pEntity->GetAllComponentsOfType( componentTypeID, allowDerivedTypes, foundComponents );
        }

        return foundComponents;
    }

    EntityCollectionModel::~EntityCollectionModel()
    {
        for ( auto& pEntity : m_entities )
        {
            KRG::Delete( pEntity );
        }
    }

    EntityCollectionDescriptor EntityCollectionModel::GetDescriptor() const
    {
        EntityCollectionDescriptor descriptor;
        EntityCollectionConverter::CreateDescriptorFromModel( m_typeRegistry, *this, descriptor );
        return descriptor;
    }
}