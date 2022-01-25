#include "PackagedResourceProvider.h"
#include "System/Resource/ResourceRequest.h"
#include "System/Resource/ResourceSettings.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    PackagedResourceProvider::PackagedResourceProvider( Settings const* pSettings )
        : ResourceProvider()
        , m_compiledResourcesPath( pSettings->m_compiledResourcePath )
    {
        KRG_ASSERT( pSettings != nullptr );
        KRG_ASSERT( m_compiledResourcesPath.IsValid() );
    }

    bool PackagedResourceProvider::IsReady() const
    {
        return true;
    }

    void PackagedResourceProvider::RequestRawResource( ResourceRequest* pRequest )
    {
        FileSystem::Path const resourceFilePath = pRequest->GetResourceID().GetResourcePath().ToFileSystemPath( m_compiledResourcesPath );
        pRequest->OnRawResourceRequestComplete( resourceFilePath.c_str() );
    }

    void PackagedResourceProvider::CancelRequest( ResourceRequest* pRequest )
    {
         // Do Nothing
    }
}