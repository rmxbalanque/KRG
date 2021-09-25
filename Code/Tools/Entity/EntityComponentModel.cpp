#include "EntityComponentModel.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"
#include "Engine/Core/Entity/EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityComponentModel::EntityComponentModel( TypeSystem::TypeInstanceModel const& typeInstance, UUID const& ID, StringID const& name )
        : m_typeInstance( typeInstance )
        , m_ID( ID )
        , m_name( name )
    {
        KRG_ASSERT( typeInstance.IsValid() && typeInstance.GetTypeInfo()->m_metadata.IsFlagSet( TypeSystem::ETypeInfoMetaData::EntityComponent ) );
        m_isSpatialComponent = typeInstance.GetTypeInfo()->IsDerivedFrom<SpatialEntityComponent>();
    }

    EntityComponentModel::EntityComponentModel( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID, StringID const& name )
        : EntityComponentModel( TypeSystem::TypeInstanceModel( typeRegistry, pTypeInfo ), ID, name )
    {}

    EntityComponentModel::EntityComponentModel( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, EntityComponentDescriptor const& componentDesc )
        : EntityComponentModel( TypeSystem::TypeInstanceModel( typeRegistry, pTypeInfo ), componentDesc.m_ID, componentDesc.m_name )
    {
        for ( auto const& propertyDesc : componentDesc.m_properties )
        {
            auto pPropertyInstance = GetProperty( propertyDesc.m_path, true );
            if ( pPropertyInstance != nullptr )
            {
                pPropertyInstance->SetStringValue( propertyDesc.m_stringValue );
            }
        }

        //-------------------------------------------------------------------------

        if ( IsSpatialComponent() )
        {
            SetAttachmentSocketID( componentDesc.m_attachmentSocketID );
            m_transform = GetTransformPropertyValue();
        }
    }

    EntityComponentModel::~EntityComponentModel()
    {
        for ( auto& pChildComponent : m_childComponents )
        {
            KRG::Delete( pChildComponent );
        }
        m_childComponents.clear();
    }

    //-------------------------------------------------------------------------

    EntityComponentModel* EntityComponentModel::CreateChildComponent( TypeSystem::TypeInstanceModel const& typeInstance, UUID const& ID, StringID const& name )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() );
        return m_childComponents.emplace_back( KRG::New<EntityComponentModel>( typeInstance, ID, name ) );
    }

    EntityComponentModel* EntityComponentModel::CreateChildComponent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID, StringID const& name )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() );
        return m_childComponents.emplace_back( KRG::New<EntityComponentModel>( typeRegistry, pTypeInfo, ID, name ) );
    }

    void EntityComponentModel::DestroyChildComponent( int32 childComponentIdx )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() && childComponentIdx >= 0 && childComponentIdx < m_childComponents.size() );
        m_childComponents.erase( m_childComponents.begin() + childComponentIdx );
    }

    //-------------------------------------------------------------------------

    void EntityComponentModel::SetParentEntityID( UUID parentEntityID )
    {
        KRG_ASSERT( !m_parentEntityID.IsValid() );
        m_parentEntityID = parentEntityID;

        for ( auto pChildComponent : m_childComponents )
        {
            pChildComponent->SetParentEntityID( parentEntityID );
        }
    }

    void EntityComponentModel::ClearParentEntityID()
    {
        KRG_ASSERT( m_parentEntityID.IsValid() );
        m_parentEntityID.Reset();

        for ( auto pChildComponent : m_childComponents )
        {
            pChildComponent->ClearParentEntityID();
        }
    }

    void EntityComponentModel::AddChildComponent( EntityComponentModel* pComponent )
    {
        KRG_ASSERT( IsValid() && IsSpatialComponent() && pComponent != nullptr && pComponent->IsSpatialComponent() && !pComponent->IsOwnedByEntity() );
        m_childComponents.emplace_back( pComponent );
        pComponent->SetParentEntityID( m_parentEntityID );
    }

   EntityComponentModel* EntityComponentModel::RemoveChildComponent( int32 childComponentIdx )
    {
       KRG_ASSERT( IsValid() && IsSpatialComponent() && childComponentIdx >= 0 && childComponentIdx < m_childComponents.size() );
       auto pComponent = m_childComponents[childComponentIdx];
       pComponent->ClearParentEntityID();

       m_childComponents.erase_unsorted( m_childComponents.begin() + childComponentIdx );
       return pComponent;
    }

   void EntityComponentModel::GetAllChildComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes, TVector<EntityComponentModel const*>& outFoundComponents ) const
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

   //-------------------------------------------------------------------------

   Transform EntityComponentModel::GetTransformPropertyValue() const
   {
       KRG_ASSERT( IsSpatialComponent() );
       static StringID const transformPropertyID( "m_transform" );

       //-------------------------------------------------------------------------

       auto pTransformProperty = m_typeInstance.GetProperty( transformPropertyID );
       KRG_ASSERT( pTransformProperty != nullptr );
       return pTransformProperty->GetValue<Transform>();
   }

   void EntityComponentModel::SetTransformPropertyValue( Transform const& transform )
   {
       KRG_ASSERT( IsSpatialComponent() );
       static StringID const transformPropertyID( "m_transform" );

       //-------------------------------------------------------------------------

       auto pTransformProperty = m_typeInstance.GetProperty( transformPropertyID );
       KRG_ASSERT( pTransformProperty != nullptr );
       pTransformProperty->SetValue<Transform>( transform );
   }
}