#include "NetworkSystem.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Logging/Log.h"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

//-------------------------------------------------------------------------

namespace KRG::Network
{
    struct NetworkState
    {
        int64                                   m_logTimeZero = 0;
        TInlineVector<ServerConnection*, 2>     m_serverConnections;
        TInlineVector<ClientConnection*, 2>     m_clientConnections;
    };

    static NetworkState* g_pNetworkState = nullptr;

    //-------------------------------------------------------------------------

    struct NetworkCallbackHandler
    {
        static void NetworkDebugOutputFunction( ESteamNetworkingSocketsDebugOutputType type, char const* pMessage )
        {
            KRG_ASSERT( g_pNetworkState != nullptr );
            SteamNetworkingMicroseconds const currentTime = SteamNetworkingUtils()->GetLocalTimestamp() - g_pNetworkState->m_logTimeZero;

            KRG_TRACE_MSG( pMessage );

            // TODO: initialize rpmalloc for the steam network thread
            if ( type == k_ESteamNetworkingSocketsDebugOutputType_Bug )
            {
                //KRG_LOG_FATAL_ERROR( "Network", "(%10.6f) %s", currentTime * 1e-6, pMessage );
            }
            else if ( type == k_ESteamNetworkingSocketsDebugOutputType_Error )
            {
                //KRG_LOG_ERROR( "Network", "(%10.6f) %s", currentTime * 1e-6, pMessage );
            }
            else if ( type == k_ESteamNetworkingSocketsDebugOutputType_Warning )
            {
                //KRG_LOG_WARNING( "Network", "(%10.6f) %s", currentTime * 1e-6, pMessage );
            }
            else if ( type == k_ESteamNetworkingSocketsDebugOutputType_Important )
            {
                //KRG_LOG_MESSAGE( "Network", "(%10.6f) %s", currentTime * 1e-6, pMessage );
            }
            else // Verbose
            {
                //KRG_LOG_MESSAGE( "Network", "(%10.6f) %s", currentTime * 1e-6, pMessage );
            }
        }

        static void ServerNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t* pInfo )
        {
            auto pInterface = SteamNetworkingSockets();
            KRG_ASSERT( pInterface != nullptr );

            // Find Server Connection
            //-------------------------------------------------------------------------

            auto FindServerConnection = [] ( ServerConnection const* pConnection, uint32 socketHandle )
            {
                return pConnection->GetSocketHandle() == socketHandle;
            };

            auto serverConnectionIter = eastl::find( g_pNetworkState->m_serverConnections.begin(), g_pNetworkState->m_serverConnections.end(), pInfo->m_info.m_hListenSocket, FindServerConnection );
            KRG_ASSERT( serverConnectionIter != g_pNetworkState->m_serverConnections.end() );
            ServerConnection* pServerConnection = *serverConnectionIter;

            // Handle state change
            //-------------------------------------------------------------------------

            switch ( pInfo->m_info.m_eState )
            {
                case k_ESteamNetworkingConnectionState_ClosedByPeer:
                case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                {
                    // Ignore if they were not previously connected.  (If they disconnected
                    // before we accepted the connection.)
                    if ( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected )
                    {
                        // Locate the client.  Note that it should have been found, because this
                        // is the only codepath where we remove clients (except on shutdown),
                        // and connection change callbacks are dispatched in queue order.
                        auto clientIter = eastl::find( pServerConnection->m_connectedClients.begin(), pServerConnection->m_connectedClients.end(), pInfo->m_hConn );
                        KRG_ASSERT( clientIter != pServerConnection->m_connectedClients.end() );
                        pServerConnection->m_connectedClients.erase_unsorted( clientIter );
                    }
                    else
                    {
                        KRG_ASSERT( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting );
                    }

                    // Clean up the connection.  This is important!
                    // The connection is "closed" in the network sense, but
                    // it has not been destroyed.  We must close it on our end, too
                    // to finish up.  The reason information do not matter in this case,
                    // and we cannot linger because it's already closed on the other end,
                    // so we just pass 0's.
                    pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
                    break;
                }

                case k_ESteamNetworkingConnectionState_Connecting:
                {
                    // This must be a new connection
                    KRG_ASSERT( eastl::find( pServerConnection->m_connectedClients.begin(), pServerConnection->m_connectedClients.end(), pInfo->m_hConn ) == pServerConnection->m_connectedClients.end() );

                    // A client is attempting to connect
                    // Try to accept the connection.
                    if ( pInterface->AcceptConnection( pInfo->m_hConn ) != k_EResultOK )
                    {
                        // This could fail. If the remote host tried to connect, but then
                        // disconnected, the connection may already be half closed.  Just
                        // destroy whatever we have on our side.
                        pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
                        break;
                    }

                    // Assign the poll group
                    if ( !pInterface->SetConnectionPollGroup( pInfo->m_hConn, pServerConnection->m_pollingGroupHandle ) )
                    {
                        pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
                        break;
                    }

                    // Add them to the client list, using std::map wacky syntax
                    pServerConnection->m_connectedClients.emplace_back( pInfo->m_hConn );
                    break;
                }

                case k_ESteamNetworkingConnectionState_None:
                // NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
                break;

                case k_ESteamNetworkingConnectionState_Connected:
                // We will get a callback immediately after accepting the connection.
                // Since we are the server, we can ignore this, it's not news to us.
                break;

                default:
                // Silences -Wswitch
                break;
            }
        }

