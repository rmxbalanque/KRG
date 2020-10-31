#include "ToolEntityCollectionConverter.h"
#include "ToolEntityCollection.h"
#include "System/Entity/Collections/EntityCollectionDescriptor.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    namespace
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
    }

    //-------------------------------------------------------------------------

    namespace
    {
        static ToolEntityComponent* CreateComponent( TypeSystem::TypeRegistry const& typeRegistry, EntityComponentDescriptor const& componentDesc )
        {
            auto pComponentTypeInfo = typeRegistry.GetTypeInfo( componentDesc.m_typeID );
            if ( pComponentTypeInfo == nullptr )
            {
                Error( "Tried to create component of unknown type: %s, for component: %s (%s)", componentDesc.m_typeID.GetAsStringID().c_str(), componentDesc.m_ID.ToString().c_str(), componentDesc.m_name.c_str() );
                return nullptr;
            }

            return KRG::New<ToolEntityComponent>( typeRegistry, pComponentTypeInfo, componentDesc );
        }

        static ToolEntitySystem* CreateSystem( TypeSystem::TypeRegistry const& typeRegistry, EntitySystemDescriptor const& systemDesc )
        {
            auto pSystemTypeInfo = typeRegistry.GetTypeInfo( systemDesc.m_typeID );
            if ( pSystemTypeInfo == nullptr )
            {
                Error( "Tried to create system of unknown type: %s", systemDesc.m_typeID.GetAsStringID().c_str() );
                return nullptr;
            }

            return KRG::New<ToolEntitySystem>( typeRegistry, pSystemTypeInfo );
        }

        static ToolEntity* CreateEntity( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc )
        {
            struct CreatedComponent
            {
                CreatedComponent( ToolEntityComponent* pComponent, EntityComponentDescriptor const* pDesc )
                    : m_pComponent( pComponent )
                    , m_pDesc( pDesc )
                {}

                ToolEntityComponent*                m_pComponent;
                EntityComponentDescriptor const*    m_pDesc;
            };

            //-------------------------------------------------------------------------

            auto pCreatedEntity = KRG::New<ToolEntity>( typeRegistry, entityDesc.m_ID, entityDesc.m_name );

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
                auto pCreatedComponent = CreateComponent( typeRegistry, componentDesc );
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

            for ( S32 i = (S32) components.size() - 1; i >= 0; i-- )
            {
                if ( !components[i].m_pComponent->IsSpatialComponent() )
                {
                    pCreatedEntity->AddComponent( components[i].m_pComponent );
                    VectorEraseUnsorted( components, i );
                }
                else // Spatial component
                {
                    if ( components[i].m_pDesc->IsRootComponent() )
                    {
                        continue;
                    }

                    // Try to find parent component
                    for ( S32 p = 0; p < (S32) components.size(); p++ )
                    {
                        // if we found the parent component, add this component to its parent and delete its entry
                        if ( components[p].m_pComponent->GetID() == components[i].m_pDesc->m_spatialParentID )
                        {
                            components[p].m_pComponent->AddChildComponent( components[i].m_pComponent );
                            VectorEraseUnsorted( components, i );
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
                auto pCreatedSystem = CreateSystem( typeRegistry, systemDesc );
                if ( pCreatedSystem == nullptr )
                {
                    Cleanup();
                    return nullptr;
                }

                pCreatedEntity->AddSystem( pCreatedSystem );
            }

            return pCreatedEntity;
        }
    }

    //-------------------------------------------------------------------------

    bool ToolEntityCollectionConverter::ConvertToToolsFormat( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& inCollection, ToolEntityCollection& outCollection )
    {
        outCollection.Clear();

        //-------------------------------------------------------------------------

        struct CreatedEntity
        {
            CreatedEntity( ToolEntity* pEntity, EntityDescriptor const* pDesc )
                : m_pEntity( pEntity )
                , m_pDesc( pDesc )
            {}

            ToolEntity*                 m_pEntity;
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
            auto pCreatedEntity = CreateEntity( typeRegistry, entityDesc );
            if ( pCreatedEntity == nullptr )
            {
                Cleanup();
                return false;
            }

            entities.emplace_back( CreatedEntity( pCreatedEntity, &entityDesc ) );
        }

        // Fix parenting and add to collection
        //-------------------------------------------------------------------------

        for ( S32 i = (S32) entities.size() - 1; i >= 0; i-- )
        {
            if ( entities[i].m_pEntity->IsSpatialEntity() )
            {
                if ( entities[i].m_pDesc->HasSpatialParent() )
                {
                    bool parentEntityFound = false;

                    // Try to find parent component
                    for ( S32 p = 0; p < (S32) entities.size(); p++ )
                    {
                        // If we found the parent entity, add this entity to its parent and delete its entry
                        if ( entities[p].m_pEntity->GetID() == entities[i].m_pDesc->m_spatialParentID )
                        {
                            entities[p].m_pEntity->AddChildEntity( entities[i].m_pEntity );
                            VectorEraseUnsorted( entities, i );
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
                VectorEraseUnsorted( entities, i );
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

    //-------------------------------------------------------------------------

    namespace
    {
        static void CreateComponentDesc( EntityDescriptor& entityDesc, ToolEntityComponent const* pComponent, UUID const& parentComponentID = UUID() )
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
                componentDesc.m_propertyValues.emplace_back( PropertyDescriptor( prop.m_path, prop.m_typeID, prop.m_value ) );
            }

            // Add component to entity
            //-------------------------------------------------------------------------

            entityDesc.m_components.emplace_back( componentDesc );

            // Child Components
            //-------------------------------------------------------------------------
            
            for ( auto pChildComponent : pComponent->GetChildComponents() )
            {
                CreateComponentDesc( entityDesc, pChildComponent, pComponent->GetID() );
            }
        }

        static void CreateEntityDesc( EntityCollectionDescriptor& outCollection, ToolEntity const* pEntity )
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
                CreateComponentDesc( entityDesc, pComponent );
            }

            // Add entity to collection
            //-------------------------------------------------------------------------

            outCollection.AddEntity( entityDesc );

            // Child Entities
            //-------------------------------------------------------------------------

            for ( auto pChildEntity : pEntity->GetChildEntities() )
            {
                CreateEntityDesc( outCollection, pChildEntity );
            }
        }
    }

    bool ToolEntityCollectionConverter::ConvertFromToolsFormat( TypeSystem::TypeRegistry const& typeRegistry, ToolEntityCollection const& inCollection, EntityCollectionDescriptor& outCollection )
    {
        outCollection.Clear();

        for ( auto pEntity : inCollection.GetEntities() )
        {
            CreateEntityDesc( outCollection, pEntity );
        }

        return true;
    }
}