#include "MaterialLoader.h"
#include "System/Render/RenderMaterial.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------
namespace KRG
{
    namespace Render
    {
        MaterialLoader::MaterialLoader()
        {
            m_loadableTypes.push_back( Material::GetStaticResourceTypeID() );
        }

        bool MaterialLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
        {
            KRG_ASSERT( archive.IsValid() );

            // Create mesh resource
            Material* pMaterial = KRG::New<Material>();
            archive >> *pMaterial;

            pResourceRecord->SetResourceData( pMaterial );
            return true;
        }

        bool MaterialLoader::Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
        {
            Material* pMaterial = pResourceRecord->GetResourceData<Material>();

            KRG_ASSERT( pMaterial->m_pDiffuseTexture.GetResourceID().IsValid() );
            pMaterial->m_pDiffuseTexture = GetInstallDependency( installDependencies, pMaterial->m_pDiffuseTexture.GetResourceID() );

            if ( !pMaterial->IsValid() )
            {
                KRG_LOG_ERROR( "Resource", "Failed to install resource: %s", resID.ToString().c_str() );
                return false;
            }
            else
            {
                ResourceLoader::Install( resID, pResourceRecord, installDependencies );
            }

            return true;
        }
    }
}