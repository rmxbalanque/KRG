#ifndef KRG_CONFIGURATION_FINAL
#include "NetworkResourceProvider.h"
#include "System/Resource/ResourceHeader.h"
#include "System/Resource/ResourceRequest.h"
#include "System/Core/ThirdParty/cereal/archives/json.hpp"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Threading/Threading.h"
#include <sstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        NetworkResourceProvider::NetworkResourceProvider( char const* pAddress, uint32 port )
            : ResourceProvider()
         {
            KRG_ASSERT( pAddress != nullptr && port < 9999 );
            m_address.sprintf( "%s:%d", pAddress, port );
        }

        bool NetworkResourceProvider::IsReady() const
        {
            return m_networkClient.IsConnected();
        }

        bool NetworkResourceProvider::Initialize()
        {
            m_networkClient.TryConnect( m_address.c_str() );

            if ( !m_networkClient.IsConnected() )
            {
                int32 numRetries = 25;
                while ( numRetries > 0 )
                {
                    KRG_LOG_ERROR( "Resource", "Couldnt connect to resource server at %s - retrying %d / 100", m_networkClient.GetAddress().c_str(), 100 - numRetries );
                    if ( m_networkClient.TryReconnect() )
                    {
                        break;
                    }
                    Threading::Sleep( 150.0f );
                    numRetries--;
                }
            }

            if ( !m_networkClient.IsConnected() )
            {
                KRG_LOG_ERROR( "Resource", "Failed to connect to resource server at %s", m_networkClient.GetAddress().c_str() );
                return false;
            }

            return true;
        }

        void NetworkResourceProvider::Update()
        {
            KRG_PROFILE_FUNCTION_RESOURCE();

            KRG_ASSERT( m_networkClient.IsConnected() );

            // Check for any network messages
            //-------------------------------------------------------------------------

            Network::IPC::Message message;
            if ( m_networkClient.WaitForMessage( message ) )
            {
                switch ( (NetworkMessageID) message.GetMessageID() )
                {
                    case NetworkMessageID::ResourceRequestComplete :
                    {
                        m_serverReponses.emplace_back( message.GetData<NetworkResourceResponse>() );
                    }
                    break;

                    case NetworkMessageID::ResourceUpdated :
                    {
                        NetworkResourceResponse response = message.GetData<NetworkResourceResponse>();
                        m_resourceExternalUpdateEvent.Execute( response.m_resourceID );
                    }
                    break;
                };
            }

            // Send all requests and keep-alive messages
            //-------------------------------------------------------------------------

            Network::IPC::Message messageToSend;
            while( m_messagesToSend.try_dequeue( messageToSend ) )
            {
                m_networkClient.SendMessageToServer( messageToSend );
            }

            if ( m_keepAliveTimer.GetElapsedTimeSeconds() > Seconds( 5.0f ) )
            {
                Network::IPC::Message const keepAliveMessage( (int32) NetworkMessageID::KeepAlive );
                m_networkClient.SendMessageToServer( keepAliveMessage );
                m_keepAliveTimer.Reset();
            }

            // Process all responses
            //-------------------------------------------------------------------------

            for ( auto& response : m_serverReponses )
            {
                auto predicate = [] ( ResourceRequest* pRequest, ResourceID const& resourceID ) { return pRequest->GetResourceID() == resourceID; };
                auto foundIter = VectorFind( m_requests, response.m_resourceID, predicate );

                // This might have been a canceled request
                if ( foundIter == m_requests.end() )
                {
                    continue;
                }

                ResourceRequest* pFoundRequest = static_cast<ResourceRequest*>( *foundIter );
                KRG_ASSERT( pFoundRequest->IsValid() );

                // Ignore any responses for requests that may have been canceled or are unloading
                if ( pFoundRequest->GetLoadingStatus() != LoadingStatus::Loading )
                {
                   continue;
                }

                // If the request has a filepath set, the compilation was a success
                pFoundRequest->OnRawResourceRequestComplete( response.m_filePath );
                FinalizeRequest( pFoundRequest );
            }

            m_serverReponses.clear();
        }

        void NetworkResourceProvider::RequestResourceInternal( ResourceRequest* pRequest )
        {
            m_messagesToSend.enqueue( Network::IPC::Message( (int32) NetworkMessageID::RequestResource, NetworkResourceRequest( pRequest->GetResourceID() ) ) );
        }

        void NetworkResourceProvider::CancelRequestInternal( ResourceRequest* pRequest )
        {
            // Do nothing
        }
    }
}
#endif