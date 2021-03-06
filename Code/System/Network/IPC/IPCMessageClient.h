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
            class KRG_SYSTEM_NETWORK_API Client
            {
            public:

                Client();
                ~Client();

                bool TryConnect( char const* pAddress );
                bool TryReconnect();
                void Disconnect();

                inline bool IsConnected() const { return m_pSocket != nullptr; }
                inline ClientID const& GetClientID() const { return m_clientID; }
                inline String const& GetAddress() const { return m_address; }

                bool WaitForMessage( Message& message, int32 timeout = 0 );
                void SendMessageToServer( Message const& message );

            private:

                bool TryConnectInternal();

            private:

                void*                                   m_pContext = nullptr;
                void*                                   m_pSocket = nullptr;
                String                                  m_address;
                ClientID                                m_clientID;
            };
        }
    }
}