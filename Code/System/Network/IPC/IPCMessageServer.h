#pragma once

#include "System/Network/_Module/API.h"
#include "IPCMessage.h"
#include "System/Network/NetworkSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Network::IPC
{
    class KRG_SYSTEM_NETWORK_API Server final : public ServerConnection
    {

    public:

        // Queues a message to be sent. Note this is a destructive operation!! This call will move the data
        void SendMessage( Message&& message );

        // Iterates over all incoming messages and calls the processing function
        void ProcessIncomingMessages( TFunction<void( Message const& message )> messageProcessorFunction );

    private:

        virtual void ProcessMessage( uint32 connectionID, void* pData, size_t size ) override;
        virtual void SendMessages( TFunction<void( ServerConnection::ClientConnectionHandle, void*, uint32 )> const& sendFunction ) override;

    private:

        TVector<Message>            m_incomingMessages;
        TVector<Message>            m_outgoingMessages;
    };
}
