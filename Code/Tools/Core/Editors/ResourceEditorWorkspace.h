#pragma once
#include "EditorModel.h"
#include "Tools/Core/Helpers/GlobalRegistryBase.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    class EntityWorld;
    class UpdateContext;

    namespace Render { class ViewportManager; }
}

//-------------------------------------------------------------------------
// Resource Editor Workspace
//-------------------------------------------------------------------------
// This is a base class to create a sub-editor for a given resource type that runs within the resource editor

namespace KRG
{
    class KRG_TOOLS_CORE_API ResourceEditorWorkspace
    {

    public:

        ResourceEditorWorkspace( EditorModel* pModel, ResourceID const& resourceID );

        virtual ~ResourceEditorWorkspace() { m_pResourceSystem->UnloadResource( m_pBaseResource ); }

        // Get the shared workspace name
        virtual char const* GetWorkspaceName() const = 0;

        // Get the shared viewport name for this workspace
        char const* GetViewportWindowName() const;

        // Get a unique shorter, display friendly name for the resource shown in the workspace
        inline char const* GetResourceDisplayName() const { return m_resourceDisplayName.c_str(); }

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
        virtual void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) = 0;

        // Should this workspace display a viewport?
        virtual bool RequiresViewportWindow() const { return true; }

        // Has any modifications been made to this file?
        virtual bool IsDirty() const { return false; }

        // Optional: Save functionality for files that support it
        inline bool Save()
        {
            bool result = true;
            if ( IsDirty() )
            {
                result = OnSave();
            }
            return result;
        }

        // Resource Status
        inline bool IsLoading() const { return m_pBaseResource.IsLoading(); }
        inline bool IsUnloaded() const { return m_pBaseResource.IsUnloaded(); }
        inline bool IsLoaded() const { return m_pBaseResource.IsLoaded(); }
        inline bool IsWaitingForResource() const { return IsLoading() || IsUnloaded(); }
        inline bool HasLoadingFailed() const { return m_pBaseResource.HasLoadingFailed(); }

    protected:

        ResourceEditorWorkspace& operator=( ResourceEditorWorkspace const& ) = delete;
        ResourceEditorWorkspace( ResourceEditorWorkspace const& ) = delete;

        virtual bool OnSave() { return false; }

        // Called when this file is set as the active one in the editor tab well
        virtual void Activate( EntityWorld* pPreviewWorld ) = 0;

        // Called when we switch away from this file being active, allows up to clean up the preview scene
        virtual void Deactivate( EntityWorld* pPreviewWorld ) = 0;

    protected:

        String                              m_resourceDisplayName;
        String                              m_windowName;
        String                              m_viewportName;
        FileSystem::Path                    m_filePath;
        Resource::ResourcePtr               m_pBaseResource;
        EditorModel*                        m_pModel = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;

    private:

        bool                                m_isActive = false;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class TResourceEditorWorkspace : public ResourceEditorWorkspace
    {
        static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derived from IResource" );

    public:

        TResourceEditorWorkspace( EditorModel* pModel, ResourceID const& resourceID )
            : ResourceEditorWorkspace( pModel, resourceID )
            , m_pResource( m_pBaseResource )
        {}

        virtual ResourceTypeID GetResourceTypeID() const override { return T::GetStaticResourceTypeID(); }
        virtual char const* GetResourceTypeFriendlyName() const override { return ""; }

    private:

        TResourceEditorWorkspace& operator=( TResourceEditorWorkspace const& ) = delete;
        TResourceEditorWorkspace( TResourceEditorWorkspace const& ) = delete;

    protected:

        TResourcePtr<T>                     m_pResource;
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
        static ResourceEditorWorkspace* TryCreateWorkspace( EditorModel* pModel, ResourceID const& resourceID );

    protected:

        // Get the resource type this factory supports
        virtual ResourceTypeID GetSupportedResourceTypeID() const = 0;

        // Virtual method that will create a workspace if the resource ID matches the appropriate types
        virtual ResourceEditorWorkspace* CreateWorkspace( EditorModel* pModel, ResourceID const& resourceID ) const = 0;
    };
}

//-------------------------------------------------------------------------
//  Macro to create a resource workspace factory
//-------------------------------------------------------------------------
// Use in a CPP to define a factory e.g., KRG_RESOURCE_WORKSPACE_FACTORY( SkeletonWorkspaceFactory, Skeleton, SkeletonResourceEditor );
    
#define KRG_RESOURCE_WORKSPACE_FACTORY( FactoryName, ResourceClass, EditorClass )\
class FactoryName final : public ResourceEditorWorkspaceFactory\
{\
    virtual ResourceTypeID GetSupportedResourceTypeID() const override { return ResourceClass::GetStaticResourceTypeID(); }\
    virtual ResourceEditorWorkspace* CreateWorkspace( EditorModel* pModel, ResourceID const& resourceID ) const override\
    {\
        KRG_ASSERT( resourceID.GetResourceTypeID() == ResourceClass::GetStaticResourceTypeID() );\
        return KRG::New<EditorClass>( pModel, resourceID );\
    }\
};\
static FactoryName g_##FactoryName;
