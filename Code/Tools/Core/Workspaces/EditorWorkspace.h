#pragma once
#include "Tools/Core/UndoStack.h"
#include "Tools/Core/Resource/ResourceDatabase.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Resource/ResourceID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Drawing/DebugDrawing.h"
#include "System/Core/Types/Function.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    class EntityWorld;
    class UpdateContext;

    namespace Render { class Viewport; }
    namespace Resource { class ResourceSystem; }
    namespace TypeSystem { class TypeRegistry; }
}

//-------------------------------------------------------------------------
// Editor Workspace
//-------------------------------------------------------------------------
// This is a base class to create a workspace within the editor

namespace KRG
{
    struct EditorContext
    {
        inline FileSystem::Path const& GetRawResourceDirectoryPath() const { return m_pResourceDatabase->GetRawResourceDirectoryPath(); }
        inline FileSystem::Path const& GetCompiledResourceDirectoryPath() const { return m_pResourceDatabase->GetCompiledResourceDirectoryPath(); }

        inline FileSystem::Path ToFileSystemPath( ResourcePath const& resourcePath ) const
        {
            KRG_ASSERT( m_pResourceDatabase != nullptr && resourcePath.IsValid() );
            return resourcePath.ToFileSystemPath( m_pResourceDatabase->GetRawResourceDirectoryPath() );
        }

        inline FileSystem::Path ToFileSystemPath( ResourceID const& resourceID ) const
        {
            KRG_ASSERT( m_pResourceDatabase != nullptr && resourceID.IsValid() );
            return resourceID.GetResourcePath().ToFileSystemPath(m_pResourceDatabase->GetRawResourceDirectoryPath());
        }

    public:

        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        Resource::ResourceDatabase const*   m_pResourceDatabase = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API EditorWorkspace
    {
    public:

        struct ViewportInfo
        {
            ImTextureID                         m_pViewportRenderTargetTexture = nullptr;
            TFunction<uint64( Int2 const& )>    m_retrievePickingID;
        };

    public:

        EditorWorkspace( EditorContext const& context, EntityWorld* pWorld, String const& displayName = "Workspace" );
        virtual ~EditorWorkspace() = default;

        // Get the display name for this workspace (shown on tab, dialogs, etc...)
        virtual char const* GetDisplayName() const { return m_displayName.c_str(); }

        // Get the main workspace window ID - Needs to be unique per workspace instance!
        inline char const* GetWorkspaceWindowID() const { KRG_ASSERT( !m_workspaceWindowID.empty() ); return m_workspaceWindowID.c_str(); }

        // Get the viewport window name/ID - Needs to be unique per workspace instance!
        inline char const* GetViewportWindowID() const { KRG_ASSERT( !m_viewportWindowID.empty() ); return m_viewportWindowID.c_str(); }

        // Get the main workspace window ID - Needs to be unique per workspace instance!
        inline char const* GetDockspaceID() const { KRG_ASSERT( !m_dockspaceID.empty() ); return m_dockspaceID.c_str(); }

        // Get a unique ID for this workspace
        virtual uint32 GetID() const = 0;

        // Should this workspace display a viewport?
        virtual bool HasViewportWindow() const { return true; }

        // Does this workspace have a toolbar?
        virtual bool HasWorkspaceToolbar() const { return true; }

        // Does this workspace's viewport have a toolbar?
        virtual bool HasViewportToolbar() const { return false; }

        // Does this workspace's viewport have a orientation guide drawn?
        virtual bool HasViewportOrientationGuide() const { return true; }

        // Get the world associated with this workspace
        inline EntityWorld* GetWorld() const { return m_pWorld; }

        // Lifetime Functions
        //-------------------------------------------------------------------------

        // Initialize the workspace: initialize window IDs, create preview entities, etc... - Base implementation must be called!
        virtual void Initialize( UpdateContext const& context );
        virtual void Shutdown( UpdateContext const& context ) {}

        // Drawing Functions
        //-------------------------------------------------------------------------

        // Set up initial docking layout
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const = 0;

        // Frame update and draw any tool windows needed for the workspace
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) = 0;

        // Draw any toolbar buttons that this workspace needs
        virtual void DrawWorkspaceToolbar( UpdateContext const& context ) { DrawDefaultToolbarItems(); }

        // Called within the context of a large overlay window allowing you to draw helpers and widgets over a viewport
        virtual void DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) {}

        // Draw the viewport toolbar
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) {}

        // Draw the viewport for this workspace
        bool DrawViewport( UpdateContext const& context, ViewportInfo const& viewportInfo, ImGuiWindowClass* pWindowClass );

        // Undo/Redo
        //-------------------------------------------------------------------------

        // Called whenever we execute an undo or redo action
        virtual void OnUndoRedo() {}

        inline bool CanUndo() { return m_undoStack.CanUndo(); }
        inline void Undo() { m_undoStack.Undo(); OnUndoRedo(); }
        inline bool CanRedo() { return m_undoStack.CanRedo(); }
        inline void Redo() { m_undoStack.Redo(); OnUndoRedo(); }

        // Saving and Dirty state
        //-------------------------------------------------------------------------

        // Has any modifications been made to this file?
        virtual bool IsDirty() const { return false; }

        // Should we always allow saving?
        virtual bool AlwaysAllowSaving() const { return false; }

        // Optional: Save functionality for files that support it
        virtual bool Save() { return false; }

        // Hot-reload
        //-------------------------------------------------------------------------

        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) {}
        virtual void EndHotReload() {}

    protected:

        // Called whenever we click inside a workspace viewport and get a non-zero picking ID
        virtual void OnMousePick( uint64 pickingID ) {}

        // Helper function for debug drawing
        inline Drawing::DrawContext GetDrawingContext();

        // Set the workspace tab-title
        void SetDisplayName( String const& name );

        // Draw the default workspace toolbar menu items (save, undo, redo, etc...)
        void DrawDefaultToolbarItems();

        // Draw the default workspace toolbar menu items (rendering options, camera, etc...)
        void DrawDefaultViewportToolbarItems();

        // Begin a toolbar group
        bool BeginViewportToolbarGroup( char const* pGroupID, ImVec2 groupSize );

        // End a toolbar group
        void EndViewportToolbarGroup();

        // Disable copies
        EditorWorkspace& operator=( EditorWorkspace const& ) = delete;
        EditorWorkspace( EditorWorkspace const& ) = delete;

    protected:

        EditorContext const&                m_editorContext;
        EntityWorld*                        m_pWorld = nullptr;
        UndoStack                           m_undoStack;
        String                              m_displayName;
        String                              m_workspaceWindowID;
        String                              m_viewportWindowID;
        String                              m_dockspaceID;
        bool                                m_isViewportFocused = false;
    };
}