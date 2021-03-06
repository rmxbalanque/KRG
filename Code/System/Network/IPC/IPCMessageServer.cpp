#include "IPCMessageServer.h"
#include "System/Core/Logging/Log.h"
#include <zmq.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Network
    {
        namespace IPC
        {
            Server::Server()
                : m_pContext( zmq_ctx_new() )
            {}

            Server::~Server()
            {
                Stop();
                zmq_ctx_destroy( m_pContext );
                m_pContext = nullptr;
            }

            bool Server::Start( int32 portNumber )
            {
                // Create server address string
                KRG_ASSERT( m_pSocket == nullptr );
                m_address.sprintf( "tcp://*:%d", portNumber );

                // Create and bind to socket
                m_pSocket = zmq_socket( m_pContext, ZMQ_ROUTER );
                int result = zmq_bind( m_pSocket, m_address.c_str() );
                if ( result != 0 )
                {
                    zmq_close( m_pSocket );
                    m_pSocket = nullptr;
                }

                return m_pSocket != nullptr;
            }

            void Server::Stop()
            {
                if ( m_pSocket != nullptr )
                {
                    zmq_close( m_pSocket );
                    m_pSocket = nullptr;
                }
            }

            bool Server::WaitForMessage( Message& message, int32 timeout )
            {
                zmq_pollitem_t pollItem = { m_pSocket, 0, ZMQ_POLLIN, 0 };
                if ( zmq_poll( &pollItem, 1, timeout ) )
                {
                    if ( pollItem.revents & ZMQ_POLLIN )
                    {
                        // Get client ID
                        //-------------------------------------------------------------------------

                        zmq_msg_t msg;
                        int rc = zmq_msg_init( &msg );
                        KRG_ASSERT( rc == 0 );
                        rc = zmq_recvmsg( m_pSocket, &msg, 0 );
                        KRG_ASSERT( rc != -1 && rc == sizeof( ClientID ) );

                        memcpy( &message.m_clientID, zmq_msg_data( &msg ), zmq_msg_size( &msg ) );
                        zmq_msg_close( &msg );

                        // Get message
                        //-------------------------------------------------------------------------

                        rc = zmq_msg_init( &msg );
                        KRG_ASSERT( rc == 0 );
                        rc = zmq_recvmsg( m_pSocket, &msg, 0 );
                        KRG_ASSERT( rc != -1 );

                        message.Initialize( zmq_msg_data( &msg ), zmq_msg_size( &msg ) );
                        zmq_msg_close( &msg );

                        return true;
                    }
                }

                return false;
            }

            void Server::SendMessageToClient( ClientID clientID, Message& message )
            {
                // Send client ID
                int32 result = zmq_send( m_pSocket, clientID.Data(), clientID.Size(), ZMQ_SNDMORE );
                KRG_ASSERT( result != -1 );

                // Send message
                result = zmq_send( m_pSocket, message.m_data.data(), message.m_data.size(), 0 );
                if ( result == -1 )
                {
                    int32 const error = zmq_errno();
                    KRG_LOG_ERROR( "Network", "ZMQ failed to send message: %s", zmq_strerror( error ) );
                }
            }
        }
    }
}