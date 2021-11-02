#pragma once
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class MapEditor;

    //-------------------------------------------------------------------------

    class EditorModel final
    {
    public:

        ~EditorModel();

        void Initialize( UpdateContext const& context );
        void Shutdown( UpdateContext const& context );

        inline FileSystem::Path const& GetRawResourceDirectory() const { return m_editorContext.m_sourceResourceDirectory; }
        inline FileSystem::Path const& GetCompiledResourceDirectory() const { return m_editorContext.m_compiledResourceDirectory; }
        inline TypeSystem::TypeRegistry const* GetTypeRegistry() const { return m_editorContext.m_pTypeRegistry; }
        inline Resource::ResourceSystem* GetResourceSystem() const { return m_editorContext.m_pResourceSystem; }

        // Map Editor
        //-------------------------------------------------------------------------

        MapEditor* GetMapEditorWorkspace() { return m_pMapEditor; }

        // Workspaces
        //-------------------------------------------------------------------------

        bool CanCreateWorkspaceForResourceType( ResourceTypeID typeID );

        void CreateWorkspace( ResourceID const& resourceID );
        void DestroyWorkspace( ResourceEditorWorkspace* pWorkspace );
        inline TVector<ResourceEditorWorkspace*> const& GetWorkspaces() const { return m_workspaces; }
        inline bool IsWorkspaceOpen( FileSystem::Path const& path ) const;

        // The active workspace is the one that has "focus" and is currently being worked on.
        inline bool IsActiveWorkspace( ResourceEditorWorkspace const* pWorkspace ) const { return m_pActiveWorkspace == pWorkspace; }
        inline ResourceEditorWorkspace* GetActiveWorkspace() const { return m_pActiveWorkspace; }
        void SetActiveWorkspace( ResourceEditorWorkspace* pWorkspace );
        void ClearActiveWorkspace();

    private:

        MapEditor*                                  m_pMapEditor = nullptr;
        ResourceEditorContext                       m_editorContext;
        TVector<ResourceEditorWorkspace*>           m_workspaces;
        ResourceEditorWorkspace*                    m_pActiveWorkspace = nullptr;
    };
}