#include "Editor_Model.h"
#include "RenderingSystem.h"
#include "MapEditor/Workspace_MapEditor.h"
#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/Entity/Map/EntityMapDescriptor.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Update/UpdateContext.h"

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
        m_editorContext.m_sourceResourceDirectory = pResourceSettings->m_rawResourcePath;
        m_editorContext.m_compiledResourceDirectory = pResourceSettings->m_compiledResourcePath;

        //-------------------------------------------------------------------------

        m_editorContext.m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
        m_editorContext.m_pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
        m_worldManager = context.GetSystem<EntityWorldManager>();
        m_pRenderingSystem = context.GetSystem<Render::RenderingSystem>();

        // Create map editor workspace
        //-------------------------------------------------------------------------

        auto pPrimaryWorld = m_worldManager->GetPrimaryWorld();
        m_pRenderingSystem->CreateCustomRenderTargetForViewport( pPrimaryWorld->GetViewport() );
        m_pMapEditor = KRG::New<EntityModel::EntityMapEditor>( m_editorContext, pPrimaryWorld );
        m_pMapEditor->Initialize();
        m_workspaces.emplace_back( m_pMapEditor );
    }

    void EditorModel::Shutdown( UpdateContext const& context )
    {
        for ( auto& pOpenWorkspace : m_workspaces )
        {
            if ( pOpenWorkspace->IsDirty() )
            {
                InlineString<255> messageTitle;
                messageTitle.sprintf( "Unsaved Changes for %s", pOpenWorkspace->GetDisplayName() );

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

            pOpenWorkspace->Shutdown();
            KRG::Delete( pOpenWorkspace );
        }

        m_workspaces.clear();

        //-------------------------------------------------------------------------

        m_worldManager = nullptr;
        m_pRenderingSystem = nullptr;
        m_editorContext.m_pResourceSystem = nullptr;
        m_editorContext.m_pTypeRegistry = nullptr;
        m_editorContext.m_compiledResourceDirectory = m_editorContext.m_sourceResourceDirectory = FileSystem::Path();
    }

    //-------------------------------------------------------------------------

    bool EditorModel::TryCreateWorkspace( ResourceID const& resourceID )
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
            auto pPreviewWorld = m_worldManager->CreateWorld();
            pPreviewWorld->LoadMap( ResourcePath( "data://Editor/EditorMap.map" ) );
            m_pRenderingSystem->CreateCustomRenderTargetForViewport( pPreviewWorld->GetViewport() );

            // Create workspace
            auto pCreatedWorkspace = ResourceWorkspaceFactory::TryCreateWorkspace( m_editorContext, pPreviewWorld, resourceID );
            KRG_ASSERT( pCreatedWorkspace != nullptr );
            pCreatedWorkspace->Initialize();
            m_workspaces.emplace_back( pCreatedWorkspace );
        }
        else
        {
            //FocusWorkspace( *foundWorkspaceIter );
        }

        return true;
    }

    void EditorModel::DestroyWorkspace( EditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );

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

        auto foundWorkspaceIter = eastl::find( m_workspaces.begin(), m_workspaces.end(), pWorkspace );
        KRG_ASSERT( foundWorkspaceIter != m_workspaces.end() );
        auto pPreviewWorld = ( *foundWorkspaceIter )->GetWorld();

        // Destroy workspace
        pWorkspace->Shutdown();
        KRG::Delete( *foundWorkspaceIter );
        m_workspaces.erase( foundWorkspaceIter );

        // Destroy preview world
        m_worldManager->DestroyWorld( pPreviewWorld );
    }

    bool EditorModel::HasDescriptorForResourceType( ResourceTypeID resourceTypeID ) const
    {
        if ( resourceTypeID == EntityModel::EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            return false;
        }

        return m_editorContext.m_pTypeRegistry->IsRegisteredResourceType( resourceTypeID );
    }

    void* EditorModel::GetViewportTextureForWorkspace( EditorWorkspace* pWorkspace ) const
    {
        KRG_ASSERT( pWorkspace != nullptr );
        auto pWorld = pWorkspace->GetWorld();
        return (void*) &m_pRenderingSystem->GetRenderTargetTextureForViewport( pWorld->GetViewport() );
    }

    //-------------------------------------------------------------------------

    bool EditorModel::IsMapEditorWorkspace( EditorWorkspace const* pWorkspace ) const
    {
        return pWorkspace == m_pMapEditor;
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
}