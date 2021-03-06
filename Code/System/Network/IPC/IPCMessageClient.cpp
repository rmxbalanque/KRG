#include "IPCMessageClient.h"
#include "System/Core/Logging/Log.h"
#include <zmq.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Network
    {
        namespace IPC
        {
            Client::Client()
                : m_pContext( zmq_ctx_new() )
                , m_clientID( ClientID::Generate() )
            {}

            Client::~Client()
            {
                Disconnect();
                zmq_ctx_destroy( m_pContext );
                m_pContext = nullptr;
            }

            bool Client::TryConnect( char const* pAddress )
            {
                // Create server address string
                KRG_ASSERT( pAddress != nullptr && m_pSocket == nullptr );
                m_address = String( "tcp://" ).append( pAddress );
                return TryConnectInternal();
            }

            bool Client::TryReconnect()
            {
                Disconnect();
                return TryConnectInternal();
            }

            bool Client::TryConnectInternal()
            {
                m_pSocket = zmq_socket( m_pContext, ZMQ_DEALER );

                // Create random client ID
                zmq_setsockopt( m_pSocket, ZMQ_ROUTING_ID, m_clientID.Data(), m_clientID.Size() );

                int result = zmq_connect( m_pSocket, m_address.c_str() );
                if ( result != 0 )
                {
                    zmq_close( m_pSocket );
                    m_pSocket = nullptr;
                }

                return m_pSocket != nullptr;
            }

            void Client::Disconnect()
            {
                if ( m_pSocket != nullptr )
                {
                    zmq_close( m_pSocket );
                    m_pSocket = nullptr;
                }
            }

            //-------------------------------------------------------------------------

            bool Client::WaitForMessage( Message& message, int32 timeout )
            {
                KRG_ASSERT( m_pSocket != nullptr );

                zmq_pollitem_t pollItem = { m_pSocket, 0, ZMQ_POLLIN, 0 };
                if ( zmq_poll( &pollItem, 1, timeout ) > 0 )
                {
                    if ( pollItem.revents & ZMQ_POLLIN )
                    {
                        // Message Data
                        //-------------------------------------------------------------------------

                        zmq_msg_t msg;
                        int rc = zmq_msg_init( &msg );
                        KRG_ASSERT( rc == 0 );
                        rc = zmq_msg_recv( &msg, m_pSocket, 0 );
                        KRG_ASSERT( rc != -1 );

                        message.Initialize( zmq_msg_data( &msg ), zmq_msg_size( &msg ) );
                        zmq_msg_close( &msg );

                        return true;
                    }
                }

                return false;
            }

            void Client::SendMessageToServer( Message const& message )
            {
                int32 result = zmq_send( m_pSocket, message.m_data.data(), message.m_data.size(), 0 );
                if ( result == -1 )
                {
                    int32 const error = zmq_errno();
                    KRG_LOG_ERROR( "Network", "ZMQ failed to send message: %s", zmq_strerror( error ) );
                }
            }
        }
    }
}