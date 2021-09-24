#ifndef KRG_CONFIGURATION_FINAL
#include "NetworkResourceProvider.h"
#include "System/Resource/ResourceHeader.h"
#include "System/Resource/ResourceRequest.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Core/ThirdParty/cereal/archives/json.hpp"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Threading/Threading.h"
#include <sstream>

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    NetworkResourceProvider::NetworkResourceProvider( Settings const* pSettings )
        : ResourceProvider()
    {
        KRG_ASSERT( pSettings != nullptr );
        KRG_ASSERT( !pSettings->m_resourceServerNetworkAddress.empty() && pSettings->m_resourceServerPort < 9999 );
        m_address.sprintf( "%s:%d", pSettings->m_resourceServerNetworkAddress.c_str(), pSettings->m_resourceServerPort );
    }

    bool NetworkResourceProvider::IsReady() const
    {
        return m_networkClient.IsConnected();
    }

    bool NetworkResourceProvider::Initialize()
    {
        return TryConnectToServer();
    }

    void NetworkResourceProvider::Shutdown()
    {
        Network::NetworkSystem::StopClientConnection( &m_networkClient );
    }

    void NetworkResourceProvider::Update()
    {
        KRG_PROFILE_FUNCTION_RESOURCE();

        // Check connection to resource server
        //-------------------------------------------------------------------------

        if ( !m_networkClient.IsConnected() )
        {
            if ( !TryConnectToServer() )
            {
                KRG_LOG_FATAL_ERROR( "Resource", "Lost connection to resource server" );
                return;
            }
        }

        // Check for any network messages
        //-------------------------------------------------------------------------

        auto ProcessMessageFunction = [this] ( Network::IPC::Message const& message )
        {
            switch ( (NetworkMessageID) message.GetMessageID() )
            {
                case NetworkMessageID::ResourceRequestComplete:
                {
                    m_serverReponses.emplace_back( message.GetData<NetworkResourceResponse>() );
                }
                break;

                case NetworkMessageID::ResourceUpdated:
                {
                    NetworkResourceResponse response = message.GetData<NetworkResourceResponse>();
                    m_resourceExternalUpdateEvent.Execute( response.m_resourceID );
                }
                break;
            };
        };

        m_networkClient.ProcessIncomingMessages( ProcessMessageFunction );

        // Send all requests and keep-alive messages
        //-------------------------------------------------------------------------

        Network::IPC::Message messageToSend;
        while ( m_messagesToSend.try_dequeue( messageToSend ) )
        {
            m_networkClient.SendMessageToServer( eastl::move( messageToSend ) );
        }

        if ( m_keepAliveTimer.GetElapsedTimeSeconds() > Seconds( 5.0f ) )
        {
            m_networkClient.SendMessageToServer( Network::IPC::Message( (int32) NetworkMessageID::KeepAlive ) );
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

    bool NetworkResourceProvider::TryConnectToServer()
    {
        Network::NetworkSystem::StartClientConnection( &m_networkClient, m_address.c_str() );

        if ( m_networkClient.IsConnecting() )
        {
            static constexpr int32 const maxRetries = 10;
            int32 numRetries = 0;
            while ( numRetries < maxRetries )
            {
                KRG_LOG_MESSAGE( "Resource", "Connecting to resource server at %s - retrying %d / %d", m_networkClient.GetAddress().c_str(), numRetries, maxRetries );
                if ( m_networkClient.IsConnected() || !m_networkClient.IsConnecting() )
                {
                    break;
                }
                Threading::Sleep( 500.0f );
                numRetries++;
            }
        }

        if ( !m_networkClient.IsConnected() )
        {
            KRG_LOG_ERROR( "Resource", "Failed to connect to resource server at %s", m_networkClient.GetAddress().c_str() );
            return false;
        }

        return true;
    }
}
#endif