#include "ToolEntityComponent.h"
#include "System/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ToolEntityComponent::ToolEntityComponent( DynamicTypeInstance const& typeInstance, UUID const& ID, StringID const& name )
        : m_typeInstance( typeInstance )
        , m_ID( ID )
        , m_name( name )
    {
        KRG_ASSERT( typeInstance.IsValid() && typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntityComponent ) );
        m_isSpatialComponent = typeInstance.GetTypeInfo()->IsDerivedFrom( SpatialEntityComponent::GetStaticTypeID() );

        //-------------------------------------------------------------------------

        static StringID const transformPropertyID( "m_transform" );
        if ( m_isSpatialComponent )
        {
            auto pTransformProperty = m_typeInstance.GetProperty( transformPropertyID );
            KRG_ASSERT( pTransformProperty != nullptr );
            m_transform = pTransformProperty->GetValue<Transform>();
        }
    }

    ToolEntityComponent::ToolEntityComponent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID, StringID const& name )
        : ToolEntityComponent( DynamicTypeInstance( typeRegistry, pTypeInfo ), ID, name )
    {}

    //-------------------------------------------------------------------------

    ToolEntityComponent& ToolEntityComponent::CreateChildComponent( DynamicTypeInstance const& typeInstance, UUID const& ID, StringID const& name )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() );
        return m_childComponents.emplace_back( ToolEntityComponent( typeInstance, ID, name ) );
    }

    ToolEntityComponent& ToolEntityComponent::CreateChildComponent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID, StringID const& name )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() );
        return m_childComponents.emplace_back( ToolEntityComponent( typeRegistry, pTypeInfo, ID, name ) );
    }

    void ToolEntityComponent::DestroyChildComponent( S32 childComponentIdx )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() && childComponentIdx >= 0 && childComponentIdx < m_childComponents.size() );
        m_childComponents.erase( m_childComponents.begin() + childComponentIdx );
    }
}