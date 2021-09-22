#include "IPCMessageClient.h"
#include "System/Core/Logging/Log.h"


//-------------------------------------------------------------------------

namespace KRG::Network::IPC
{
    void Client::SendMessageToServer( Message&& message )
    {
        message.m_clientConnectionID = GetClientConnectionID();
        m_outgoingMessages.emplace_back( eastl::move( message ) );
    }

    void Client::ProcessMessage( void* pData, size_t size )
    {
        auto& message = m_incomingMessages.emplace_back( Message() );
        message.Initialize( pData, size );
    }

    void Client::SendMessages( TFunction<void( void*, uint32 )> const& sendFunction )
    {
        for ( auto& msg : m_outgoingMessages )
        {
            sendFunction( msg.m_data.data(), (uint32) msg.m_data.size() );
        }

        m_outgoingMessages.clear();
    }

    void Client::ProcessIncomingMessages( TFunction<void( Message const& message )> messageProcessorFunction )
    {
        for ( auto& msg : m_incomingMessages )
        {
            messageProcessorFunction( msg );
        }

        m_incomingMessages.clear();
    }
}