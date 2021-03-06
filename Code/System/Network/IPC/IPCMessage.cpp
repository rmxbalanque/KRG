#include "IPCMessage.h"
#include "System/Core/Math/Random.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Network
    {
        namespace IPC
        {
            KRG::Network::IPC::ClientID ClientID::Generate()
            {
                ClientID ID;
                auto pData = reinterpret_cast<uint32*>( &ID.m_ID );
                pData[0] = Math::GetRandomInt();
                pData[1] = Math::GetRandomInt();
                return ID;
            }

            //-------------------------------------------------------------------------

            void Message::Initialize( MessageID messageID )
            {
                m_data.resize( sizeof( MessageID ) );
                *( MessageID*) m_data.data() = messageID;
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
                *( MessageID*) m_data.data() = messageID;
                memcpy( m_data.data() + sizeof( MessageID ), pData, dataSize );
            }
        }
    }
}