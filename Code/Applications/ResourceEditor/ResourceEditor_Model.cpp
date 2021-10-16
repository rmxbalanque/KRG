#include "ResourceEditor_Model.h"
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceEditorModel::~ResourceEditorModel()
    {
        KRG_ASSERT( m_openWorkspaces.empty() );
    }

    void ResourceEditorModel::Initialize( UpdateContext const& context )
    {
        auto pSettingsRegistry = context.GetSystem<SettingsRegistry>();
        KRG_ASSERT( pSettingsRegistry != nullptr );
        auto pResourceSettings = pSettingsRegistry->GetSettings<Resource::Settings>();
        KRG_ASSERT( pResourceSettings != nullptr );

        m_editorContext.m_sourceDataDirectory = pResourceSettings->m_rawResourcePath;
        m_editorContext.m_compiledDataDirectory = pResourceSettings->m_compiledResourcePath;

        m_editorContext.m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
        KRG_ASSERT( m_editorContext.m_pTypeRegistry != nullptr );

        m_editorContext.m_pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
        KRG_ASSERT( m_editorContext.m_pResourceSystem != nullptr );

        m_editorContext.m_pPreviewWorld = context.GetSystem<EntityWorld>();
        KRG_ASSERT( m_editorContext.m_pPreviewWorld != nullptr );
    }

    void ResourceEditorModel::Shutdown( UpdateContext const& context )
    {
        ClearActiveWorkspace();

        //-------------------------------------------------------------------------

        for ( auto& pOpenWorkspace : m_openWorkspaces )
        {
            if ( pOpenWorkspace->IsDirty() )
            {
                InlineString<255> messageTitle;
                messageTitle.sprintf( "Unsaved Changes for %s", pOpenWorkspace->GetFilePath().c_str() );

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

        m_openWorkspaces.clear();
    }

    //-------------------------------------------------------------------------

    bool ResourceEditorModel::CanCreateWorkspaceForResourceType( ResourceTypeID typeID )
    {
        KRG_ASSERT( typeID.IsValid() );
        return ResourceEditorWorkspaceFactory::CanCreateWorkspace( typeID );
    }

    ResourceEditorWorkspace* ResourceEditorModel::CreateResourceWorkspace( ResourceEditorContext const& model, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
    {
        return ResourceEditorWorkspaceFactory::TryCreateWorkspace( model, resourceID );
    }

    //-------------------------------------------------------------------------

    void ResourceEditorModel::OpenWorkspace( ResourceID const & resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        FileSystem::Path const path = resourceID.GetPath().ToFileSystemPath( m_editorContext.m_sourceDataDirectory );

        //-------------------------------------------------------------------------

        auto SearchPredicate = [] ( ResourceEditorWorkspace*& pExistingWorkspace, FileSystem::Path const& path )
        { 
            return pExistingWorkspace->GetFilePath() == path;
        };

        auto foundWorkspaceIter = eastl::find( m_openWorkspaces.begin(), m_openWorkspaces.end(), path, SearchPredicate );
        if ( foundWorkspaceIter == m_openWorkspaces.end() )
        {
            auto pCreatedWorkspace = m_openWorkspaces.emplace_back( CreateResourceWorkspace( m_editorContext, m_editorContext.m_pResourceSystem, resourceID ) );
            SetActiveWorkspace( pCreatedWorkspace );
        }
        else
        {
            SetActiveWorkspace( *foundWorkspaceIter );
        }
    }

    bool ResourceEditorModel::IsWorkspaceOpenForFile( FileSystem::Path const& path ) const
    {
        for ( auto pWorkspace : m_openWorkspaces )
        {
            if ( pWorkspace->GetFilePath() == path )
            {
                return true;
            }
        }

        return false;
    }

    void ResourceEditorModel::CloseWorkspace( ResourceEditorWorkspace* pWorkspace )
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

        auto foundWorkspaceIter = eastl::find( m_openWorkspaces.begin(), m_openWorkspaces.end(), pWorkspace );
        KRG_ASSERT( foundWorkspaceIter != m_openWorkspaces.end() );
        KRG::Delete( *foundWorkspaceIter );
        m_openWorkspaces.erase( foundWorkspaceIter );

        //-------------------------------------------------------------------------

        if ( !m_openWorkspaces.empty() )
        {
            SetActiveWorkspace( m_openWorkspaces.back() );
        }
    }

    //-------------------------------------------------------------------------

    void ResourceEditorModel::SetActiveWorkspace( ResourceEditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );
        KRG_ASSERT( VectorContains( m_openWorkspaces, pWorkspace ) );

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

    void ResourceEditorModel::ClearActiveWorkspace()
    {
        if ( m_pActiveWorkspace != nullptr )
        {
            m_pActiveWorkspace->SetInactive( m_editorContext.m_pPreviewWorld );
            m_pActiveWorkspace = nullptr;
        }
    }
}