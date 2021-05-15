#pragma once
#include "Tools/Editor/Widgets/TreeView.h"
#include "Tools/Core/FileSystem/FileSystemWatcher.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/FileSystem/DataPath.h"

//-------------------------------------------------------------------------

namespace KRG::Editor
{
    class KRG_TOOLS_EDITOR_API DataBrowserTreeItem : public TreeViewItem
    {
    public:

        enum class Type
        {
            Directory = 0,
            File,
        };

    public:

        DataBrowserTreeItem( char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, DataPath const& dataPath, ResourceTypeID resourceTypeID = ResourceTypeID() );

        virtual void DrawControls() override;
        virtual bool SupportsContextMenu() const override { return true; }
        virtual void DrawContextMenu() override;
        virtual bool CanBeSetActive() const override { return IsFile(); }

        // File Info
        //-------------------------------------------------------------------------

        inline bool IsFile() const { return m_type == Type::File; }
        inline bool IsDirectory() const { return m_type == Type::Directory; }
        inline FileSystem::Path const& GetPath() const { return m_path; }
        inline DataPath const& GetDataPath() const { return m_dataPath; }

        // Resource Info
        //-------------------------------------------------------------------------

        inline bool IsRawFile() const { KRG_ASSERT( IsFile() ); return !m_resourceTypeID.IsValid(); }
        inline bool IsResourceFile() const { KRG_ASSERT( IsFile() ); return m_resourceTypeID.IsValid(); }
        inline ResourceID GetResourceID() const { KRG_ASSERT( IsResourceFile() ); return ResourceID( m_dataPath ); }
        inline ResourceTypeID const& GetResourceTypeID() const { KRG_ASSERT( IsFile() ); return m_resourceTypeID; }

        template<typename T>
        inline bool IsResourceOfType() const
        {
            static_assert( std::is_base_of<Resource::IResource, T>::value, "T must derive from IResource" );
            return m_resourceTypeID == T::GetStaticResourceTypeID(); 
        }

    protected:

        FileSystem::Path                        m_path;
        DataPath                                m_dataPath;
        ResourceTypeID                          m_resourceTypeID;
        Type                                    m_type;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_EDITOR_API DataBrowserTreeView : public TreeView, public FileSystem::IFileSystemChangeListener
    {

    public:

        virtual ~DataBrowserTreeView();

        void RebuildBrowserTree();
        void Update( UpdateContext const& context );

    protected:

        DataBrowserTreeView( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& dataDirectoryPath, TVector<String> const& fileRestrictions = TVector<String>() );

        virtual DataBrowserTreeItem* ChildItemFactory( TreeViewItem* pParentItem, char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, DataPath const& dataPath, ResourceTypeID resourceTypeID = ResourceTypeID() )
        {
            KRG_ASSERT( pParentItem != nullptr );
            return pParentItem->CreateChild<DataBrowserTreeItem>( pName, hierarchyLevel, path, dataPath, resourceTypeID );
        }

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

        TypeSystem::TypeRegistry const&             m_typeRegistry;
        FileSystem::Path                            m_dataDirectoryPath;
        int32                                       m_dataDirectoryPathDepth;
        TVector<String>                             m_fileRestrictions;
        FileSystem::FileSystemWatcher               m_fileSystemWatcher;
        TVector<FileSystem::Path>                   m_foundPaths;
    };

    //-------------------------------------------------------------------------

    template<class T>
    class TDataBrowserTreeView final : public DataBrowserTreeView
    {
        static_assert( std::is_base_of<DataBrowserTreeItem, T>::value, "T must derive from DataBrowserTreeItem" );

    public:

        TDataBrowserTreeView( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& dataDirectoryPath, TVector<String> const& fileRestrictions = TVector<String>() )
            : DataBrowserTreeView( typeRegistry, dataDirectoryPath, fileRestrictions) 
        {}

    private:

        virtual DataBrowserTreeItem* ChildItemFactory( TreeViewItem* pParentItem, char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, DataPath const& dataPath, ResourceTypeID resourceTypeID = ResourceTypeID() ) override final
        {
            KRG_ASSERT( pParentItem != nullptr );
            return pParentItem->CreateChild<T>( pName, hierarchyLevel, path, dataPath, resourceTypeID );
        }
    };
}