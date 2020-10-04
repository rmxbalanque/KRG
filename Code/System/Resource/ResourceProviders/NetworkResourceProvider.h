#pragma once
#ifndef KRG_CONFIGURATION_FINAL

#include "../_Module/API.h"
#include "ResourceNetworkMessages.h"
#include "System/Resource/ResourceProvider.h"
#include "System/Network/IPC/IPCMessageClient.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class KRG_SYSTEM_RESOURCE_API NetworkResourceProvider : public ResourceProvider
        {

        public:

            NetworkResourceProvider( char const* pAddress, U32 port );

            virtual bool IsReady() const override final;

        private:

            NetworkResourceProvider() = delete;

            virtual bool Initialize() override final;
            virtual void Update() override final;

            virtual void RequestResourceInternal( ResourceRequest* pRequest ) override final;
            virtual void CancelRequestInternal( ResourceRequest* pRequest ) override final;

        private:

            Network::IPC::Client                            m_networkClient;
            String                                          m_address;
            Timer                                           m_keepAliveTimer;
            TVector<NetworkResourceResponse>                m_serverReponses;
            TVector<Network::IPC::Message>                  m_messagesToSend;
        };
    }
}

#endif
