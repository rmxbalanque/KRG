#include "ResourceProvider.h"
#include "ResourceRequest.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class InternalResourceRequest : public ResourceRequest
    {
        friend class ResourceProvider;
    };

    static_assert( sizeof( InternalResourceRequest ) == sizeof( ResourceRequest ), "Sizes need to match exactly!" );

    //-------------------------------------------------------------------------

    void ResourceProvider::Update()
    {
        #if KRG_DEVELOPMENT_TOOLS
        m_externallyUpdatedResources.clear();
        #endif

        UpdateInternal();
    }

    void ResourceProvider::RequestRawResource( ResourceRequest* pRequest )
    {
        KRG_ASSERT( pRequest != nullptr && pRequest->IsValid() && pRequest->GetLoadingStatus() == LoadingStatus::Loading );

        #if KRG_DEVELOPMENT_TOOLS
        auto predicate = [] ( ResourceRequest* pRequest, ResourceID const& resourceID ) { return pRequest->GetResourceID() == resourceID; };
        auto foundIter = VectorFind( m_requests, pRequest->GetResourceID(), predicate );
        KRG_ASSERT( foundIter == m_requests.end() );
        #endif

        //-------------------------------------------------------------------------

        RequestResourceInternal( pRequest );
        m_requests.emplace_back( pRequest );
    }

    void ResourceProvider::CancelRequest( ResourceRequest* pRequest )
    {
        KRG_ASSERT( pRequest != nullptr && pRequest->IsValid() );

        auto foundIter = VectorFind( m_requests, pRequest );
        KRG_ASSERT( foundIter != m_requests.end() );

        CancelRequestInternal( *foundIter );
        m_requests.erase_unsorted( foundIter );
    }

    void ResourceProvider::FinalizeRequest( ResourceRequest* pRequest )
    {
        KRG_ASSERT( pRequest != nullptr && pRequest->IsValid() );

        // Remove from request list
        auto foundIter = VectorFind( m_requests, pRequest );
        KRG_ASSERT( foundIter != m_requests.end() );
        m_requests.erase_unsorted( foundIter );
    }
}