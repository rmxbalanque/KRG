#include "ResourceLoader.h"
#include "ResourceHeader.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        bool ResourceLoader::Load( ResourceID const& resourceID, TVector<Byte>& rawData, ResourceRecord* pResourceRecord ) const
        {
            Serialization::BinaryMemoryArchive archive( Serialization::Mode::Read, rawData );
            if ( archive.IsValid() )
            {
                // Read resource header
                Resource::ResourceHeader header;
                archive >> header;

                // Set all install dependencies
                pResourceRecord->m_installDependencyResourceIDs.reserve( header.m_installDependencies.size() );
                for ( auto const& depResourceID : header.m_installDependencies )
                {
                    pResourceRecord->m_installDependencyResourceIDs.push_back( depResourceID );
                }

                // Perform resource load
                if ( !LoadInternal( resourceID, pResourceRecord, archive ) )
                {
                    KRG_LOG_ERROR( "Resource", "Resource loader failed to load resource: %s", resourceID.c_str() );
                    return false;
                }

                // Loaders must always set a valid resource data ptr, even if the resource internally is invalid
                // This is enforced to prevent leaks from occurring when a loader allocates a resource, then tries to 
                // load it unsuccessfully and then forgets to release the allocated data.
                KRG_ASSERT( pResourceRecord->GetResourceData() != nullptr );
                return true;
            }
            else
            {
                KRG_LOG_ERROR( "Resource", "Failed to read binary resource data (%s)", resourceID.c_str() );
                return false;
            }
        }

        void ResourceLoader::Unload( ResourceID const& resourceID, ResourceRecord* pResourceRecord ) const
        {
            KRG_ASSERT( pResourceRecord->IsUnloading() || pResourceRecord->HasLoadingFailed() );
            UnloadInternal( resourceID, pResourceRecord );
            pResourceRecord->m_installDependencyResourceIDs.clear();
        }

        void ResourceLoader::UnloadInternal( ResourceID const& resourceID, ResourceRecord* pResourceRecord ) const
        {
            IResource* pData = pResourceRecord->GetResourceData();
            KRG::Delete( pData );
            pResourceRecord->SetResourceData( nullptr );
        }
    }
}