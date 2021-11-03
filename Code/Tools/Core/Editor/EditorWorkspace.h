#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Resource/ResourceID.h"
#include "System/Core/FileSystem/FileSystemPath.h"

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

        static char const* const s_viewportWindowName;

    public:

        EditorWorkspace( EditorContext const& context, EntityWorld* pWorld );
        virtual ~EditorWorkspace() = default;

        // Get the display name for this workspace
        virtual char const* GetDisplayName() const = 0;

        // Get a unique ID for this workspace
        virtual uint32 GetID() const = 0;

        // Should this workspace display a viewport?
        virtual bool HasViewportWindow() const { return true; }

        // Does this workspace's viewport have a toolbar?
        virtual bool HasViewportWindowToolbar() const { return false; }

        // Get the world associated with this workspace
        inline EntityWorld* GetWorld() const { return m_pWorld; }

        // Lifetime Functions
        //-------------------------------------------------------------------------

        virtual void Initialize() {}
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

        // Disable copies
        EditorWorkspace& operator=( EditorWorkspace const& ) = delete;
        EditorWorkspace( EditorWorkspace const& ) = delete;

    protected:

        EditorContext const&                m_editorContext;
        EntityWorld*                        m_pWorld = nullptr;
    };
}