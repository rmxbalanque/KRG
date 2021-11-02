#pragma once
#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Helpers/GlobalRegistryBase.h"
#include "imgui.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    class EntityWorld;
    class UpdateContext;

    namespace Render { class Viewport; }
}

//-------------------------------------------------------------------------
// Resource Editor Workspace
//-------------------------------------------------------------------------
// This is a base class to create a sub-editor for a given resource type that runs within the resource editor

namespace KRG
{
    struct ResourceEditorContext
    {
        FileSystem::Path                    m_sourceResourceDirectory;
        FileSystem::Path                    m_compiledResourceDirectory;
        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
        EntityWorld*                        m_pPreviewWorld = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API ResourceEditorWorkspace
    {

    public:

        virtual ~ResourceEditorWorkspace() = default;

        // Get the shared workspace name
        virtual char const* GetWorkspaceName() const = 0;

        // Get the shared viewport name for this workspace
        char const* GetViewportWindowName() const;

        // Get a unique window name for this workspace
        inline char const* GetWindowName() const { return m_windowName.c_str(); }

        // Get the actual file-path for the file
        inline FileSystem::Path const& GetFilePath() const { return m_filePath; }

        // Get the resource type ID of the view resource
        virtual ResourceTypeID GetResourceTypeID() const { return ResourceTypeID::Unknown; }

        // Return the friendly resource type name for the viewed resource
        virtual char const* GetResourceTypeFriendlyName() const { return ""; }

        // Has this workspace been activated?
        inline bool IsActive() const { return m_isActive; }

        // Called when this file is set as the active one in the editor tab well
        void SetActive( EntityWorld* pPreviewWorld )
        {
            KRG_ASSERT( !m_isActive );
            Activate( pPreviewWorld );
            m_isActive = true; 
        }

        // Called when we switch away from this file being active, allows up to clean up the preview scene
        void SetInactive( EntityWorld* pPreviewWorld ) 
        {
            KRG_ASSERT( m_isActive );
            Deactivate( pPreviewWorld );
            m_isActive = false;
        }

        // Set up initial docking layout
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const = 0;

        // Draw any UI required for this file within the already allocated window
        virtual void UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) = 0;

        // Called within the context of a large overlay window allowing you to draw helpers and widgets over a viewport
        virtual void DrawOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) {}

        // Should this workspace display a viewport?
        virtual bool RequiresViewportWindow() const { return true; }

        // Do we have a viewport toolbar>
        virtual bool HasViewportToolbar() const { return false; }
        
