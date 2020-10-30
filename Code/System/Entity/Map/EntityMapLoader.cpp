#include "EntityMapLoader.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapLoader::EntityMapLoader()
        : m_pTypeRegistry( nullptr )
    {
        m_loadableTypes.push_back( EntityMapDescriptor::GetStaticResourceTypeID() );
    }

    void EntityMapLoader::SetTypeRegistry( TypeSystem::TypeRegistry const* pTypeRegistry )
    {
        KRG_ASSERT( pTypeRegistry != nullptr );
        m_pTypeRegistry = pTypeRegistry;
    }

    bool EntityMapLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() && m_pTypeRegistry != nullptr );

        auto pMap = KRG::New<EntityMapDescriptor>();
        archive >> *pMap;

        // Set loaded resource
        pResourceRecord->SetResourceData( pMap );

        return true;
    }
}