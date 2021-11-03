#pragma once

#include "EditorWorkspace.h"
#include "Tools/Core/Helpers/GlobalRegistryBase.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------
// Resource Editor Workspace
//-------------------------------------------------------------------------
// This is a base class to create a sub-editor for a given resource type that runs within the resource editor

namespace KRG
{
    template<typename T>
    class TResourceEditorWorkspace : public EditorWorkspace
    {
        static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derived from IResource" );

    public:

        // Specify whether to initially load the resource, this is not necessary for all editors
        TResourceEditorWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
            : EditorWorkspace( context, pWorld )
            , m_pResource( resourceID )
        {
            KRG_ASSERT( resourceID.IsValid() );

            FileSystem::Path const filePath = resourceID.GetResourcePath().ToFileSystemPath( context.m_sourceResourceDirectory );
            m_displayName.sprintf( "%s##%u", filePath.GetFileNameWithoutExtension().c_str(), resourceID.GetResourcePath().GetID() );
            m_editorContext.m_pResourceSystem->LoadResource( m_pResource );
        }

        ~TResourceEditorWorkspace()
        {
            if ( !m_pResource.IsUnloaded() )
            {
                m_editorContext.m_pResourceSystem->UnloadResource( m_pResource );
            }
        }

        virtual char const* GetDisplayName() const override { return m_displayName.c_str(); }
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

        String                              m_displayName;
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
class factoryName final : public ResourceEditorWorkspaceFactory\
{\
    virtual ResourceTypeID GetSupportedResourceTypeID() const override { return resourceClass::GetStaticResourceTypeID(); }\
    virtual EditorWorkspace* CreateWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID ) const override\
    {\
        KRG_ASSERT( resourceID.GetResourceTypeID() == resourceClass::GetStaticResourceTypeID() );\
        return KRG::New<workspaceClass>( context, pWorld, resourceID );\
    }\
};\
static factoryName g_##factoryName;