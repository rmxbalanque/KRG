#pragma once

#include "EditorWorkspace.h"
#include "Tools/Core/Helpers/GlobalRegistryBase.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource { struct ResourceDescriptor; }

    //-------------------------------------------------------------------------
    // Resource Editor Workspace
    //-------------------------------------------------------------------------
    // This is a base class to create a sub-editor for a given resource type that runs within the resource editor

    template<typename T>
    class TResourceWorkspace : public EditorWorkspace
    {
        static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derived from IResource" );

    public:

        // Specify whether to initially load the resource, this is not necessary for all editors
        TResourceWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID, bool shouldLoadResource = true )
            : EditorWorkspace( context, pWorld, resourceID.GetResourcePath().ToFileSystemPath( context.m_sourceResourceDirectory ).GetFileNameWithoutExtension() )
            , m_pResource( resourceID )
        {
            KRG_ASSERT( resourceID.IsValid() );

            if ( shouldLoadResource )
            {
                m_editorContext.m_pResourceSystem->LoadResource( m_pResource );
            }
        }

        ~TResourceWorkspace()
        {
            if ( !m_pResource.IsUnloaded() )
            {
                m_editorContext.m_pResourceSystem->UnloadResource( m_pResource );
            }
        }

        virtual uint32 GetID() const override{ return m_pResource.GetResourceID().GetID(); }

        // Resource Status
        inline bool IsLoading() const { return m_pResource.IsLoading(); }
        inline bool IsUnloaded() const { return m_pResource.IsUnloaded(); }
        inline bool IsLoaded() const { return m_pResource.IsLoaded(); }
        inline bool IsWaitingForResource() const { return m_isHotReloading || IsLoading() || IsUnloaded() || m_pResource.IsUnloading(); }
        inline bool HasLoadingFailed() const { return m_pResource.HasLoadingFailed(); }

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

    protected:

        String                              m_title;
        TResourcePtr<T>                     m_pResource;

    private:

        bool                                m_isHotReloading = false;
    };

    //-------------------------------------------------------------------------
    // Generic Resource Workspace
    //-------------------------------------------------------------------------
    // Created for any resources without a custom workspace associated

    class GenericResourceWorkspace final : public EditorWorkspace
    {
    public:

        GenericResourceWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID );
        ~GenericResourceWorkspace();

    private:

        virtual uint32 GetID() const override{ return m_descriptorID.GetID(); }
        virtual bool HasViewportWindow() const { return false; }
        virtual void Initialize() override;
        virtual void Shutdown() override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual bool IsDirty() const override { return m_propertyGrid.IsDirty(); }
        virtual bool Save() override;

    private:

        String                              m_descriptorWindowName;
        ResourceID                          m_descriptorID;
        FileSystem::Path                    m_descriptorPath;
        PropertyGrid                        m_propertyGrid;
        Resource::ResourceDescriptor*       m_pDescriptor = nullptr;
    };

    //-------------------------------------------------------------------------
    // Resource Workspace Factory
    //-------------------------------------------------------------------------
    // Used to spawn the appropriate factory

    class KRG_TOOLS_CORE_API ResourceWorkspaceFactory : public TGlobalRegistryBase<ResourceWorkspaceFactory>
    {
        KRG_DECLARE_GLOBAL_REGISTRY( ResourceWorkspaceFactory );

    public:

        static bool HasCustomWorkspace( ResourceTypeID const& resourceTypeID );
        static EditorWorkspace* TryCreateWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID );

    protected:

        // Get the resource type this factory supports
        virtual ResourceTypeID GetSupportedResourceTypeID() const = 0;

        // Virtual method that will create a workspace if the resource ID matches the appropriate types
        virtual EditorWorkspace* CreateWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID ) const = 0;
    };
}

//-------------------------------------------------------------------------
//  Macro to create a resource workspace factory
//-------------------------------------------------------------------------
// Use in a CPP to define a factory e.g., KRG_RESOURCE_WORKSPACE_FACTORY( SkeletonWorkspaceFactory, Skeleton, SkeletonResourceEditor );
    
#define KRG_RESOURCE_WORKSPACE_FACTORY( factoryName, resourceClass, workspaceClass )\
class factoryName final : public ResourceWorkspaceFactory\
{\
    virtual ResourceTypeID GetSupportedResourceTypeID() const override { return resourceClass::GetStaticResourceTypeID(); }\
    virtual EditorWorkspace* CreateWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID ) const override\
    {\
        KRG_ASSERT( resourceID.GetResourceTypeID() == resourceClass::GetStaticResourceTypeID() );\
        return KRG::New<workspaceClass>( context, pWorld, resourceID );\
    }\
};\
static factoryName g_##factoryName;