#include "Editor_Model.h"
#include "RenderingSystem.h"
#include "MapEditor/Workspace_MapEditor.h"
#include "MapEditor/Workspace_GamePreviewer.h"
#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/Entity/EntityMapDescriptor.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "Engine/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EditorModel::~EditorModel()
    {
        KRG_ASSERT( m_workspaces.empty() );
    }

    void EditorModel::Initialize( UpdateContext const& context )
    {
        auto pSettingsRegistry = context.GetSystem<SettingsRegistry>();
        KRG_ASSERT( pSettingsRegistry != nullptr );
        
        auto pResourceSettings = pSettingsRegistry->GetSettings<Resource::Settings>();
        KRG_ASSERT( pResourceSettings != nullptr );

        //-------------------------------------------------------------------------

        m_editorContext.m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
        m_editorContext.m_pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
        m_pWorldManager = context.GetSystem<EntityWorldManager>();
        m_pRenderingSystem = context.GetSystem<Render::RenderingSystem>();

        m_resourceDB.Initialize( m_editorContext.m_pTypeRegistry, pResourceSettings->m_rawResourcePath, pResourceSettings->m_compiledResourcePath );
        m_editorContext.m_pResourceDatabase = &m_resourceDB;

        // Create map editor workspace
        //-------------------------------------------------------------------------

        // Destroy the default created game world
        m_pWorldManager->DestroyWorld( m_pWorldManager->GetWorlds()[0] );

        // Create the map editor world
        auto pMapEditorWorld = m_pWorldManager->CreateWorld( EntityWorldType::Editor );
        m_pRenderingSystem->CreateCustomRenderTargetForViewport( pMapEditorWorld->GetViewport() );
        m_pMapEditor = KRG::New<EntityModel::EntityMapEditor>( m_editorContext, pMapEditorWorld );
        m_pMapEditor->Initialize( context );
        m_workspaces.emplace_back( m_pMapEditor );
    }

    void EditorModel::Shutdown( UpdateContext const& context )
    {
        for ( auto& pOpenWorkspace : m_workspaces )
        {
            if ( pOpenWorkspace->IsDirty() )
            {
                InlineString<255> const messageTitle( InlineString<255>::CtorSprintf(), "Unsaved Changes for %s", pOpenWorkspace->GetDisplayName() );
                auto messageDialog = pfd::message( messageTitle.c_str(), "You have unsaved changes!\nDo you wish to save these changes before closing?", pfd::choice::yes_no_cancel );
                switch ( messageDialog.result() )
                {
                    case pfd::button::yes:
                    {
                        pOpenWorkspace->Save();
                    }
                    break;
                }
            }

            pOpenWorkspace->Shutdown( context );
            KRG::Delete( pOpenWorkspace );
        }

        m_workspaces.clear();

        //-------------------------------------------------------------------------

        m_pWorldManager = nullptr;
        m_pRenderingSystem = nullptr;
        m_editorContext.m_pResourceSystem = nullptr;
        m_editorContext.m_pTypeRegistry = nullptr;
        m_editorContext.m_pResourceDatabase = nullptr;

        m_resourceDB.Shutdown();
    }

    void EditorModel::Update( UpdateContext const& context )
    {
        // Update the resource database
        m_resourceDB.Update();

        // Destroy all required workspaces
        // We needed to defer this to the start of the update since we may have references resources that we might unload (i.e. textures)
        for ( auto pWorkspaceToDestroy : m_workspaceDestructionRequests )
        {
            DestroyWorkspace( context, pWorkspaceToDestroy );
        }
        m_workspaceDestructionRequests.clear();

        // Create all workspaces
        for ( auto const& resourceID : m_workspaceCreationRequests )
        {
            TryCreateWorkspace( context, resourceID );
        }
        m_workspaceCreationRequests.clear();
    }

    //-------------------------------------------------------------------------

    bool EditorModel::TryCreateWorkspace( UpdateContext const& context, ResourceID const& resourceID )
    {
        ResourceTypeID const resourceTypeID = resourceID.GetResourceTypeID();

        // Handle maps explicitly
        //-------------------------------------------------------------------------

        if ( resourceTypeID == EntityModel::EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            m_pMapEditor->LoadMap( resourceID );
            //FocusWorkspace( *foundWorkspaceIter );
            return true;
        }

        // Other resource types
        //-------------------------------------------------------------------------

        auto pExistingWorkspace = FindResourceWorkspace( resourceID );
        if ( pExistingWorkspace == nullptr )
        {
            // Create preview world
            auto pPreviewWorld = m_pWorldManager->CreateWorld( EntityWorldType::Editor );
            pPreviewWorld->LoadMap( ResourcePath( "data://Editor/EditorMap.map" ) );
            m_pRenderingSystem->CreateCustomRenderTargetForViewport( pPreviewWorld->GetViewport() );

            // Create workspace
            auto pCreatedWorkspace = ResourceWorkspaceFactory::TryCreateWorkspace( m_editorContext, pPreviewWorld, resourceID );
            KRG_ASSERT( pCreatedWorkspace != nullptr );
            pCreatedWorkspace->Initialize( context );
            m_workspaces.emplace_back( pCreatedWorkspace );
        }
        else
        {
            //FocusWorkspace( *foundWorkspaceIter );
        }

        return true;
    }

    void EditorModel::DestroyWorkspace( UpdateContext const& context, EditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );
        KRG_ASSERT( m_pMapEditor != pWorkspace );

        auto foundWorkspaceIter = eastl::find( m_workspaces.begin(), m_workspaces.end(), pWorkspace );
        KRG_ASSERT( foundWorkspaceIter != m_workspaces.end() );

        if ( pWorkspace->IsDirty() )
        {
            auto messageDialog = pfd::message( "Unsaved Changes", "You have unsaved changes!\nDo you wish to save these changes before closing?", pfd::choice::yes_no_cancel );
            switch ( messageDialog.result() )
            {
                case pfd::button::yes:
                {
                    if ( !pWorkspace->Save() )
                    {
                        return;
                    }
                }
                break;

                case pfd::button::cancel:
                {
                    return;
                }
                break;
            }
        }

        //-------------------------------------------------------------------------

        bool const isGamePreviewerWorkspace = m_pGamePreviewer == pWorkspace;

        // Destroy the custom viewport render target
        auto pPreviewWorld = pWorkspace->GetWorld();
        m_pRenderingSystem->DestroyCustomRenderTargetForViewport( pPreviewWorld->GetViewport() );

        // Destroy workspace
        pWorkspace->Shutdown( context );
        KRG::Delete( pWorkspace );
        m_workspaces.erase( foundWorkspaceIter );

        // Clear the game previewer workspace ptr if we just destroyed it
        if ( isGamePreviewerWorkspace )
        {
            m_pMapEditor->OnGamePreviewEnded();
            m_pGamePreviewer = nullptr;
        }

        // Destroy preview world
        m_pWorldManager->DestroyWorld( pPreviewWorld );
    }

    bool EditorModel::HasDescriptorForResourceType( ResourceTypeID resourceTypeID ) const
    {
        if ( resourceTypeID == EntityModel::EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            return false;
        }

        return m_editorContext.m_pTypeRegistry->IsRegisteredResourceType( resourceTypeID );
    }

    bool EditorModel::IsMapEditorWorkspace( EditorWorkspace const* pWorkspace ) const
    {
        return m_pMapEditor == pWorkspace;
    }

    char const* EditorModel::GetMapEditorWindowName() const
    {
        return m_pMapEditor->GetWorkspaceWindowID();
    }

    bool EditorModel::IsGamePreviewWorkspace( EditorWorkspace const* pWorkspace ) const
    {
        return m_pGamePreviewer == pWorkspace;
    }

    void* EditorModel::GetViewportTextureForWorkspace( EditorWorkspace* pWorkspace ) const
    {
        KRG_ASSERT( pWorkspace != nullptr );
        auto pWorld = pWorkspace->GetWorld();
        return (void*) &m_pRenderingSystem->GetRenderTargetTextureForViewport( pWorld->GetViewport() );
    }

    EditorWorkspace* EditorModel::FindResourceWorkspace( ResourceID const& resourceID ) const
    {
        KRG_ASSERT( resourceID.IsValid() );
        uint32 const resourcePathID = resourceID.GetResourcePath().GetID();

        auto foundWorkspaceIter = eastl::find( m_workspaces.begin(), m_workspaces.end(), resourcePathID, [] ( EditorWorkspace* const& pExistingWorkspace, uint32 ID ) { return pExistingWorkspace->GetID() == ID; } );
        if ( foundWorkspaceIter != m_workspaces.end() )
        {
            return *foundWorkspaceIter;
        }

        return nullptr;
    }

    EditorWorkspace* EditorModel::FindResourceWorkspace( uint32 workspaceID ) const
    {
        auto foundWorkspaceIter = eastl::find( m_workspaces.begin(), m_workspaces.end(), workspaceID, [] ( EditorWorkspace* const& pExistingWorkspace, uint32 ID ) { return pExistingWorkspace->GetID() == ID; } );
        if ( foundWorkspaceIter != m_workspaces.end() )
        {
            return *foundWorkspaceIter;
        }

        return nullptr;
    }

    //-------------------------------------------------------------------------

    bool EditorModel::IsGamePreviewAllowed() const
    {
        return m_pMapEditor->HasLoadedMap();
    }

    void EditorModel::StartGamePreview( UpdateContext const& context )
    {
        KRG_ASSERT( !IsGamePreviewRunning() );

        auto pPreviewWorld = m_pWorldManager->CreateWorld( EntityWorldType::Game );
        m_pRenderingSystem->CreateCustomRenderTargetForViewport( pPreviewWorld->GetViewport() );
        m_pGamePreviewer = KRG::New<GamePreviewer>( m_editorContext, pPreviewWorld );
        m_pGamePreviewer->Initialize( context );
        m_pGamePreviewer->LoadMapToPreview( m_pMapEditor->GetLoadedMap() );
        m_workspaces.emplace_back( m_pGamePreviewer );

        m_pMapEditor->OnGamePreviewStarted();
    }

    void EditorModel::StopGamePreview( UpdateContext const& context )
    {
        KRG_ASSERT( IsGamePreviewRunning() );
        DestroyWorkspace( context, m_pGamePreviewer );
        m_pGamePreviewer = nullptr;
    }
}