#include "NavmeshLoader.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Navmesh/NavmeshData.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    NavmeshLoader::NavmeshLoader()
    {
        m_loadableTypes.push_back( NavmeshData::GetStaticResourceTypeID() );
    }

    bool NavmeshLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        auto pNavmeshData = KRG::New<NavmeshData>();
        archive >> *pNavmeshData;
      
        pResourceRecord->SetResourceData( pNavmeshData );
        return true;
    }

    void NavmeshLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto pNavmeshData = pResourceRecord->GetResourceData<NavmeshData>();
        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}