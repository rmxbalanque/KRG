#include "EntityInstanceModel.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityInstanceModel::EntityInstanceModel( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID, StringID name )
        : m_pTypeRegistry( &typeRegistry )
        , m_ID( ID )
        , m_name( name )
        , m_typeInstance( typeRegistry, pTypeInfo )
    {
        KRG_ASSERT( pTypeInfo != nullptr && pTypeInfo->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::Entity ) );
        KRG_ASSERT( ID.IsValid() && name.IsValid() );
    }

    EntityInstanceModel::EntityInstanceModel( TypeSystem::TypeRegistry const& typeRegistry, UUID const& ID, StringID name )
        : EntityInstanceModel( typeRegistry, typeRegistry.GetTypeInfo( Entity::GetStaticTypeID() ), ID, name )
    {}

    EntityInstanceModel::~EntityInstanceModel()
    {
        for ( auto& pEntity : m_childEntities )
        {
            KRG::Delete( pEntity );
        }
        m_childEntities.clear();

        //-------------------------------------------------------------------------

        for ( auto& pSystem : m_systems )
        {
            KRG::Delete( pSystem );
        }
        m_systems.clear();

        //-------------------------------------------------------------------------

        for ( auto& pComponent : m_components )
        {
            KRG::Delete( pComponent );
        }
        m_components.clear();
    }

    //-------------------------------------------------------------------------

    bool EntityInstanceModel::IsValid() const
    {
        if ( !m_typeInstance.IsValid() )
        {
            return false;
        }

        if ( !m_ID.IsValid() || !m_name.IsValid() )
        {
            return false;
        }

        if ( m_isSpatialEntity && !m_rootComponentID.IsValid() )
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------
    
    void EntityInstanceModel::UpdateWorldTransforms( Transform const& parentTransform )
    {
        KRG_ASSERT( IsSpatialEntity() );
        auto pRootComponent = FindMutableComponent( m_rootComponentID );
        auto const rootWorldTransform = pRootComponent->UpdateWorldTransform( parentTransform );

        // Update all child entities
        for ( auto pChildEntity : m_childEntities )
        {
            pChildEntity->UpdateWorldTransforms( rootWorldTransform );
        }
    }

    //-------------------------------------------------------------------------

    void EntityInstanceModel::AddComponent( EntityComponentModel* pComponent )
    {
        KRG_ASSERT( IsValid() && pComponent != nullptr );
        KRG_ASSERT( pComponent->IsValid() && !pComponent->IsOwnedByEntity() );

        //-------------------------------------------------------------------------

        // Add component
        m_components.emplace_back( pComponent );
        pComponent->SetParentEntityID( m_ID );

        // Update spatial hierarchy
        if ( pComponent->IsSpatialComponent() )
        {
            // We are allowed only 1 spatial component per entity in the tools
            KRG_ASSERT( !m_isSpatialEntity );

            m_isSpatialEntity = true;
            m_rootComponentID = pComponent->GetID();
        }
    }

    void EntityInstanceModel::CreateComponent( TypeSystem::TypeID componentTypeID, StringID const& name )
    {
        KRG_ASSERT( m_pTypeRegistry != nullptr );
        auto pTypeInfo = m_pTypeRegistry->GetTypeInfo( componentTypeID );
        KRG_ASSERT( pTypeInfo != nullptr && pTypeInfo->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntityComponent ) );

        // Add component
        auto pAddedComponent = m_components.emplace_back( KRG::New<EntityComponentModel>( TypeSystem::TypeInstanceModel( *m_pTypeRegistry, pTypeInfo ), UUID::GenerateID(), name ) );
        pAddedComponent->SetParentEntityID( m_ID );

        // Update spatial hierarchy
        if ( pAddedComponent->IsSpatialComponent() )
        {
            // We are allowed only 1 spatial component per entity in the tools
            KRG_ASSERT( !m_isSpatialEntity );

            m_isSpatialEntity = true;
            m_rootComponentID = pAddedComponent->GetID();
        }
    }

    EntityComponentModel* EntityInstanceModel::RemoveComponent( int32 componentIdx )
    {
        KRG_ASSERT( IsValid() && componentIdx >= 0 && componentIdx < m_components.size() );

        // Update root component ID
        auto pComponent = m_components[componentIdx];
        if ( pComponent->GetID() == m_rootComponentID )
        {
            m_rootComponentID.Reset();
        }

        // Clear parent entity ID
        pComponent->ClearParentEntityID();

        // Remove and return component
        m_components.erase_unsorted( m_components.begin() + componentIdx );
        return pComponent;
    }

    void EntityInstanceModel::DestroyComponent( int32 componentIdx )
    {
        KRG_ASSERT( IsValid() && componentIdx >= 0 && componentIdx < m_components.size() );

        // Update root component ID
        auto pComponent = m_components[componentIdx];
        if ( pComponent->GetID() == m_rootComponentID )
        {
            m_rootComponentID.Reset();
        }

        // Destroy component
        KRG::Delete( m_components[componentIdx] );
        m_components.erase_unsorted( m_components.begin() + componentIdx );
    }

    void EntityInstanceModel::GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes, TVector<EntityComponentModel const*>& outFoundComponents ) const
    {
        for ( auto const& pComponent : m_components )
        {
            KRG_ASSERT( pComponent != nullptr && pComponent->IsValid() );
            if ( pComponent->GetTypeID() == componentTypeID || ( allowDerivedTypes && pComponent->IsTypeDerivedFrom( componentTypeID ) ) )
            {
                outFoundComponents.emplace_back( pComponent );
            }

            pComponent->GetAllChildComponentsOfType( componentTypeID, allowDerivedTypes, outFoundComponents );
        }
    }

    //-------------------------------------------------------------------------

    void EntityInstanceModel::AddSystem( EntitySystemModel* pSystem )
    {
        KRG_ASSERT( IsValid() && pSystem != nullptr && pSystem->IsValid() && !pSystem->IsOwnedByEntity() );

        for ( auto const& pExistingSystem : m_systems )
        {
            KRG_ASSERT( pExistingSystem->GetTypeID() != pSystem->GetTypeID() );
        }

        pSystem->m_parentEntityID = m_ID;
        m_systems.emplace_back( pSystem );
    }

    EntitySystemModel* EntityInstanceModel::RemoveSystem( int32 systemIdx )
    {
        KRG_ASSERT( IsValid() && systemIdx >= 0 && systemIdx < m_systems.size() );
        auto pSystem = m_systems[systemIdx];
        pSystem->m_parentEntityID.Reset();

        m_systems.erase_unsorted( m_systems.begin() + systemIdx );
        return pSystem;
    }

    void EntityInstanceModel::DestroySystem( int32 systemIdx )
    {
        KRG_ASSERT( IsValid() && systemIdx >= 0 && systemIdx < m_systems.size() );
        KRG::Delete( m_systems[systemIdx] );
        m_systems.erase_unsorted( m_systems.begin() + systemIdx );
    }

    //-------------------------------------------------------------------------

    EntityInstanceModel* EntityInstanceModel::CreateChildEntity( UUID const& ID, StringID name )
    {
        KRG_ASSERT( IsValid() );
        auto pNewEntity = KRG::New<EntityInstanceModel>( *m_pTypeRegistry, m_typeInstance.GetTypeInfo(), ID, name );
        pNewEntity->m_parentEntityID = m_ID;
        return m_childEntities.emplace_back( pNewEntity );
    }

    void EntityInstanceModel::AddChildEntity( EntityInstanceModel* pEntity )
    {
        KRG_ASSERT( IsValid() && pEntity != nullptr && pEntity->IsValid() && !pEntity->IsOwnedByEntity() );
        pEntity->m_parentEntityID = m_ID;
        m_childEntities.emplace_back( pEntity );
    }

    EntityInstanceModel* EntityInstanceModel::RemoveChildEntity( int32 childEntityIdx )
    {
        KRG_ASSERT( IsValid() && childEntityIdx >= 0 && childEntityIdx < m_childEntities.size() );        
        auto pEntity = m_childEntities[childEntityIdx];
        pEntity->m_parentEntityID.Reset();

        m_childEntities.erase_unsorted( m_childEntities.begin() + childEntityIdx );
        return pEntity;
    }

    void EntityInstanceModel::DestroyChildEntity( int32 childEntityIdx )
    {
        KRG_ASSERT( IsValid() && childEntityIdx >= 0 && childEntityIdx < m_childEntities.size() );
        KRG::Delete( m_childEntities[childEntityIdx] );
        m_childEntities.erase_unsorted( m_childEntities.begin() + childEntityIdx );
    }
}