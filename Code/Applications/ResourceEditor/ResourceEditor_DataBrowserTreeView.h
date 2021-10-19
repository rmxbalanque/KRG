#pragma once
#include "Tools/Core/Trees/TreeView.h"
#include "Tools/Core/FileSystem/FileSystemWatcher.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Resource/ResourcePath.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceEditorModel;

    //-------------------------------------------------------------------------

    class DataBrowserTreeItem : public TreeViewItem
    {
    public:

        enum class Type
        {
            Directory = 0,
            File,
        };

    public:

        DataBrowserTreeItem( char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, ResourcePath const& resourcePath, ResourceTypeID resourceTypeID = ResourceTypeID() );

        virtual void DrawControls() override;
        virtual bool SupportsContextMenu() const override { return true; }
        virtual void DrawContextMenu() override;
        virtual bool CanBeSetActive() const override { return false; }

        // File Info
        //-------------------------------------------------------------------------

        inline bool IsFile() const { return m_type == Type::File; }
        inline bool IsDirectory() const { return m_type == Type::Directory; }
        inline FileSystem::Path const& GetFilePath() const { return m_path; }
        inline ResourcePath const& GetResourcePath() const { return m_resourcePath; }

        // Resource Info
        //-------------------------------------------------------------------------

        inline bool IsRawFile() const { KRG_ASSERT( IsFile() ); return !m_resourceTypeID.IsValid(); }
        inline bool IsResourceFile() const { KRG_ASSERT( IsFile() ); return m_resourceTypeID.IsValid(); }
        inline ResourceID GetResourceID() const { KRG_ASSERT( IsResourceFile() ); return ResourceID( m_resourcePath ); }
        inline ResourceTypeID const& GetResourceTypeID() const { KRG_ASSERT( IsFile() ); return m_resourceTypeID; }

        template<typename T>
        inline bool IsResourceOfType() const
        {
            static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derive from IResource" );
            return m_resourceTypeID == T::GetStaticResourceTypeID(); 
        }

    protected:

        FileSystem::Path                        m_path;
        ResourcePath                            m_resourcePath;
        ResourceTypeID                          m_resourceTypeID;
        Type                                    m_type;
    };

    //-------------------------------------------------------------------------

    class DataBrowserTreeView final : public TreeView, public FileSystem::IFileSystemChangeListener
    {

    public:

        DataBrowserTreeView( ResourceEditorModel* pModel );
        virtual ~DataBrowserTreeView();

        void RebuildBrowserTree();
        void Update( UpdateContext const& context );

    private:

        // File system listener
        virtual void OnFileCreated( FileSystem::Path const& path ) override final { RebuildBrowserTree(); }
        virtual void OnFileDeleted( FileSystem::Path const& path ) override final { RebuildBrowserTree(); }
        virtual void OnFileRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override final { RebuildBrowserTree(); }
        virtual void OnDirectoryCreated( FileSystem::Path const& path ) override final{ RebuildBrowserTree(); }
        virtual void OnDirectoryDeleted( FileSystem::Path const& path ) override final { RebuildBrowserTree(); }
        virtual void OnDirectoryRenamed( FileSystem::Path const& oldPath, FileSystem::Path const& newPath ) override final { RebuildBrowserTree(); }

        TreeViewItem& FindOrCreateParentForItem( FileSystem::Path const& path );

    private:

        ResourceEditorModel*                        m_pModel = nullptr;
        FileSystem::Path                            m_dataDirectoryPath;
        int32                                       m_dataDirectoryPathDepth;
        FileSystem::FileSystemWatcher               m_fileSystemWatcher;
        TVector<FileSystem::Path>                   m_foundPaths;
    };
}