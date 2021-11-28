#include "IPCMessageServer.h"
#include "System/Core/Logging/Log.h"
#include "steam/isteamnetworkingsockets.h"

//-------------------------------------------------------------------------

namespace KRG::Network::IPC
{
    void Server::SendMessage( Message&& message )
    {
        auto& outgoingMessage = m_outgoingMessages.emplace_back( eastl::move( message ) );
    }

    void Server::ProcessMessage( uint32 connectionID, void* pData, size_t size )
    {
        auto& msg = m_incomingMessages.emplace_back( Message() );
        msg.Initialize( pData, size );
        msg.SetClientConnectionID( connectionID );
    }

    void Server::ProcessIncomingMessages( TFunction<void( Message const& message )> messageProcessorFunction )
    {
        for ( auto& msg : m_incomingMessages )
        {
            messageProcessorFunction( msg );
        }

        m_incomingMessages.clear();
    }

    void Server::SendMessages( TFunction<void( ServerConnection::ClientConnectionID, void*, uint32 )> const& sendFunction )
    {
        for ( auto& msg : m_outgoingMessages )
        {
            if ( msg.m_clientConnectionID != 0 )
            {
                sendFunction( msg.m_clientConnectionID, msg.m_data.data(), (uint32) msg.m_data.size() );
            }
            else // Send to All
            {
                for ( auto const& client : m_connectedClients )
                {
                    sendFunction( client.m_ID, msg.m_data.data(), (uint32) msg.m_data.size() );
                }
            }
        }

        m_outgoingMessages.clear();
    }
}