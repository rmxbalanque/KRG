#pragma once
#include "../EditorModel.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceEditorWorkspace;

    //-------------------------------------------------------------------------

    class ResourceEditorModel : public EditorModel
    {
    public:

        virtual ~ResourceEditorModel();

        virtual void Shutdown( UpdateContext const& context ) override;

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
        inline ResourceEditorWorkspace const* GetActiveWorkspace() const { return m_pActiveWorkspace; }
        void SetActiveWorkspace( ResourceEditorWorkspace* pWorkspace );
        void ClearActiveWorkspace();

    private:

        ResourceEditorWorkspace* CreateResourceWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID );

    private:

        TVector<ResourceEditorWorkspace*>             m_openWorkspaces;
        ResourceEditorWorkspace*                      m_pActiveWorkspace = nullptr;
    };
}