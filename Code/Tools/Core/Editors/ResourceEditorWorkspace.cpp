#include "ResourceEditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceEditorWorkspace::ResourceEditorWorkspace( EditorModel* pModel, ResourceID const& resourceID )
        : m_pResourceSystem( pModel->GetResourceSystem() )
        , m_pBaseResource( resourceID )
        , m_pModel( pModel )
    {
        KRG_ASSERT( m_pResourceSystem != nullptr && resourceID.IsValid() );
        m_pResourceSystem->LoadResource( m_pBaseResource );
        m_filePath = resourceID.GetDataPath().ToFileSystemPath( pModel->GetSourceDataDirectory() );

        m_resourceDisplayName = m_filePath.GetFileNameWithoutExtension();
        m_windowName.sprintf( "%s##%u", m_resourceDisplayName.c_str(), resourceID.GetDataPath().GetID() );
    }

    char const* ResourceEditorWorkspace::GetViewportWindowName() const
    {
        if ( m_viewportName.empty() )
        {
            const_cast<String&>( m_viewportName ).sprintf( "Viewport##%s", GetWorkspaceName() );
        }

        return m_viewportName.c_str();
    }

    //-------------------------------------------------------------------------

    KRG_DEFINE_GLOBAL_REGISTRY( ResourceEditorWorkspaceFactory );

    //-------------------------------------------------------------------------

    bool ResourceEditorWorkspaceFactory::CanCreateWorkspace( ResourceTypeID const& resourceTypeID )
    {
        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( resourceTypeID == pCurrentFactory->GetSupportedResourceTypeID() )
            {
                return true;
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return false;
    }

    ResourceEditorWorkspace* ResourceEditorWorkspaceFactory::TryCreateWorkspace( EditorModel* pModel, ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        auto resourceTypeID = resourceID.GetResourceTypeID();
        KRG_ASSERT( resourceTypeID.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( resourceTypeID == pCurrentFactory->GetSupportedResourceTypeID() )
            {
                return pCurrentFactory->CreateWorkspace( pModel, resourceID );
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return nullptr;
    }
}