        static void ClientNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t* pInfo )
        {
            auto pInterface = SteamNetworkingSockets();
            KRG_ASSERT( pInterface != nullptr );

            // Find Client Connection
            //-------------------------------------------------------------------------

            auto FindClientConnection = [] ( ClientConnection const* pConnection, uint32 connectionHandle )
            {
                return pConnection->GetClientConnectionID() == connectionHandle;
            };

            auto clientConnectionIter = eastl::find( g_pNetworkState->m_clientConnections.begin(), g_pNetworkState->m_clientConnections.end(), pInfo->m_hConn, FindClientConnection );
            KRG_ASSERT( clientConnectionIter != g_pNetworkState->m_clientConnections.end() );
            ClientConnection* pClientConnection = *clientConnectionIter;

            // Handle state change
            //-------------------------------------------------------------------------

            // What's the state of the connection?
            switch ( pInfo->m_info.m_eState )
            {
                case k_ESteamNetworkingConnectionState_ClosedByPeer:
                case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                {
                    // Clean up the connection.  This is important!
                    // The connection is "closed" in the network sense, but
                    // it has not been destroyed.  We must close it on our end, too
                    // to finish up.  The reason information do not matter in this case,
                    // and we cannot linger because it's already closed on the other end,
                    // so we just pass 0's.
                    pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
                    pClientConnection->m_connectionHandle = k_HSteamNetConnection_Invalid;
                    pClientConnection->m_address.clear();
                    break;
                }

                case k_ESteamNetworkingConnectionState_Connecting:
                // We will get this callback when we start connecting.
                // We can ignore this.
                break;

                case k_ESteamNetworkingConnectionState_Connected:
                break;

                case k_ESteamNetworkingConnectionState_None:
                // NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
                break;

                default:
                // Silences -Wswitch
                break;
            }
        }
    };

    //-------------------------------------------------------------------------

    ServerConnection::~ServerConnection()
    {
        KRG_ASSERT( m_pollingGroupHandle == k_HSteamNetPollGroup_Invalid && m_socketHandle == k_HSteamListenSocket_Invalid );
    }

    //-------------------------------------------------------------------------

    ClientConnection::~ClientConnection()
    {
        KRG_ASSERT( m_connectionHandle == k_HSteamNetConnection_Invalid );
    }

    bool ClientConnection::IsConnected() const
    {
        ISteamNetworkingSockets* pInterface = SteamNetworkingSockets();
        KRG_ASSERT( pInterface != nullptr );

        SteamNetConnectionInfo_t info;
        pInterface->GetConnectionInfo( m_connectionHandle, &info );

        return info.m_eState == k_ESteamNetworkingConnectionState_Connected;
    }

    bool ClientConnection::IsConnecting() const
    {
        ISteamNetworkingSockets* pInterface = SteamNetworkingSockets();
        KRG_ASSERT( pInterface != nullptr );

        SteamNetConnectionInfo_t info;
        pInterface->GetConnectionInfo( m_connectionHandle, &info );

        return info.m_eState == k_ESteamNetworkingConnectionState_Connecting;
    }

    //-------------------------------------------------------------------------

    bool NetworkSystem::Initialize()
    {
        KRG_ASSERT( g_pNetworkState == nullptr );

        SteamDatagramErrMsg errMsg;
        if ( !GameNetworkingSockets_Init( nullptr, errMsg ) )
        {
            KRG_LOG_ERROR( "Network", "Failed to initialize network system: %s", errMsg );
            return false;
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        SteamNetworkingUtils()->SetDebugOutputFunction( k_ESteamNetworkingSocketsDebugOutputType_Msg, NetworkCallbackHandler::NetworkDebugOutputFunction );
        #endif

        g_pNetworkState = KRG::New<NetworkState>();
        g_pNetworkState->m_logTimeZero = SteamNetworkingUtils()->GetLocalTimestamp();
        return true;
    }

    void NetworkSystem::Shutdown()
    {
        if ( g_pNetworkState != nullptr )
        {
            // Give connections time to finish up.  This is an application layer protocol here, it's not TCP.  Note that if you have an application and you need to be
            // more sure about cleanup, you won't be able to do this.  You will need to send a message and then either wait for the peer to close the connection, or
            // you can pool the connection to see if any reliable data is pending.
            Threading::Sleep( 250 );

            GameNetworkingSockets_Kill();
            KRG::Delete( g_pNetworkState );
        }
    }

    void NetworkSystem::Update()
    {
        auto pInterface = SteamNetworkingSockets();
        KRG_ASSERT( pInterface != nullptr );

        // Handle connection changes
        //-------------------------------------------------------------------------

        pInterface->RunCallbacks();

        // Process Received Messages
        //-------------------------------------------------------------------------

        for ( auto pServerConnection : g_pNetworkState->m_serverConnections )
        {
            ISteamNetworkingMessage *pIncomingMsg = nullptr;
            int32 const numMsgs = pInterface->ReceiveMessagesOnPollGroup( pServerConnection->m_pollingGroupHandle, &pIncomingMsg, 1 );

            if ( numMsgs == 0 )
            {
                continue;
            }

            if ( numMsgs < 0 )
            {
                KRG_LOG_FATAL_ERROR( "Network", "Error checking for messages" );
            }

            pServerConnection->ProcessMessage( pIncomingMsg->m_conn, pIncomingMsg->m_pData, pIncomingMsg->m_cbSize );
            pIncomingMsg->Release();
        }

        for ( auto pClientConnection : g_pNetworkState->m_clientConnections )
        {
            ISteamNetworkingMessage *pIncomingMsg = nullptr;
            int32 const numMsgs = pInterface->ReceiveMessagesOnConnection( pClientConnection->m_connectionHandle, &pIncomingMsg, 1 );

            if ( numMsgs == 0 )
            {
                continue;
            }

            if ( numMsgs < 0 )
            {
                KRG_LOG_FATAL_ERROR( "Network", "Error checking for messages" );
            }

            pClientConnection->ProcessMessage( pIncomingMsg->m_pData, pIncomingMsg->m_cbSize );
            pIncomingMsg->Release();
        }

        // Send Outgoing Messages
        //-------------------------------------------------------------------------

        for ( auto pServerConnection : g_pNetworkState->m_serverConnections )
        {
            auto ServerSendFunction = [pInterface, pServerConnection] ( uint32 connectionHandle, void* pData, uint32 size )
            {
                pInterface->SendMessageToConnection( connectionHandle, pData, size, k_nSteamNetworkingSend_Reliable, nullptr );
            };

            pServerConnection->SendMessages( ServerSendFunction );
        }

        for ( auto pClientConnection : g_pNetworkState->m_clientConnections )
        {
            auto ClientSendFunction = [pInterface, pClientConnection] ( void* pData, uint32 size )
            {
                pInterface->SendMessageToConnection( pClientConnection->m_connectionHandle, pData, size, k_nSteamNetworkingSend_Reliable, nullptr );
            };

            pClientConnection->SendMessages( ClientSendFunction );
        }
    }

    //-------------------------------------------------------------------------

    bool NetworkSystem::StartServerConnection( ServerConnection* pServerConnection, uint16 portNumber )
    {
        KRG_ASSERT( pServerConnection != nullptr && !pServerConnection->IsRunning() );
        
        ISteamNetworkingSockets* pInterface = SteamNetworkingSockets();
        KRG_ASSERT( pInterface != nullptr );

        //-------------------------------------------------------------------------

        SteamNetworkingIPAddr serverLocalAddr;
        serverLocalAddr.Clear();
        serverLocalAddr.m_port = portNumber;

        SteamNetworkingConfigValue_t opt;
        opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*) NetworkCallbackHandler::ServerNetConnectionStatusChangedCallback );
        pServerConnection->m_socketHandle = pInterface->CreateListenSocketIP( serverLocalAddr, 1, &opt );
        if ( pServerConnection->m_socketHandle == k_HSteamListenSocket_Invalid )
        {
            KRG_LOG_ERROR( "Network", "Failed to listen on port %d", portNumber );
            return false;
        }

        pServerConnection->m_pollingGroupHandle = pInterface->CreatePollGroup();
        if ( pServerConnection->m_pollingGroupHandle == k_HSteamNetPollGroup_Invalid )
        {
            KRG_LOG_ERROR( "Network", "Failed to listen on port %d", portNumber );
            return false;
        }

        //-------------------------------------------------------------------------

        g_pNetworkState->m_serverConnections.emplace_back( pServerConnection );

        return true;
    }

    void NetworkSystem::StopServerConnection( ServerConnection* pServerConnection )
    {
        KRG_ASSERT( pServerConnection != nullptr );

        ISteamNetworkingSockets* pInterface = SteamNetworkingSockets();
        KRG_ASSERT( pInterface != nullptr );

        for ( auto const& it : pServerConnection->m_connectedClients )
        {
            pInterface->CloseConnection( it, 0, "Server Shutdown", true );
        }
        pServerConnection->m_connectedClients.clear();

        //-------------------------------------------------------------------------

        if ( pServerConnection->m_pollingGroupHandle != k_HSteamNetPollGroup_Invalid )
        {
            pInterface->DestroyPollGroup( pServerConnection->m_pollingGroupHandle );
            pServerConnection->m_pollingGroupHandle = k_HSteamNetPollGroup_Invalid;
        }

        if ( pServerConnection->m_socketHandle != k_HSteamListenSocket_Invalid )
        {
            pInterface->CloseListenSocket( pServerConnection->m_socketHandle );
            pServerConnection->m_socketHandle = k_HSteamListenSocket_Invalid;
        }

        //-------------------------------------------------------------------------

        g_pNetworkState->m_serverConnections.erase_first_unsorted( pServerConnection );
    }

    //-------------------------------------------------------------------------

    bool NetworkSystem::StartClientConnection( ClientConnection* pClientConnection, char const* pAddress )
    {
        KRG_ASSERT( pClientConnection != nullptr && !pClientConnection->IsConnected() );
        KRG_ASSERT( pAddress != nullptr );

        ISteamNetworkingSockets* pInterface = SteamNetworkingSockets();
        KRG_ASSERT( pInterface != nullptr );

        //-------------------------------------------------------------------------

        SteamNetworkingIPAddr serverAddr;
        if ( !serverAddr.ParseString( pAddress ) )
        {
            KRG_LOG_ERROR( "Network", "Invalid client IP address provided: %s", pAddress );
            return false;
        }

        if ( serverAddr.m_port == 0 )
        {
            KRG_LOG_ERROR( "Network", "No port for client address provided: %s", pAddress );
            return false;
        }

        pClientConnection->m_address = pAddress;

        //-------------------------------------------------------------------------

        SteamNetworkingConfigValue_t opt;
        opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*) NetworkCallbackHandler::ClientNetConnectionStatusChangedCallback );
        pClientConnection->m_connectionHandle = pInterface->ConnectByIPAddress( serverAddr, 1, &opt );
        if ( pClientConnection->m_connectionHandle == k_HSteamNetConnection_Invalid )
        {
            KRG_LOG_ERROR( "Network", "Failed to create connection" );
            return false;
        }

        g_pNetworkState->m_clientConnections.emplace_back( pClientConnection );

        return false;
    }

    void NetworkSystem::StopClientConnection( ClientConnection* pClientConnection )
    {
        KRG_ASSERT( pClientConnection != nullptr );

        ISteamNetworkingSockets* pInterface = SteamNetworkingSockets();
        KRG_ASSERT( pInterface != nullptr );

        if ( pClientConnection->m_connectionHandle != k_HSteamNetConnection_Invalid )
        {
            pInterface->CloseConnection( pClientConnection->m_connectionHandle, k_ESteamNetConnectionEnd_App_Generic, "Closing Connection", false );
            pClientConnection->m_connectionHandle = k_HSteamNetConnection_Invalid;
        }

        pClientConnection->m_address.clear();

        //-------------------------------------------------------------------------

        g_pNetworkState->m_clientConnections.erase_first_unsorted( pClientConnection );
    }
}