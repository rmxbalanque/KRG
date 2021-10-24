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
        for ( auto pWorld : m_worlds )
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

    void EntityWorldManager::CreateWorld()
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
    }

    void EntityWorldManager::DestroyWorld( EntityWorld* pWorld )
    {
        KRG_UNIMPLEMENTED_FUNCTION();
    }

    //-------------------------------------------------------------------------

    bool EntityWorldManager::IsBusyLoading() const
    {
        for ( auto pWorld : m_worlds )
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
        for ( auto pWorld : m_worlds )
        {
            pWorld->UpdateLoading();
        }
    }

    void EntityWorldManager::UpdateWorlds( UpdateContext const& context )
    {
        for ( auto pWorld : m_worlds )
        {
            pWorld->Update( context );

            // Update world view
            //-------------------------------------------------------------------------
            // We explicitly update the camera at the end of the pre-physics stage

            if ( context.GetUpdateStage() == UpdateStage::PrePhysics )
            {
                auto pCameraSystem = pWorld->GetWorldSystem<CameraWorldSystem>();
                if ( pCameraSystem->HasActiveCamera() )
                {
                    auto pActiveCamera = pCameraSystem->GetActiveCamera();

                    // Update camera view dimensions if needed
                    if ( pWorld->m_viewport.GetDimensions() != pActiveCamera->GetViewVolume().GetViewDimensions() )
                    {
                        pActiveCamera->UpdateViewDimensions( pWorld->m_viewport.GetDimensions() );
                    }

                    // Update world viewpoint
                    pWorld->m_viewport.SetViewVolume( pActiveCamera->GetViewVolume() );
                }
            }
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void EntityWorldManager::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload )
    {
        for ( auto pWorld : m_worlds )
        {
            pWorld->BeginHotReload( usersToReload );
        }
    }

    void EntityWorldManager::EndHotReload()
    {
        for ( auto pWorld : m_worlds )
        {
            pWorld->EndHotReload();
        }
    }
    #endif
}