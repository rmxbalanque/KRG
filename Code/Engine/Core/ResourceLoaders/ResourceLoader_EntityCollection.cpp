#include "ResourceLoader_EntityCollection.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityCollectionLoader::EntityCollectionLoader()
        : m_pTypeRegistry( nullptr )
    {
        m_loadableTypes.push_back( EntityCollectionDescriptor::GetStaticResourceTypeID() );
        m_loadableTypes.push_back( EntityMapDescriptor::GetStaticResourceTypeID() );
    }

    void EntityCollectionLoader::SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry )
    {
        KRG_ASSERT( pTypeRegistry != nullptr );
        m_pTypeRegistry = pTypeRegistry;
    }

    bool EntityCollectionLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() && m_pTypeRegistry != nullptr );

        EntityCollectionDescriptor* pCollectionDesc = nullptr;

        if ( resID.GetResourceTypeID() == EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            auto pMap = KRG::New<EntityMapDescriptor>();
            archive >> *pMap;
            pCollectionDesc = pMap;
        }
        else  if ( resID.GetResourceTypeID() == EntityCollectionDescriptor::GetStaticResourceTypeID() )
        {
            auto pEC = KRG::New<EntityCollectionDescriptor>();
            archive >> *pEC;
            pCollectionDesc = pEC;
        }

        // Set loaded resource
        pResourceRecord->SetResourceData( pCollectionDesc );
        return true;
    }
}