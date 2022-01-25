#include "EntityWorldManager.h"
#include "EntityWorld.h"
#include "EntityWorldDebugView.h"
#include "Engine/Core/Systems/WorldSystem_PlayerManager.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Core/Systems/SystemRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityWorldManager::~EntityWorldManager()
    {
        KRG_ASSERT( m_worlds.empty() && m_worldSystemTypeInfos.empty() );
    }

    void EntityWorldManager::Initialize( SystemRegistry const& systemsRegistry )
    {
        m_pSystemsRegistry = &systemsRegistry;

        //-------------------------------------------------------------------------

        auto pTypeRegistry = systemsRegistry.GetSystem<TypeSystem::TypeRegistry>();
        KRG_ASSERT( pTypeRegistry != nullptr );
        m_worldSystemTypeInfos = pTypeRegistry->GetAllDerivedTypes( IWorldEntitySystem::GetStaticTypeID(), false, false, true );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_debugViewTypeInfos = pTypeRegistry->GetAllDerivedTypes( EntityWorldDebugView::GetStaticTypeID(), false, false, true );
        #endif

        // Create a game world
        //-------------------------------------------------------------------------

        CreateWorld( EntityWorldType::Game );
    }

    void EntityWorldManager::Shutdown()
    {
        for ( auto& pWorld : m_worlds )
        {
            #if KRG_DEVELOPMENT_TOOLS
            pWorld->ShutdownDebugViews();
            #endif

            pWorld->Shutdown();
            KRG::Delete( pWorld );
        }
        m_worlds.clear();

        //-------------------------------------------------------------------------

        m_worldSystemTypeInfos.clear();
        m_pSystemsRegistry = nullptr;
    }

    //-------------------------------------------------------------------------

    void EntityWorldManager::StartFrame()
    {
        for ( auto& pWorld : m_worlds )
        {
            #if KRG_DEVELOPMENT_TOOLS
            pWorld->ResetDebugDrawingSystem();
            #endif
        }
    }

    void EntityWorldManager::EndFrame()
    {

    }

    //-------------------------------------------------------------------------

    EntityWorld* EntityWorldManager::GetGameWorld()
    {
        for ( auto const& pWorld : m_worlds )
        {
            if ( pWorld->IsGameWorld() )
            {
                return pWorld;
            }
        }

        return nullptr;
    }

    EntityWorld* EntityWorldManager::CreateWorld( EntityWorldType worldType )
    {
        KRG_ASSERT( m_pSystemsRegistry != nullptr );

        //-------------------------------------------------------------------------

        // Only a single game world is allowed
        if ( worldType == EntityWorldType::Game )
        {
            if ( GetGameWorld() != nullptr )
            {
                KRG_UNREACHABLE_CODE();
                return nullptr;
            }
        }

        //-------------------------------------------------------------------------

        auto pNewWorld = KRG::New<EntityWorld>( worldType );
        pNewWorld->Initialize( *m_pSystemsRegistry, m_worldSystemTypeInfos );
        m_worlds.emplace_back( pNewWorld );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        pNewWorld->InitializeDebugViews( *m_pSystemsRegistry, m_debugViewTypeInfos );
        
        if ( !pNewWorld->IsGameWorld() )
        {
            pNewWorld->GetWorldSystem<PlayerManager>()->SetDebugMode( PlayerManager::DebugMode::OnlyDebugCamera );
        }
        #endif

        return pNewWorld;
    }

    void EntityWorldManager::DestroyWorld( EntityWorld* pWorld )
    {
        KRG_ASSERT( Threading::IsMainThread() );

        // Remove world from worlds list
        auto foundWorldIter = eastl::find( m_worlds.begin(), m_worlds.end(), pWorld );
        KRG_ASSERT( foundWorldIter != m_worlds.end() );
        m_worlds.erase( foundWorldIter );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        pWorld->ShutdownDebugViews();
        #endif

        // Shutdown and destroy world
        pWorld->Shutdown();
        KRG::Delete( pWorld );
    }

    //-------------------------------------------------------------------------

    bool EntityWorldManager::IsBusyLoading() const
    {
        for ( auto const& pWorld : m_worlds )
        {
            if ( pWorld->IsBusyLoading() )
            {
                return true;
            }
        }

        return false;
    }

    void EntityWorldManager::UpdateLoading()
    {
        for ( auto const& pWorld : m_worlds )
        {
            pWorld->UpdateLoading();
        }
    }

    void EntityWorldManager::UpdateWorlds( UpdateContext const& context )
    {
        for ( auto const& pWorld : m_worlds )
        {
            if ( pWorld->IsSuspended() )
            {
                continue;
            }

            // Reflect input state
            //-------------------------------------------------------------------------

            if ( context.GetUpdateStage() == UpdateStage::FrameStart )
            {
                auto pPlayerManager = pWorld->GetWorldSystem<PlayerManager>();
                auto pWorldInputState = pWorld->GetInputState();

                if ( pPlayerManager->IsPlayerEnabled() )
                {
                    auto pInputSystem = context.GetSystem<Input::InputSystem>();
                    pInputSystem->ReflectState( context.GetDeltaTime(), pWorld->GetTimeScale(), *pWorldInputState );
                }
                else
                {
                    pWorldInputState->Clear();
                }
            }

            // Run world updates
            //-------------------------------------------------------------------------

            pWorld->Update( context );

            // Update world view
            //-------------------------------------------------------------------------
            // We explicitly reflect the camera at the end of the post-physics stage as we assume it has been updated at that point

            if ( context.GetUpdateStage() == UpdateStage::PostPhysics && pWorld->GetViewport() != nullptr )
            {
                auto pViewport = pWorld->GetViewport();
                auto pPlayerManager = pWorld->GetWorldSystem<PlayerManager>();
                if ( pPlayerManager->HasActiveCamera() )
                {
                    auto pActiveCamera = pPlayerManager->GetActiveCamera();

                    // Update camera view dimensions if needed
                    if ( pViewport->GetDimensions() != pActiveCamera->GetViewVolume().GetViewDimensions() )
                    {
                        pActiveCamera->UpdateViewDimensions( pViewport->GetDimensions() );
                    }

                    // Update world viewport
                    pViewport->SetViewVolume( pActiveCamera->GetViewVolume() );
                }
            }
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void EntityWorldManager::SetPlayerEnabled( EntityWorld* pWorld, bool isPlayerEnabled )
    {
        auto pPlayerManagerSystem = pWorld->GetWorldSystem<PlayerManager>();
        pPlayerManagerSystem->SetPlayerControllerState( isPlayerEnabled );
    }

    void EntityWorldManager::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload )
    {
        for ( auto const& pWorld : m_worlds )
        {
            pWorld->BeginHotReload( usersToReload );
        }
    }

    void EntityWorldManager::EndHotReload()
    {
        for ( auto const& pWorld : m_worlds )
        {
            pWorld->EndHotReload();
        }
    }
    #endif
}