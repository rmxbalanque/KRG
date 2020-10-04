#pragma once

#include "System/Network/_Module/API.h"
#include "IPCMessage.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Network
    {
        namespace IPC
        {
            class KRG_SYSTEM_NETWORK_API Server
            {
                static constexpr S32 const MaxConnections = 64;

            public:

                Server();
                ~Server();

                bool Start( S32 portNumber );
                void Stop();

                inline bool IsRunning() const { return m_pSocket != nullptr; }

                bool WaitForMessage( Message& message, S32 timeout = 0 );
                void SendMessageToClient( ClientID clientID, Message& message );

            private:

                void*           m_pContext = nullptr;
                void*           m_pSocket = nullptr;
                String          m_address;
            };
        }
    }
}
