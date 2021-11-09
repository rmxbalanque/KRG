#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Resource/ResourceID.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Debug/DebugDrawing.h"

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
        FileSystem::Path                    m_sourceResourceDirectory;
        FileSystem::Path                    m_compiledResourceDirectory;
        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API EditorWorkspace
    {
    public:

        EditorWorkspace( EditorContext const& context, EntityWorld* pWorld, String const& displayName = "Workspace" );
        virtual ~EditorWorkspace() = default;

        // Get the display name for this workspace (shown on tab, dialogs, etc...)
        virtual char const* GetDisplayName() const { return m_displayName.c_str(); }

        // Get the main workspace window ID - Needs to be unique per workspace instance!
        inline char const* GetWorkspaceWindowID() const { KRG_ASSERT( !m_workspaceWindowID.empty() ); return m_workspaceWindowID.c_str(); }

        // Get the viewport window name/ID - Needs to be unique per workspace instance!
        inline  char const* GetViewportWindowID() const { KRG_ASSERT( !m_viewportWindowID.empty() ); return m_viewportWindowID.c_str(); }

        // Get the main workspace window ID - Needs to be unique per workspace instance!
        inline char const* GetDockspaceID() const { KRG_ASSERT( !m_dockspaceID.empty() ); return m_dockspaceID.c_str(); }

        // Get a unique ID for this workspace
        virtual uint32 GetID() const = 0;

        // Should this workspace display a viewport?
        virtual bool HasViewportWindow() const { return true; }

        // Does this workspace's viewport have a toolbar?
        virtual bool HasViewportToolbar() const { return false; }

        // Get the world associated with this workspace
        inline EntityWorld* GetWorld() const { return m_pWorld; }

        // Lifetime Functions
        //-------------------------------------------------------------------------

        // Initialize the workspace: initialize window IDs, create preview entities, etc... - Base implementation must be called!
        virtual void Initialize();
        virtual void Shutdown() {}

        // Drawing Functions
        //-------------------------------------------------------------------------

        // Set up initial docking layout
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const = 0;

        // Frame update and draw any tool windows needed for the workspace
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) = 0;

        // Called within the context of a large overlay window allowing you to draw helpers and widgets over a viewport
        virtual void DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) {}

        // Draw the viewport toolbar
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) {};

        // Saving and Dirty state
        //-------------------------------------------------------------------------

        // Has any modifications been made to this file?
        virtual bool IsDirty() const { return false; }

        // Optional: Save functionality for files that support it
        virtual bool Save() { return false; }

        // Hot-reload
        //-------------------------------------------------------------------------

        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) {}
        virtual void EndHotReload() {}

    protected:

        inline Debug::DrawingContext GetDrawingContext();
        void SetDisplayName( String const& name );

        // Disable copies
        EditorWorkspace& operator=( EditorWorkspace const& ) = delete;
        EditorWorkspace( EditorWorkspace const& ) = delete;

    protected:

        EditorContext const&                m_editorContext;
        EntityWorld*                        m_pWorld = nullptr;
        String                              m_displayName;
        String                              m_workspaceWindowID;
        String                              m_viewportWindowID;
        String                              m_dockspaceID;
    };
}