        // Draw the viewport toolbar
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) {};

        // Has any modifications been made to this file?
        virtual bool IsDirty() const { return false; }

        // Optional: Save functionality for files that support it
        inline bool Save() { return OnSave(); }

        // Resource Status
        virtual bool IsLoading() const { return false; }
        virtual bool IsUnloaded() const { return false; }
        virtual bool IsLoaded() const { return false; }
        virtual bool IsWaitingForResource() const { return false; }
        virtual bool HasLoadingFailed() const { return false; }

        // Hot-reload
        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) {}
        virtual void EndHotReload() {}

    protected:

        ResourceEditorWorkspace( ResourceEditorContext const& context );

        // Disable copies
        ResourceEditorWorkspace& operator=( ResourceEditorWorkspace const& ) = delete;
        ResourceEditorWorkspace( ResourceEditorWorkspace const& ) = delete;

        virtual bool OnSave() { return false; }

        // Called when this file is set as the active one in the editor tab well
        virtual void Activate( EntityWorld* pPreviewWorld ) = 0;

        // Called when we switch away from this file being active, allows up to clean up the preview scene
        virtual void Deactivate( EntityWorld* pPreviewWorld ) = 0;

    protected:

        ResourceEditorContext const&        m_editorContext;

        // UI Info
        String                              m_windowName;
        String                              m_viewportName;
        FileSystem::Path                    m_filePath;

    private:

        bool                                m_isActive = false;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class TResourceEditorWorkspace : public ResourceEditorWorkspace
    {
        static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derived from IResource" );

    public:

        // Specify whether to initially load the resource, this is not necessary for all editors
        TResourceEditorWorkspace( ResourceEditorContext const& context, ResourceID const& resourceID, bool shouldLoadResource = true )
            : ResourceEditorWorkspace( context )
            , m_pResource( resourceID )
        {
            KRG_ASSERT( resourceID.IsValid() );
            m_filePath = resourceID.GetPath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );

            String const displayName = m_filePath.GetFileNameWithoutExtension();
            m_windowName.sprintf( "%s##%u", displayName.c_str(), resourceID.GetPath().GetID() );

            if ( shouldLoadResource )
            {
                m_editorContext.m_pResourceSystem->LoadResource( m_pResource );
            }
        }

        ~TResourceEditorWorkspace()
        {
            if ( !m_pResource.IsUnloaded() )
            {
                m_editorContext.m_pResourceSystem->UnloadResource( m_pResource );
            }
        }

        virtual ResourceTypeID GetResourceTypeID() const override { return T::GetStaticResourceTypeID(); }
        virtual char const* GetResourceTypeFriendlyName() const override { return ""; }

        // Resource Status
        virtual bool IsLoading() const override { return m_pResource.IsLoading(); }
        virtual bool IsUnloaded() const override { return m_pResource.IsUnloaded(); }
        virtual bool IsLoaded() const override { return m_pResource.IsLoaded(); }
        virtual bool IsWaitingForResource() const override { return IsLoading() || IsUnloaded(); }
        virtual bool HasLoadingFailed() const override { return m_pResource.HasLoadingFailed(); }

        // Hot-reload
        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) override
        {
            bool const shouldReload = ( m_pResource.IsLoaded() || m_pResource.IsLoading() );
            if ( shouldReload && VectorContains( resourcesToBeReloaded, m_pResource.GetResourceID() ) )
            {
                m_editorContext.m_pResourceSystem->UnloadResource( m_pResource );
                m_isHotReloading = true;
            }
        }

        virtual void EndHotReload() override 
        {
            if ( m_isHotReloading )
            {
                m_editorContext.m_pResourceSystem->LoadResource( m_pResource );
                m_isHotReloading = false;
            }
        }

    private:

        // Disable copies
        TResourceEditorWorkspace& operator=( TResourceEditorWorkspace const& ) = delete;
        TResourceEditorWorkspace( TResourceEditorWorkspace const& ) = delete;

    protected:

        TResourcePtr<T>                     m_pResource;

    private:

        bool                                m_isHotReloading = false;
    };

    //-------------------------------------------------------------------------
    // Resource Workspace Factory
    //-------------------------------------------------------------------------
    // Used to spawn the appropriate factory

    class KRG_TOOLS_CORE_API ResourceEditorWorkspaceFactory : public TGlobalRegistryBase<ResourceEditorWorkspaceFactory>
    {
        KRG_DECLARE_GLOBAL_REGISTRY( ResourceEditorWorkspaceFactory );

    public:

        static bool CanCreateWorkspace( ResourceTypeID const& resourceTypeID );
        static ResourceEditorWorkspace* TryCreateWorkspace( ResourceEditorContext const& context, ResourceID const& resourceID );

    protected:

        // Get the resource type this factory supports
        virtual ResourceTypeID GetSupportedResourceTypeID() const = 0;

        // Virtual method that will create a workspace if the resource ID matches the appropriate types
        virtual ResourceEditorWorkspace* CreateWorkspace( ResourceEditorContext const& context, ResourceID const& resourceID ) const = 0;
    };
}

//-------------------------------------------------------------------------
//  Macro to create a resource workspace factory
//-------------------------------------------------------------------------
// Use in a CPP to define a factory e.g., KRG_RESOURCE_WORKSPACE_FACTORY( SkeletonWorkspaceFactory, Skeleton, SkeletonResourceEditor );
    
#define KRG_RESOURCE_WORKSPACE_FACTORY( factoryName, resourceClass, editorClass )\
class factoryName final : public ResourceEditorWorkspaceFactory\
{\
    virtual ResourceTypeID GetSupportedResourceTypeID() const override { return resourceClass::GetStaticResourceTypeID(); }\
    virtual ResourceEditorWorkspace* CreateWorkspace( ResourceEditorContext const& context, ResourceID const& resourceID ) const override\
    {\
        KRG_ASSERT( resourceID.GetResourceTypeID() == resourceClass::GetStaticResourceTypeID() );\
        return KRG::New<editorClass>( context, resourceID );\
    }\
};\
static factoryName g_##factoryName;