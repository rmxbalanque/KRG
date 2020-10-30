#include "ToolEntityComponent.h"
#include "System/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    ToolEntityComponent::ToolEntityComponent( TypeSystem::ToolTypeInstance const& typeInstance, UUID const& ID, StringID const& name )
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
        : ToolEntityComponent( TypeSystem::ToolTypeInstance( typeRegistry, pTypeInfo ), ID, name )
    {}

    ToolEntityComponent::~ToolEntityComponent()
    {
        for ( auto& pChildComponent : m_childComponents )
        {
            KRG::Delete( pChildComponent );
        }
        m_childComponents.clear();
    }

    //-------------------------------------------------------------------------

    ToolEntityComponent* ToolEntityComponent::CreateChildComponent( TypeSystem::ToolTypeInstance const& typeInstance, UUID const& ID, StringID const& name )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() );
        return m_childComponents.emplace_back( KRG::New<ToolEntityComponent>( typeInstance, ID, name ) );
    }

    ToolEntityComponent* ToolEntityComponent::CreateChildComponent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID, StringID const& name )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() );
        return m_childComponents.emplace_back( KRG::New<ToolEntityComponent>( typeRegistry, pTypeInfo, ID, name ) );
    }

    void ToolEntityComponent::DestroyChildComponent( S32 childComponentIdx )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() && childComponentIdx >= 0 && childComponentIdx < m_childComponents.size() );
        m_childComponents.erase( m_childComponents.begin() + childComponentIdx );
    }

    //-------------------------------------------------------------------------

    void ToolEntityComponent::SetParentEntityID( UUID parentEntityID )
    {
        KRG_ASSERT( !m_parentEntityID.IsValid() );
        m_parentEntityID = parentEntityID;

        for ( auto pChildComponent : m_childComponents )
        {
            pChildComponent->SetParentEntityID( parentEntityID );
        }
    }

    void ToolEntityComponent::ClearParentEntityID()
    {
        KRG_ASSERT( m_parentEntityID.IsValid() );
        m_parentEntityID.Reset();

        for ( auto pChildComponent : m_childComponents )
        {
            pChildComponent->ClearParentEntityID();
        }
    }

    void ToolEntityComponent::AddChildComponent( ToolEntityComponent* pComponent )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() && pComponent != nullptr && pComponent->IsSpatialComponent() && !pComponent->IsOwnedByEntity() );
        m_childComponents.emplace_back( pComponent );
        pComponent->SetParentEntityID( m_parentEntityID );
    }

   ToolEntityComponent* ToolEntityComponent::RemoveChildComponent( S32 childComponentIdx )
    {
       KRG_ASSERT( IsValid() && IsSpatialComponent() && childComponentIdx >= 0 && childComponentIdx < m_childComponents.size() );
       auto pComponent = m_childComponents[childComponentIdx];
       pComponent->ClearParentEntityID();

       m_childComponents.erase_unsorted( m_childComponents.begin() + childComponentIdx );
       return pComponent;
    }

   void ToolEntityComponent::GetAllChildComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes, TVector<ToolEntityComponent const*>& outFoundComponents ) const
   {
       for ( auto const& pComponent : m_childComponents )
       {
           KRG_ASSERT( pComponent != nullptr && pComponent->IsValid() );
           if ( pComponent->GetTypeID() == componentTypeID || ( allowDerivedTypes && pComponent->IsTypeDerivedFrom( componentTypeID ) ) )
           {
               outFoundComponents.emplace_back( pComponent );
           }

           pComponent->GetAllChildComponentsOfType( componentTypeID, allowDerivedTypes, outFoundComponents );
       }
   }
}