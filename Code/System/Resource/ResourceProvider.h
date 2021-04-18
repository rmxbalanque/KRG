#pragma once

#include "_Module/API.h"
#include "ResourceID.h"
#include "System/Core/Types/Event.h"
#include "System/Core/Types/LoadingStatus.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
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
            virtual void Update() = 0;

            // Request a resource to be loaded
            void RequestRawResource( ResourceRequest* pRequest );

            // Cancel a prior request
            void CancelRequest( ResourceRequest* pRequest );

            // This event is fired when a resource is updated, this is primarily used to support hot-reload
            inline TSingleUserEvent<void( ResourceID const& )> OnResourceExternallyUpdated() { return m_resourceExternalUpdateEvent; }

        protected:

            // Called just before emplacing the request on the request list
            virtual void RequestResourceInternal( ResourceRequest* pRequest ) {};

            // Called just before removing the request from the request list
            virtual void CancelRequestInternal( ResourceRequest* pRequest ) {};

            // This function is called once the provider is finished with the request and will remove the request from the requests list
            void FinalizeRequest( ResourceRequest* pRequest );

        protected:

            TVector<ResourceRequest*>                                   m_requests;
            TSingleUserEventInternal<void(ResourceID const& )>          m_resourceExternalUpdateEvent;
        };
    }
}