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
        ResourceEditorWorkspace* CreateResourceWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID );

        // File Management
        //-------------------------------------------------------------------------

        void OpenResourceFile( ResourceID const& resourceID );
        void CloseFile( ResourceEditorWorkspace* pFile );
        inline TVector<ResourceEditorWorkspace*> const& GetOpenFiles() const { return m_openTabs; }
        inline bool IsFileOpen( FileSystem::Path const& path ) const;

        // The active file is the file that has "focus" and is currently being previewed or edited.
        inline bool IsActiveFile( ResourceEditorWorkspace const* pFile ) const { return m_pActiveTab == pFile; }
        void SetActiveFile( ResourceEditorWorkspace* pFile );
        void ClearActiveFile();

    private:

        // File Management
        TVector<ResourceEditorWorkspace*>             m_openTabs;
        ResourceEditorWorkspace*                      m_pActiveTab = nullptr;
    };
}