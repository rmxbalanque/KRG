#pragma once
#include "Tools/Core/Editor/EditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class MapEditor;
    class EntityWorldManager;
    namespace Render{ class RenderingSystem; }

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
        inline Render::RenderingSystem* GetRenderingSystem() const { return m_pRenderingSystem; }

        // Map Editor
        //-------------------------------------------------------------------------

        MapEditor* GetMapEditorWorkspace() { return m_pMapEditor; }

        // Workspaces
        //-------------------------------------------------------------------------
        
        inline TVector<EditorWorkspace*> const& GetWorkspaces() const { return m_workspaces; }
        inline bool IsWorkspaceOpen( ResourceID const& resourceID ) const { return FindResourceWorkspace( resourceID ) != nullptr; }
        inline bool IsWorkspaceOpen( uint32 workspaceID ) const { return FindResourceWorkspace( workspaceID ) != nullptr; }

        bool CanCreateWorkspaceForResourceType( ResourceTypeID typeID ) const;
        void CreateWorkspace( ResourceID const& resourceID );
        void DestroyWorkspace( EditorWorkspace* pWorkspace );

        void* GetViewportTextureForWorkspace( EditorWorkspace* pWorkspace ) const;

    private:

        EditorWorkspace* FindResourceWorkspace( ResourceID const& resourceID ) const;
        EditorWorkspace* FindResourceWorkspace( uint32 workspaceID ) const;

    private:

        MapEditor*                          m_pMapEditor = nullptr;
        EntityWorldManager*                 m_worldManager = nullptr;
        Render::RenderingSystem*            m_pRenderingSystem = nullptr;
        EditorContext                       m_editorContext;
        TVector<EditorWorkspace*>           m_workspaces;
    };
}