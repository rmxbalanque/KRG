#pragma once
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceEditorWorkspace;

    //-------------------------------------------------------------------------

    class ResourceEditorModel final
    {
    public:

        ~ResourceEditorModel();

        void Initialize( UpdateContext const& context );
        void Shutdown( UpdateContext const& context );

        inline FileSystem::Path const& GetSourceDataDirectory() const { return m_editorContext.m_sourceDataDirectory; }
        inline FileSystem::Path const& GetCompiledDataDirectory() const { return m_editorContext.m_compiledDataDirectory; }
        inline TypeSystem::TypeRegistry const* GetTypeRegistry() const { return m_editorContext.m_pTypeRegistry; }
        inline Resource::ResourceSystem* GetResourceSystem() const { return m_editorContext.m_pResourceSystem; }

        // Workspaces
        //-------------------------------------------------------------------------

        bool CanCreateWorkspaceForResourceType( ResourceTypeID typeID );

        inline bool HasOpenWorkspaces() const { return !m_openWorkspaces.empty(); }

        void OpenWorkspace( ResourceID const& resourceID );
        void CloseWorkspace( ResourceEditorWorkspace* pWorkspace );
        inline TVector<ResourceEditorWorkspace*> const& GetOpenWorkspaces() const { return m_openWorkspaces; }
        inline bool IsWorkspaceOpenForFile( FileSystem::Path const& path ) const;

        // The active workspace is the one that has "focus" and is currently being worked on.
        inline bool IsActiveWorkspace( ResourceEditorWorkspace const* pWorkspace ) const { return m_pActiveWorkspace == pWorkspace; }
        inline ResourceEditorWorkspace* GetActiveWorkspace() const { return m_pActiveWorkspace; }
        void SetActiveWorkspace( ResourceEditorWorkspace* pWorkspace );
        void ClearActiveWorkspace();

    private:

        ResourceEditorWorkspace* CreateResourceWorkspace( ResourceEditorContext const& model, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID );

    private:

        ResourceEditorContext                       m_editorContext;
        TVector<ResourceEditorWorkspace*>           m_openWorkspaces;
        ResourceEditorWorkspace*                    m_pActiveWorkspace = nullptr;
    };
}