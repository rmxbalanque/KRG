#include "EntityWorldManager.h"
#include "EntityWorld.h"
#include "Debug/EntityWorldDebugView.h"
#include "Engine/Core/Camera/CameraWorldSystem.h"
#include "Engine/Core/Camera/CameraComponent.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityWorldManager::~EntityWorldManager()
    {
        KRG_ASSERT( m_worlds.empty() && m_worldSystemTypeInfos.empty() );
        KRG_ASSERT( !m_createNewWorldEvent.HasBoundUsers() );
    }

    void EntityWorldManager::Initialize( SystemRegistry const& systemsRegistry )
    {
        m_pSystemsRegistry = &systemsRegistry;

        //-------------------------------------------------------------------------

        auto pTypeRegistry = systemsRegistry.GetSystem<TypeSystem::TypeRegistry>();
        KRG_ASSERT( pTypeRegistry != nullptr );
        m_worldSystemTypeInfos = pTypeRegistry->GetAllDerivedTypes( IWorldEntitySystem::GetStaticTypeID(), false, false );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        m_debugViewTypeInfos = pTypeRegistry->GetAllDerivedTypes( EntityWorldDebugView::GetStaticTypeID(), false, false );
        #endif

        // Create the primary world
        //-------------------------------------------------------------------------

        CreateWorld();
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

    EntityWorld* EntityWorldManager::CreateWorld()
    {
        KRG_ASSERT( m_pSystemsRegistry != nullptr );

        //-------------------------------------------------------------------------

        auto pNewWorld = KRG::New<EntityWorld>();
        pNewWorld->Initialize( *m_pSystemsRegistry, m_worldSystemTypeInfos );
        m_createNewWorldEvent.Execute( pNewWorld );
        m_worlds.emplace_back( pNewWorld );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        pNewWorld->InitializeDebugViews( *m_pSystemsRegistry, m_debugViewTypeInfos );
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

            //-------------------------------------------------------------------------

            pWorld->Update( context );

            // Update world view
            //-------------------------------------------------------------------------
            // We explicitly reflect the camera at the end of the pre-physics stage as we assume it has been updated at that point

            if ( context.GetUpdateStage() == UpdateStage::PrePhysics && pWorld->GetViewport() != nullptr )
            {
                auto pViewport = pWorld->GetViewport();
                auto pCameraSystem = pWorld->GetWorldSystem<CameraWorldSystem>();
                if ( pCameraSystem->HasActiveCamera() )
                {
                    auto pActiveCamera = pCameraSystem->GetActiveCamera();

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