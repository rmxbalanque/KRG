#include "EntityMapLoader.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityMapLoader::EntityMapLoader()
        : m_pTypeRegistry( nullptr )
    {
        m_loadableTypes.push_back( EntityMap::GetStaticResourceTypeID() );
    }

    void EntityMapLoader::SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry )
    {
        KRG_ASSERT( pTypeRegistry != nullptr );
        m_pTypeRegistry = pTypeRegistry;
    }

    bool EntityMapLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() && m_pTypeRegistry != nullptr );

        auto pMap = KRG::New<EntityMap>();
        archive >> *pMap;

        // Set loaded resource
        pResourceRecord->SetResourceData( pMap );

        return true;
    }
}