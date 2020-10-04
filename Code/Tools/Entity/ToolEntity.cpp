#include "ToolEntity.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ToolEntity::ToolEntity( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID, StringID name )
        : m_pTypeRegistry( &typeRegistry )
        , m_ID( ID )
        , m_name( name )
        , m_typeInstance( typeRegistry, pTypeInfo )
    {
        KRG_ASSERT( pTypeInfo != nullptr && pTypeInfo->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::Entity ) );
        KRG_ASSERT( ID.IsValid() && name.IsValid() );
    }

    ToolEntity::ToolEntity( TypeSystem::TypeRegistry const& typeRegistry, UUID const& ID, StringID name )
        : ToolEntity( typeRegistry, typeRegistry.GetTypeInfo( Entity::GetStaticTypeID() ), ID, name )
    {}

    //-------------------------------------------------------------------------

    void ToolEntity::AddComponent( ToolEntityComponent const& component )
    {
        KRG_ASSERT( IsValid() && component.IsValid() );

        //-------------------------------------------------------------------------

        auto addedComponent = m_components.emplace_back( component );
        if ( component.IsSpatialComponent() )
        {
            // We are allowed only 1 spatial component per entity in the tools
            KRG_ASSERT( !m_isSpatialEntity );

            m_isSpatialEntity = true;
            m_rootComponentID = addedComponent.GetID();
        }
    }

    void ToolEntity::CreateComponent( TypeSystem::TypeID componentTypeID, StringID const& name )
    {
        KRG_ASSERT( m_pTypeRegistry != nullptr );
        auto pTypeInfo = m_pTypeRegistry->GetTypeInfo( componentTypeID );
        KRG_ASSERT( pTypeInfo != nullptr && pTypeInfo->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntityComponent ) );

        auto addedComponent = m_components.emplace_back( ToolEntityComponent( DynamicTypeInstance( *m_pTypeRegistry, pTypeInfo ), UUID::GenerateID(), name ) );
        if ( addedComponent.IsSpatialComponent() )
        {
            // We are allowed only 1 spatial component per entity in the tools
            KRG_ASSERT( !m_isSpatialEntity );

            m_isSpatialEntity = true;
            m_rootComponentID = addedComponent.GetID();
        }
    }

    //-------------------------------------------------------------------------

    void ToolEntity::AddSystem( ToolEntitySystem const& system )
    {
        KRG_ASSERT( IsValid() && system.IsValid() );

        for ( auto const& existingSystem : m_systems )
        {
            KRG_ASSERT( system.GetTypeID() != system.GetTypeID() );
        }

        m_systems.emplace_back( system );
    }

    //-------------------------------------------------------------------------

    ToolEntity& ToolEntity::CreateChildEntity( UUID const& ID, StringID name )
    {
        KRG_ASSERT( IsValid() );
        return m_childEntities.emplace_back( ToolEntity( *m_pTypeRegistry, m_typeInstance.GetTypeInfo(), ID, name ) );
    }

    void ToolEntity::DestroyChildEntity( S32 childEntityIdx )
    {
        KRG_ASSERT( IsValid() && childEntityIdx >= 0 && childEntityIdx < m_childEntities.size() );
        m_childEntities.erase( m_childEntities.begin() + childEntityIdx );
    }
}