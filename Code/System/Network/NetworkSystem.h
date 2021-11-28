#pragma once
#include "_Module\API.h"
#include "System\Core\Types\Containers.h"
#include "System\Core\Types\Function.h"
#include "System\Core\Types\String.h"

//-------------------------------------------------------------------------

namespace KRG::Network
{
    class NetworkSystem;

    //-------------------------------------------------------------------------

    using AddressString = InlineString<30>;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_NETWORK_API ServerConnection
    {
        friend NetworkSystem;
        friend struct NetworkCallbackHandler;

    public:

        using ClientConnectionID = uint32;

        struct ClientInfo
        {
            ClientInfo( ClientConnectionID ID, AddressString const& address )
                : m_ID( ID )
                , m_address( address )
            {
                KRG_ASSERT( m_ID != 0 && !m_address.empty() );
            }

            ClientConnectionID      m_ID;
            AddressString           m_address;
        };

    public:

        virtual ~ServerConnection();

        // Server Info
        //-------------------------------------------------------------------------

        inline bool IsRunning() const { return m_socketHandle != 0 && m_pollingGroupHandle != 0; }
        inline uint32 GetSocketHandle() const { return m_socketHandle; }

        // Client info
        //-------------------------------------------------------------------------

        inline int32 GetNumConnectedClients() const { return (int32) m_connectedClients.size(); }
        inline TVector<ClientInfo> const& GetConnectedClients() const { return m_connectedClients; }

        inline ClientInfo const& GetConnectedClientInfo( int32 clientIdx ) const 
        {
            KRG_ASSERT( clientIdx >= 0 && clientIdx < m_connectedClients.size() );
            return m_connectedClients[clientIdx]; 
        }

        inline bool HasConnectedClient( ClientConnectionID clientID ) const 
        {
            auto const SearchPredicate = [] ( ClientInfo const& info, ClientConnectionID const& clientID ) { return info.m_ID == clientID; };
            return eastl::find( m_connectedClients.begin(), m_connectedClients.end(), clientID, SearchPredicate ) != m_connectedClients.end();
        }

        // Messages
        //-------------------------------------------------------------------------

        virtual void ProcessMessage( uint32 connectionID, void* pData, size_t size ) = 0;
        virtual void SendMessages( TFunction<void( ClientConnectionID, void*, uint32 )> const& sendFunction ) = 0;

    private:

        bool TryStartConnection( uint16 portNumber );
        void CloseConnection();

        void AddConnectedClient( ClientConnectionID clientID, AddressString const& clientAddress );
        void RemoveConnectedClient( ClientConnectionID clientID );

    protected:

        uint32                                          m_socketHandle = 0;
        uint32                                          m_pollingGroupHandle = 0;
        TVector<ClientInfo>                             m_connectedClients;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_NETWORK_API ClientConnection
    {
        friend NetworkSystem;
        friend struct NetworkCallbackHandler;

    public:

        enum class Status
        {
            Disconnected,
            Connecting,
            Connected,
            Reconnecting,
            ConnectionFailed
        };

    public:

        virtual ~ClientConnection();

        inline bool IsConnected() const { return m_status == Status::Connected; }
        inline bool IsConnecting() const { return m_status == Status::Connecting || m_status == Status::Reconnecting; };
        inline bool HasConnectionFailed() const { return m_status == Status::ConnectionFailed; }
        inline bool IsDisconnected() const { return m_status == Status::Disconnected; }

        inline uint32 const& GetClientConnectionID() const { return m_connectionHandle; }
        inline AddressString const& GetAddress() const { return m_address; }

        virtual void ProcessMessage( void* pData, size_t size ) = 0;

        virtual void SendMessages( TFunction<void( void*, uint32 )> const& sendFunction ) = 0;

    private:

        bool TryStartConnection();
        void CloseConnection();
        void Update();

    private:

        AddressString                                   m_address;
        uint32                                          m_connectionHandle = 0;
        uint32                                          m_reconnectionAttemptsRemaining = 5;
        Status                                          m_status = Status::Disconnected;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_NETWORK_API NetworkSystem final
    {
        friend struct NetworkCallbackHandler;

    public:

        static bool Initialize();
        static void Shutdown();
        static void Update();

        // Start a server connection on the specified port
        static bool StartServerConnection( ServerConnection* pServerConnection, uint16 portNumber );
        static void StopServerConnection( ServerConnection* pServerConnection );

        // Start a client connection to a specified address. Address format: "XXX.XXX.XXX.XXX:Port"
        static bool StartClientConnection( ClientConnection* pClientConnection, char const* pAddress );
        static void StopClientConnection( ClientConnection* pClientConnection );

    private:

        NetworkSystem() = delete;
    };
}