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
    class EditorModel;
    class ResourceDescriptorCreator;

    //-------------------------------------------------------------------------

    class ResourceBrowserTreeItem : public TreeViewItem
    {
    public:

        enum class Type
        {
            Directory = 0,
            File,
        };

    public:

        ResourceBrowserTreeItem( char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, ResourcePath const& resourcePath, ResourceTypeID resourceTypeID = ResourceTypeID() );

        virtual bool SupportsContextMenu() const override { return true; }
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

    class ResourceBrowserTreeView final : public TreeView, public FileSystem::IFileSystemChangeListener
    {

    public:

        ResourceBrowserTreeView( EditorModel* pModel );
        virtual ~ResourceBrowserTreeView();

        void RebuildBrowserTree();

    private:

        TreeViewItem& FindOrCreateParentForItem( FileSystem::Path const& path );

        virtual void DrawExtra() override;
        virtual void DrawItemControls( TreeViewItem* pItem ) override;
        virtual void DrawItemContextMenu( TreeViewItem* pItem ) override;

        void DrawCreateNewDescriptorMenu( FileSystem::Path const& path );

    private:

        EditorModel*                                m_pModel = nullptr;
        FileSystem::Path                            m_sourceDirectoryPath;
        int32                                       m_dataDirectoryPathDepth;
        TVector<FileSystem::Path>                   m_foundPaths;

        ResourceDescriptorCreator*                  m_pResourceDescriptorCreator = nullptr;
    };
}