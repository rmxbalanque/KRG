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
        KRG_ASSERT( m_openTabs.empty() );
    }

    void ResourceEditorModel::Shutdown( UpdateContext const& context )
    {
        ClearActiveFile();

        //-------------------------------------------------------------------------

        for ( auto& pOpenFile : m_openTabs )
        {
            KRG::Delete( pOpenFile );
        }

        m_openTabs.clear();

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

    void ResourceEditorModel::OpenResourceFile( ResourceID const & resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        FileSystem::Path const path = resourceID.GetDataPath().ToFileSystemPath( m_sourceDataDirectory );

        //-------------------------------------------------------------------------

        auto foundFileIter = eastl::find( m_openTabs.begin(), m_openTabs.end(), path, [] ( ResourceEditorWorkspace*& pExistingFile, FileSystem::Path const& path ) { return pExistingFile->GetFilePath() == path; } );
        if ( foundFileIter == m_openTabs.end() )
        {
            auto pCreatedWorkspace = m_openTabs.emplace_back( CreateResourceWorkspace( this, m_pResourceSystem, resourceID ) );
            SetActiveFile( pCreatedWorkspace );
        }
        else
        {
            SetActiveFile( *foundFileIter );
        }
    }

    bool ResourceEditorModel::IsFileOpen( FileSystem::Path const& path ) const
    {
        for ( auto pFile : m_openTabs )
        {
            if ( pFile->GetFilePath() == path )
            {
                return true;
            }
        }

        return false;
    }

    void ResourceEditorModel::CloseFile( ResourceEditorWorkspace* pFile )
    {
        KRG_ASSERT( pFile != nullptr );

        if ( m_pActiveTab == pFile )
        {
            ClearActiveFile();
        }

        //-------------------------------------------------------------------------

        auto foundFileIter = eastl::find( m_openTabs.begin(), m_openTabs.end(), pFile );
        KRG_ASSERT( foundFileIter != m_openTabs.end() );
        KRG::Delete( *foundFileIter );
        m_openTabs.erase( foundFileIter );
    }

    //-------------------------------------------------------------------------

    void ResourceEditorModel::SetActiveFile( ResourceEditorWorkspace* pFile )
    {
        KRG_ASSERT( pFile != nullptr );
        KRG_ASSERT( VectorContains( m_openTabs, pFile ) );

        if ( m_pActiveTab == pFile )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_pActiveTab != nullptr )
        {
            ClearActiveFile();
        }

        pFile->SetActive( m_pPreviewWorld );
        m_pActiveTab = pFile;
    }

    void ResourceEditorModel::ClearActiveFile()
    {
        if ( m_pActiveTab != nullptr )
        {
            m_pActiveTab->SetInactive( m_pPreviewWorld );
            m_pActiveTab = nullptr;
        }
    }
}