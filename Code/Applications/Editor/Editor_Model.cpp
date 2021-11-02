#include "Editor_Model.h"
#include "MapEditor/ResourceEditor_Map.h"
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "System/Resource/ResourceSettings.h"
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

        m_editorContext.m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
        KRG_ASSERT( m_editorContext.m_pTypeRegistry != nullptr );

        m_editorContext.m_pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
        KRG_ASSERT( m_editorContext.m_pResourceSystem != nullptr );

        m_editorContext.m_pPreviewWorld = context.GetSystem<EntityWorldManager>()->GetPrimaryWorld();
        KRG_ASSERT( m_editorContext.m_pPreviewWorld != nullptr );

        // Create map editor workspace
        //-------------------------------------------------------------------------

        m_pMapEditor = KRG::New<MapEditor>( m_editorContext );
        m_workspaces.emplace_back( m_pMapEditor );
        SetActiveWorkspace( m_pMapEditor );
    }

    void EditorModel::Shutdown( UpdateContext const& context )
    {
        ClearActiveWorkspace();

        //-------------------------------------------------------------------------

        for ( auto& pOpenWorkspace : m_workspaces )
        {
            if ( pOpenWorkspace->IsDirty() )
            {
                InlineString<255> messageTitle;
                messageTitle.sprintf( "Unsaved Changes for %s", pOpenWorkspace->GetWindowName() );

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

            KRG::Delete( pOpenWorkspace );
        }

        m_workspaces.clear();
    }

    //-------------------------------------------------------------------------

    bool EditorModel::CanCreateWorkspaceForResourceType( ResourceTypeID typeID )
    {
        KRG_ASSERT( typeID.IsValid() );
        return ResourceEditorWorkspaceFactory::CanCreateWorkspace( typeID );
    }

    //-------------------------------------------------------------------------

    void EditorModel::CreateWorkspace( ResourceID const & resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        FileSystem::Path const path = resourceID.GetPath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );

        //-------------------------------------------------------------------------

        auto SearchPredicate = [] ( ResourceEditorWorkspace*& pExistingWorkspace, FileSystem::Path const& path )
        { 
            return pExistingWorkspace->GetFilePath() == path;
        };

        auto foundWorkspaceIter = eastl::find( m_workspaces.begin(), m_workspaces.end(), path, SearchPredicate );
        if ( foundWorkspaceIter == m_workspaces.end() )
        {
            auto pCreatedWorkspace = ResourceEditorWorkspaceFactory::TryCreateWorkspace( m_editorContext, resourceID );
            KRG_ASSERT( pCreatedWorkspace != nullptr );
            m_workspaces.emplace_back( pCreatedWorkspace );
            SetActiveWorkspace( pCreatedWorkspace );
        }
        else
        {
            SetActiveWorkspace( *foundWorkspaceIter );
        }
    }

    bool EditorModel::IsWorkspaceOpen( FileSystem::Path const& path ) const
    {
        for ( auto pWorkspace : m_workspaces )
        {
            if ( pWorkspace->GetFilePath() == path )
            {
                return true;
            }
        }

        return false;
    }

    void EditorModel::DestroyWorkspace( ResourceEditorWorkspace* pWorkspace )
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

        if ( m_pActiveWorkspace == pWorkspace )
        {
            ClearActiveWorkspace();
        }

        //-------------------------------------------------------------------------

        auto foundWorkspaceIter = eastl::find( m_workspaces.begin(), m_workspaces.end(), pWorkspace );
        KRG_ASSERT( foundWorkspaceIter != m_workspaces.end() );
        KRG::Delete( *foundWorkspaceIter );
        m_workspaces.erase( foundWorkspaceIter );

        //-------------------------------------------------------------------------

        if ( !m_workspaces.empty() )
        {
            SetActiveWorkspace( m_workspaces.back() );
        }
    }

    //-------------------------------------------------------------------------

    void EditorModel::SetActiveWorkspace( ResourceEditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );
        KRG_ASSERT( VectorContains( m_workspaces, pWorkspace ) );

        if ( m_pActiveWorkspace == pWorkspace )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_pActiveWorkspace != nullptr )
        {
            ClearActiveWorkspace();
        }

        pWorkspace->SetActive( m_editorContext.m_pPreviewWorld );
        m_pActiveWorkspace = pWorkspace;
    }

    void EditorModel::ClearActiveWorkspace()
    {
        if ( m_pActiveWorkspace != nullptr )
        {
            m_pActiveWorkspace->SetInactive( m_editorContext.m_pPreviewWorld );
            m_pActiveWorkspace = nullptr;
        }
    }
}