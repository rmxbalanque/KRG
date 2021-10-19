#include "ResourceEditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceEditorWorkspace::ResourceEditorWorkspace( ResourceEditorContext const& context, ResourceID const& resourceID, bool shouldLoadResource )
        : m_editorContext( context )
        , m_pBaseResource( resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        m_filePath = resourceID.GetPath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );

        m_resourceDisplayName = m_filePath.GetFileNameWithoutExtension();
        m_windowName.sprintf( "%s##%u", m_resourceDisplayName.c_str(), resourceID.GetPath().GetID() );

        if ( shouldLoadResource )
        {
            m_editorContext.m_pResourceSystem->LoadResource( m_pBaseResource );
        }
    }

    ResourceEditorWorkspace::~ResourceEditorWorkspace()
    {
        if ( !m_pBaseResource.IsUnloaded() )
        {
            m_editorContext.m_pResourceSystem->UnloadResource( m_pBaseResource );
        }
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

    ResourceEditorWorkspace* ResourceEditorWorkspaceFactory::TryCreateWorkspace( ResourceEditorContext const& context, ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        auto resourceTypeID = resourceID.GetResourceTypeID();
        KRG_ASSERT( resourceTypeID.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( resourceTypeID == pCurrentFactory->GetSupportedResourceTypeID() )
            {
                return pCurrentFactory->CreateWorkspace( context, resourceID );
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return nullptr;
    }
}