#pragma once

#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Editor Model
    //-------------------------------------------------------------------------
    // Base class for an editor model, this helps to manage global editor state across multiple tools
    // Each editor will create its own model and supply it to its various tools

    class EditorModel
    {

    public:

        virtual ~EditorModel();

        virtual void Initialize( UpdateContext const& context );
        virtual void Shutdown( UpdateContext const& context );
        virtual void Update( UpdateContext const& context ) {}

        // Core Systems and Settings
        //-------------------------------------------------------------------------

        inline FileSystem::Path const& GetSourceDataDirectory() const { return m_sourceDataDirectory; }
        inline FileSystem::Path const& GetCompiledDataDirectory() const { return m_compiledDataDirectory; }
        inline TypeSystem::TypeRegistry const& GetTypeRegistry() const { return *m_pTypeRegistry; }

        // Resource Management
        //-------------------------------------------------------------------------
        // The active resource is the resource that has "focus" and is currently being previewed or edited.

        void LoadResource( ResourceID const& resourceID );
        void UnloadResource( ResourceID const& resourceID );
        bool IsLoadedResource( ResourceID const& resourceID ) const;
        inline TVector<Resource::ResourcePtr> const& GetLoadedResources() const { return m_loadedResources; }
        inline Resource::ResourcePtr const& GetLoadedResource( ResourceID const& resourceID ) const;

        inline bool HasActiveResource() const { return m_activeResource.IsValid(); }
        inline ResourceID const& GetActiveResource() const { return m_activeResource; }
        Resource::ResourcePtr const& GetActiveResourcePtr() const;
        inline bool IsActiveResourceLoading() const { return GetActiveResourcePtr().IsLoading(); }
        void SetActiveResource( ResourceID const& resourceID );
        void ClearActiveResource();

    protected:

        // Called whenever we set a new acive resource
        virtual void OnSetActiveResource( ResourceID const& resourceID ) {}

        // Called just before we clear the active resource
        virtual void OnClearActiveResource() {}

    protected:

        FileSystem::Path                    m_sourceDataDirectory;
        FileSystem::Path                    m_compiledDataDirectory;

        // Core editor systems
        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;

        // Resource management
        TVector<Resource::ResourcePtr>      m_loadedResources;
        ResourceID                          m_activeResource;
    };
}