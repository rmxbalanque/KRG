#pragma once
#include "System/Network/_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Memory/MemoryStreamHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Network::IPC
{
    enum class MessageType
    {
        Generic = 0,
        NumMessageTypes
    };

    //-------------------------------------------------------------------------

    typedef int32 MessageID;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_NETWORK_API Message
    {
        friend class Server;
        friend class Client;
        constexpr static MessageID const InvalidID = 0xFFFFFFFF;

    public:

        Message() { Initialize( InvalidID ); }
        explicit Message( MessageID ID ) { Initialize( ID ); }
        explicit Message( MessageID ID, void* pData, size_t dataSize ) { Initialize( ID, pData, dataSize ); }

        explicit Message( Message&& msg ) { m_clientConnectionID = msg.m_clientConnectionID; m_data.swap( msg.m_data ); }
        explicit Message( Message const& msg ) { m_clientConnectionID = msg.m_clientConnectionID; m_data = msg.m_data; }

        template<typename T>
        Message( MessageID ID, T&& typeToSerialize )
        {
            std::stringstream stream;
            {
                cereal::JSONOutputArchive archive( stream );
                archive << typeToSerialize;
            }

            Initialize( ID, (void*) stream.str().c_str(), (size_t) stream.str().length() );
        }

        //-------------------------------------------------------------------------

        Message& operator=( Message const& msg )
        {
            m_data = msg.m_data;
            return *this;
        }

        Message& operator=( Message&& msg ) noexcept
        {
            m_data.swap( msg.m_data );
            return *this;
        }

        //-------------------------------------------------------------------------

        inline uint32 const& GetClientConnectionID() const { return m_clientConnectionID; }
        inline void SetClientConnectionID( uint32 clientConnectionID ) { m_clientConnectionID = clientConnectionID; }
        inline MessageID GetMessageID() const { return *(MessageID*) m_data.data(); }

        inline bool IsValid() const { return GetMessageID() != InvalidID; }
        inline bool HasPayload() const { return GetPayloadDataSize() > sizeof( MessageID ); }
        inline Byte const* GetPayloadData() const { return m_data.data() + sizeof( MessageID ); }
        inline size_t GetPayloadDataSize() const { return m_data.size() - sizeof( MessageID ); }

        // Serialization functions
        template<typename T>
        inline void SetData( MessageID msgID, T const& typeToSerialize )
        {
            std::stringstream stream;
            {
                cereal::JSONOutputArchive archive( stream );
                archive << typeToSerialize;
            }

            Initialize( msgID, (void*) stream.str().c_str(), (size_t) stream.str().length() );
        }

        // Serialization functions
        template<typename T>
        inline T GetData() const
        {
            T outType;
            MemoryStreamView stream( GetPayloadData(), GetPayloadDataSize() );
            cereal::JSONInputArchive archive( stream );
            archive >> outType;
            return outType;
        }

    private:

        void Initialize( MessageID messageID );
        void Initialize( void* pData, size_t dataSize );
        void Initialize( MessageID messageID, void* pData, size_t dataSize );

    private:

        TInlineVector<Byte, sizeof( MessageID )>    m_data;
        uint32                                      m_clientConnectionID = 0;
    };
}