#pragma once

#include "_Module/API.h"
#include "ResourceID.h"
#include "System/Core/Types/Event.h"
#include "System/Core/Types/LoadingStatus.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    //-------------------------------------------------------------------------
    // The resource provider is the system that is responsible for resolving a resource request to raw resource data
    // It is responsible for loading the request resource from the disk/network as well decompressing it
    // The raw decompressed data is then provided to the resource system for installation
    //-------------------------------------------------------------------------

    class ResourceRequest;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RESOURCE_API ResourceProvider
    {

    public:

        ResourceProvider() = default;
        virtual ~ResourceProvider() {}

        virtual bool IsReady() const = 0;
        virtual bool Initialize() { return true; }
        virtual void Shutdown() {}

        // The resource provider update function
        void Update();

        // Request a resource to be loaded
        void RequestRawResource( ResourceRequest* pRequest );

        // Cancel a prior request
        void CancelRequest( ResourceRequest* pRequest );

        // Get any externally updated resource for this update
        #if KRG_DEVELOPMENT_TOOLS
        TVector<ResourceID> const& GetExternallyUpdatedResources() const { return m_externallyUpdatedResources; }
        #endif

    protected:

        virtual void UpdateInternal() = 0;

        // Called just before emplacing the request on the request list
        virtual void RequestResourceInternal( ResourceRequest* pRequest ) {};

        // Called just before removing the request from the request list
        virtual void CancelRequestInternal( ResourceRequest* pRequest ) {};

        // This function is called once the provider is finished with the request and will remove the request from the requests list
        void FinalizeRequest( ResourceRequest* pRequest );

    protected:

        TVector<ResourceRequest*>          m_requests;

        #if KRG_DEVELOPMENT_TOOLS
        TVector<ResourceID>                m_externallyUpdatedResources;
        #endif
    };
}