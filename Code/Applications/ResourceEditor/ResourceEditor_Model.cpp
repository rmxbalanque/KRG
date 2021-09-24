#include "ResourceEditor_Model.h"
#include "Tools/Core/Editors/ResourceEditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceEditorModel::~ResourceEditorModel()
    {
        KRG_ASSERT( m_openWorkspaces.empty() );
    }

    void ResourceEditorModel::Shutdown( UpdateContext const& context )
    {
        ClearActiveWorkspace();

        //-------------------------------------------------------------------------

        for ( auto& pOpenWorkspace : m_openWorkspaces )
        {
            KRG::Delete( pOpenWorkspace );
        }

        m_openWorkspaces.clear();

        //-------------------------------------------------------------------------

        EditorModel::Shutdown( context );
    }

    //-------------------------------------------------------------------------

    bool ResourceEditorModel::CanCreateWorkspaceForResourceType( ResourceTypeID typeID )
    {
        KRG_ASSERT( typeID.IsValid() );
        return ResourceEditorWorkspaceFactory::CanCreateWorkspace( typeID );
    }

    ResourceEditorWorkspace* ResourceEditorModel::CreateResourceWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
    {
        return ResourceEditorWorkspaceFactory::TryCreateWorkspace( pModel, resourceID );
    }

    //-------------------------------------------------------------------------

    void ResourceEditorModel::OpenWorkspace( ResourceID const & resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        FileSystem::Path const path = resourceID.GetDataPath().ToFileSystemPath( m_sourceDataDirectory );

        //-------------------------------------------------------------------------

        auto SearchPredicate = [] ( ResourceEditorWorkspace*& pExistingWorkspace, FileSystem::Path const& path )
        { 
            return pExistingWorkspace->GetFilePath() == path;
        };

        auto foundWorkspaceIter = eastl::find( m_openWorkspaces.begin(), m_openWorkspaces.end(), path, SearchPredicate );
        if ( foundWorkspaceIter == m_openWorkspaces.end() )
        {
            auto pCreatedWorkspace = m_openWorkspaces.emplace_back( CreateResourceWorkspace( this, m_pResourceSystem, resourceID ) );
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

        pWorkspace->SetActive( m_pPreviewWorld );
        m_pActiveWorkspace = pWorkspace;
    }

    void ResourceEditorModel::ClearActiveWorkspace()
    {
        if ( m_pActiveWorkspace != nullptr )
        {
            m_pActiveWorkspace->SetInactive( m_pPreviewWorld );
            m_pActiveWorkspace = nullptr;
        }
    }
}