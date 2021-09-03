#include "ResourceWorkspace.h"
#include "System\DevTools\ThirdParty\imgui\imgui.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    ResourceEditorWorkspace::ResourceEditorWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
        : m_pResourceSystem( pResourceSystem )
        , m_pBaseResource( resourceID )
        , m_pModel( pModel )
    {
        KRG_ASSERT( m_pResourceSystem != nullptr && resourceID.IsValid() );
        m_pResourceSystem->LoadResource( m_pBaseResource );
        m_filePath = resourceID.GetDataPath().ToFileSystemPath( pModel->GetSourceDataDirectory() );

        m_resourceDisplayName = m_filePath.GetFileNameWithoutExtension();
        m_windowName.sprintf( "%s##%u", m_resourceDisplayName.c_str(), resourceID.GetDataPath().GetID() );
    }
}