#include "IPCMessage.h"

//-------------------------------------------------------------------------

namespace KRG::Network::IPC
{
    void Message::Initialize( MessageID messageID )
    {
        m_data.resize( sizeof( MessageID ) );
        *(MessageID*) m_data.data() = messageID;
    }

    void Message::Initialize( void* pData, size_t dataSize )
    {
        KRG_ASSERT( pData != nullptr && dataSize >= sizeof( MessageID ) );
        m_data.resize( dataSize );
        memcpy( m_data.data(), pData, dataSize );
        KRG_ASSERT( GetMessageID() != InvalidID );
    }

    void Message::Initialize( MessageID messageID, void* pData, size_t dataSize )
    {
        KRG_ASSERT( messageID != InvalidID && pData != nullptr && dataSize > 0 );
        m_data.resize( dataSize + sizeof( MessageID ) );
        *(MessageID*) m_data.data() = messageID;
        memcpy( m_data.data() + sizeof( MessageID ), pData, dataSize );
    }
}