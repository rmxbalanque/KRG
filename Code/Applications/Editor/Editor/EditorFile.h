#pragma once
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EntityWorld;
    class Entity;
    class UpdateContext;
}

namespace KRG::Render { class ViewportManager; }

//-------------------------------------------------------------------------
// Base File View
//-------------------------------------------------------------------------
// This is a base class to draw a open file editor and set up the preview

namespace KRG
{
    class EditorFile
    {
    public:

        virtual ~EditorFile() = default;

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
        inline bool IsDirty() const { return m_isDirty; }

        // Optional: Save functionality for files that support it
        inline bool Save()
        {
            bool result = true;
            if ( m_isDirty )
            {
                result = OnSave();
                m_isDirty = false;
            }
            return result;
        }

    protected:

        virtual bool OnSave() { return false; }

    protected:

        String                              m_displayName;
        FileSystem::Path                    m_filePath;
        bool                                m_isDirty = false;
    };
}

//-------------------------------------------------------------------------
// Resource File View
//-------------------------------------------------------------------------
// Helper to manage loading and unloading of resources

namespace KRG
{
    template<typename T>
    class TResourceFile : public EditorFile
    {
        static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derived from IResource" );

    public:

        TResourceFile( FileSystem::Path const& sourceDataDirectory, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
            : m_pResourceSystem( pResourceSystem )
            , m_pResource( resourceID )
        {
            KRG_ASSERT( m_pResourceSystem != nullptr && resourceID.IsValid() );
            KRG_ASSERT( resourceID.GetResourceTypeID() == T::GetStaticResourceTypeID() );
            m_pResourceSystem->LoadResource( m_pResource );
            m_filePath = resourceID.GetDataPath().ToFileSystemPath( sourceDataDirectory );
            m_displayName = m_filePath.GetFileNameWithoutExtension();
        }

        virtual ~TResourceFile()
        {
            m_pResourceSystem->UnloadResource( m_pResource );
        }

        inline bool IsLoading() const { return m_pResource.IsLoading(); }
        inline bool IsLoaded() const { return m_pResource.IsLoaded(); }
        inline bool HasLoadingFailed() const { return m_pResource.HasLoadingFailed(); }

    private:

        TResourceFile& operator=( TResourceFile const& ) = delete;
        TResourceFile( TResourceFile const& ) = delete;

    protected:

        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
        TResourcePtr<T>                     m_pResource;
    };
}