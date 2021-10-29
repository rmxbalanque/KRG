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
        KRG_ASSERT( m_worldRecords.empty() && m_worldSystemTypeInfos.empty() );
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
        for ( auto& worldRecord : m_worldRecords )
        {
            #if KRG_DEVELOPMENT_TOOLS
            worldRecord.m_pWorld->ShutdownDebugViews();
            #endif

            worldRecord.m_pWorld->Shutdown();
            KRG::Delete( worldRecord.m_pWorld );
        }
        m_worldRecords.clear();

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
        m_worldRecords.emplace_back( pNewWorld );

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
        for ( auto const& worldRecord : m_worldRecords )
        {
            if ( worldRecord.m_pWorld->IsBusyLoading() )
            {
                return true;
            }
        }

        return false;
    }

    void EntityWorldManager::UpdateLoading()
    {
        for ( auto const& worldRecord : m_worldRecords )
        {
            worldRecord.m_pWorld->UpdateLoading();
        }
    }

    void EntityWorldManager::UpdateWorlds( UpdateContext const& context )
    {
        for ( auto const& worldRecord : m_worldRecords )
        {
            worldRecord.m_pWorld->Update( context );

            // Update world view
            //-------------------------------------------------------------------------
            // We explicitly reflect the camera at the end of the pre-physics stage as we assume it has been updated at that point

            if ( context.GetUpdateStage() == UpdateStage::PrePhysics && worldRecord.m_pViewport != nullptr )
            {
                auto pCameraSystem = worldRecord.m_pWorld->GetWorldSystem<CameraWorldSystem>();
                if ( pCameraSystem->HasActiveCamera() )
                {
                    auto pActiveCamera = pCameraSystem->GetActiveCamera();

                    // Update camera view dimensions if needed
                    if ( worldRecord.m_pViewport->GetDimensions() != pActiveCamera->GetViewVolume().GetViewDimensions() )
                    {
                        pActiveCamera->UpdateViewDimensions( worldRecord.m_pViewport->GetDimensions() );
                    }

                    // Update world viewport
                    worldRecord.m_pViewport->SetViewVolume( pActiveCamera->GetViewVolume() );
                }
            }
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void EntityWorldManager::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload )
    {
        for ( auto const& worldRecord : m_worldRecords )
        {
            worldRecord.m_pWorld->BeginHotReload( usersToReload );
        }
    }

    void EntityWorldManager::EndHotReload()
    {
        for ( auto const& worldRecord : m_worldRecords )
        {
            worldRecord.m_pWorld->EndHotReload();
        }
    }
    #endif
}