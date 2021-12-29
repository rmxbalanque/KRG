#pragma once
#include "Tools/Core/Workspaces/EditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EntityWorldManager;
    class GamePreviewer;
    namespace EntityModel { class EntityMapEditor; }
    namespace Render{ class RenderingSystem; }

    //-------------------------------------------------------------------------

    class EditorContext final
    {
    public:

        ~EditorContext();

        void Initialize( UpdateContext const& context );
        void Shutdown( UpdateContext const& context );
        void Update( UpdateContext const& context );

        inline FileSystem::Path const& GetRawResourceDirectory() const { return m_resourceDB.GetRawResourceDirectoryPath(); }
        inline FileSystem::Path const& GetCompiledResourceDirectory() const { return m_resourceDB.GetCompiledResourceDirectoryPath(); }
        inline TypeSystem::TypeRegistry const* GetTypeRegistry() const { return m_workspaceInitContext.m_pTypeRegistry; }
        inline Resource::ResourceDatabase const* GetResourceDatabase() const { return m_workspaceInitContext.m_pResourceDatabase; }
        inline Resource::ResourceSystem* GetResourceSystem() const { return m_workspaceInitContext.m_pResourceSystem; }
        inline Render::RenderingSystem* GetRenderingSystem() const { return m_pRenderingSystem; }

        bool HasDescriptorForResourceType( ResourceTypeID resourceTypeID ) const;

        // Map editor and game preview
        //-------------------------------------------------------------------------

        bool IsMapEditorWorkspace( EditorWorkspace const* pWorkspace ) const;
        char const* GetMapEditorWindowName() const;

        bool IsGamePreviewWorkspace( EditorWorkspace const* pWorkspace ) const;
        inline GamePreviewer* GetGamePreviewWorkspace() const { return m_pGamePreviewer; }

        inline bool IsGamePreviewRunning() const { return m_pGamePreviewer != nullptr; }
        bool IsGamePreviewAllowed() const;

        void StartGamePreview( UpdateContext const& context );
        void StopGamePreview( UpdateContext const& context );

        // Workspaces
        //-------------------------------------------------------------------------

        inline TVector<EditorWorkspace*> const& GetWorkspaces() const { return m_workspaces; }
        void* GetViewportTextureForWorkspace( EditorWorkspace* pWorkspace ) const;
        uint64 GetViewportPickingID( EditorWorkspace* pWorkspace, Int2 const& pixelCoords ) const;

        inline bool IsWorkspaceOpen( uint32 workspaceID ) const { return FindResourceWorkspace( workspaceID ) != nullptr; }
        inline bool IsWorkspaceOpen( ResourceID const& resourceID ) const { return FindResourceWorkspace( resourceID ) != nullptr; }

        // Immediately destroy a workspace
        void DestroyWorkspace( UpdateContext const& context, EditorWorkspace* pWorkspace );

        // Queues a workspace destruction request till the next update
        void QueueDestroyWorkspace( EditorWorkspace* pWorkspace );

        // Tries to immediately create a workspace
        bool TryCreateWorkspace( UpdateContext const& context, ResourceID const& resourceID );

        // Queues a workspace creation request till the next update
        void QueueCreateWorkspace( ResourceID const& resourceID ) { m_workspaceCreationRequests.emplace_back( resourceID ); }

    private:

        EditorWorkspace* FindResourceWorkspace( ResourceID const& resourceID ) const;
        EditorWorkspace* FindResourceWorkspace( uint32 workspaceID ) const;

        void DestroyWorkspaceInternal( UpdateContext const& context, EditorWorkspace* pWorkspace );

    private:

        EntityModel::EntityMapEditor*       m_pMapEditor = nullptr;
        GamePreviewer*                      m_pGamePreviewer = nullptr;
        EntityWorldManager*                 m_pWorldManager = nullptr;
        Render::RenderingSystem*            m_pRenderingSystem = nullptr;
        Resource::ResourceDatabase          m_resourceDB;
        WorkspaceInitializationContext      m_workspaceInitContext;
        TVector<EditorWorkspace*>           m_workspaces;
        TVector<ResourceID>                 m_workspaceCreationRequests;
        TVector<EditorWorkspace*>           m_workspaceDestructionRequests;
    };
}