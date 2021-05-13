#pragma once
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EntityWorld;
    class Entity;
    class UpdateContext;
    class EditorModel;
}

namespace KRG::Render { class ViewportManager; }

//-------------------------------------------------------------------------
// File Tab Base
//-------------------------------------------------------------------------
// This is a base class to draw a open file editor and set up the preview

namespace KRG
{
    class EditorFileTab
    {
    public:

        virtual ~EditorFileTab() = default;

        // Get a shorter, display friendly name for this file
        inline char const* GetDisplayName() const { return m_displayName.c_str(); }

        // Get the actual file-path for the file
        inline FileSystem::Path const& GetFilePath() const { return m_filePath; }

        // Called when this file is set as the active one in the toolkit
        virtual void SetActive( EntityWorld* pPreviewWorld ) = 0;

        // Called when we switch away from this file being active, allows up to clean up the preview scene
        virtual void SetInactive( EntityWorld* pPreviewWorld ) = 0;

        // Draw any UI required for this file within the already allocated window
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) = 0;

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

    protected:

        virtual bool OnSave() { return false; }

    protected:

        String                              m_displayName;
        FileSystem::Path                    m_filePath;
    };
}

//-------------------------------------------------------------------------
// File tab for an open resource
//-------------------------------------------------------------------------
// Helper to manage loading and unloading of resources

namespace KRG
{
    template<typename T>
    class TResourceFileTab : public EditorFileTab
    {
        static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derived from IResource" );

    public:

        TResourceFileTab( EditorModel* pModel, FileSystem::Path const& sourceDataDirectory, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
            : m_pResourceSystem( pResourceSystem )
            , m_pResource( resourceID )
            , m_pModel( pModel )
        {
            KRG_ASSERT( m_pResourceSystem != nullptr && resourceID.IsValid() );
            KRG_ASSERT( resourceID.GetResourceTypeID() == T::GetStaticResourceTypeID() );
            m_pResourceSystem->LoadResource( m_pResource );
            m_filePath = resourceID.GetDataPath().ToFileSystemPath( sourceDataDirectory );
            m_displayName = m_filePath.GetFileNameWithoutExtension();
        }

        virtual ~TResourceFileTab()
        {
            m_pResourceSystem->UnloadResource( m_pResource );
        }

        inline bool IsLoading() const { return m_pResource.IsLoading(); }
        inline bool IsLoaded() const { return m_pResource.IsLoaded(); }
        inline bool HasLoadingFailed() const { return m_pResource.HasLoadingFailed(); }

    private:

        TResourceFileTab& operator=( TResourceFileTab const& ) = delete;
        TResourceFileTab( TResourceFileTab const& ) = delete;

    protected:

        EditorModel*                        m_pModel = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
        TResourcePtr<T>                     m_pResource;
    };
}