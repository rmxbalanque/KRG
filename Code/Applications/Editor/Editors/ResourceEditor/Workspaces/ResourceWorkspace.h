#pragma once
#include "Applications/Editor/Editors/EditorModel.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

struct ImGuiWindowClass;

namespace KRG
{
    class EntityWorld;
    class Entity;
    class UpdateContext;
}

namespace KRG::Render { class ViewportManager; }

//-------------------------------------------------------------------------
// File Tab Base
//-------------------------------------------------------------------------
// This is a base class to draw a open file editor and set up the preview

namespace KRG::Resource
{
    class ResourceEditorWorkspace
    {
    public:

        ResourceEditorWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID );

        virtual ~ResourceEditorWorkspace() { m_pResourceSystem->UnloadResource( m_pBaseResource ); }

        // Get a shorter, display friendly name for this file
        virtual char const* GetWorkspaceName() const = 0;

        // Get a shorter, display friendly name for this file
        inline char const* GetResourceDisplayName() const { return m_resourceDisplayName.c_str(); }

        // Get a shorter, display friendly name for this file
        inline char const* GetWindowName() const { return m_windowName.c_str(); }

        // Get the actual file-path for the file
        inline FileSystem::Path const& GetFilePath() const { return m_filePath; }

        // Get the resource type ID of the view resource
        virtual ResourceTypeID GetResourceTypeID() const { return ResourceTypeID::Unknown; }

        // Return the friendly resource type name for the viewed resource
        virtual char const* GetResourceTypeFriendlyName() const { return ""; }

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
        FileSystem::Path                    m_filePath;
        bool                                m_isActive = false;
        Resource::ResourcePtr               m_pBaseResource;
        EditorModel*                        m_pModel = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class TResourceWorkspace : public ResourceEditorWorkspace
    {
        static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derived from IResource" );

    public:

        TResourceWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
            : ResourceEditorWorkspace( pModel, pResourceSystem, resourceID )
            , m_pResource( m_pBaseResource )
        {}

        virtual ResourceTypeID GetResourceTypeID() const override { return T::GetStaticResourceTypeID(); }
        virtual char const* GetResourceTypeFriendlyName() const override { return ""; }

    private:

        TResourceWorkspace& operator=( TResourceWorkspace const& ) = delete;
        TResourceWorkspace( TResourceWorkspace const& ) = delete;

    protected:

        TResourcePtr<T>                     m_pResource;
    };
}