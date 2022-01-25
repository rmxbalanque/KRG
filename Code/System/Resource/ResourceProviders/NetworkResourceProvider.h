#pragma once

#include "ResourceNetworkMessages.h"
#include "System/Resource/ResourceProvider.h"
#include "System/Network/IPC/IPCMessageClient.h"
#include "System/Core/Time/Timers.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Resource
{
    class Settings;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RESOURCE_API NetworkResourceProvider final : public ResourceProvider
    {

    public:

        NetworkResourceProvider( Settings const* pSettings );

        virtual bool IsReady() const override final;

    private:

        NetworkResourceProvider() = delete;

        virtual bool Initialize() override final;
        virtual void Shutdown() override final;
        virtual void Update() override final;

        virtual void RequestRawResource( ResourceRequest* pRequest ) override;
        virtual void CancelRequest( ResourceRequest* pRequest ) override;

        virtual TVector<ResourceID> const& GetExternallyUpdatedResources() const override { return m_externallyUpdatedResources; }

    private:

        Network::IPC::Client                                m_networkClient;
        String                                              m_address;
        TVector<NetworkResourceResponse>                    m_serverReponses;
        Threading::LockFreeQueue<Network::IPC::Message>     m_messagesToSend;
        bool                                                m_networkFailureDetected = false;

        TVector<ResourceRequest*>                           m_requests;

        #if KRG_DEVELOPMENT_TOOLS
        TVector<ResourceID>                                 m_externallyUpdatedResources;
        #endif
    };
}
#endif