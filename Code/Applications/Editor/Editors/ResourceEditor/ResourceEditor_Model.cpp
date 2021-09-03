#include "ResourceEditor_Model.h"
#include "Workspaces/ResourceWorkspace.h"

#include "Workspaces\Render\StaticMeshWorkspace.h"
#include "Workspaces\Render\SkeletalMeshWorkspace.h"
#include "Workspaces\Animation\SkeletonWorkspace.h"
#include "Workspaces\Animation\AnimationClipWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
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

        if ( typeID == Render::StaticMesh::GetStaticResourceTypeID() )
        {
            return true;
        }
        else if ( typeID == Render::SkeletalMesh::GetStaticResourceTypeID() )
        {
            return true;
        }
        else if ( typeID == Animation::Skeleton::GetStaticResourceTypeID() )
        {
            return true;
        }
        else if ( typeID == Animation::AnimationClip::GetStaticResourceTypeID() )
        {
            return true;
        }

        return false;
    }

    ResourceEditorWorkspace* ResourceEditorModel::CreateResourceWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );

        ResourceTypeID const typeID = resourceID.GetResourceTypeID();
        KRG_ASSERT( typeID.IsValid() );

        if ( typeID == Render::StaticMesh::GetStaticResourceTypeID() )
        {
            return KRG::New<Render::StaticMeshWorkspace>( pModel, pResourceSystem, resourceID );
        }
        else if ( typeID == Render::SkeletalMesh::GetStaticResourceTypeID() )
        {
            return KRG::New<Render::SkeletalMeshWorkspace>( pModel, pResourceSystem, resourceID );
        }
        else if ( typeID == Animation::Skeleton::GetStaticResourceTypeID() )
        {
            return KRG::New<Animation::SkeletonWorkspace>( pModel, pResourceSystem, resourceID );
        }
        else if ( typeID == Animation::AnimationClip::GetStaticResourceTypeID() )
        {
            return KRG::New<Animation::AnimationClipWorkspace>( pModel, pResourceSystem, resourceID );
        }

        KRG_UNREACHABLE_CODE();
        return nullptr;
    }

    //-------------------------------------------------------------------------

    void ResourceEditorModel::OpenWorkspace( ResourceID const & resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        FileSystem::Path const path = resourceID.GetDataPath().ToFileSystemPath( m_sourceDataDirectory );

        //-------------------------------------------------------------------------

        auto foundWorkspaceIter = eastl::find( m_openWorkspaces.begin(), m_openWorkspaces.end(), path, [] ( ResourceEditorWorkspace*& pExistingFile, FileSystem::Path const& path ) { return pExistingFile->GetFilePath() == path; } );
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