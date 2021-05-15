#include "DataBrowserTreeView.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Editor
{
    DataBrowserTreeItem::DataBrowserTreeItem( char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, DataPath const& dataPath, ResourceTypeID resourceTypeID )
        : TreeViewItem( StringID( pName ), pName, hierarchyLevel )
        , m_path( path )
        , m_dataPath( dataPath )
        , m_resourceTypeID( resourceTypeID )
        , m_type( path.IsFilePath() ? Type::File : Type::Directory )
    {
        KRG_ASSERT( m_path.IsValid() );
        KRG_ASSERT( m_dataPath.IsValid() );

        // Directories are not allowed to have resource type IDs set
        if ( IsDirectory() )
        {
            KRG_ASSERT( !resourceTypeID.IsValid() );
        }
    }

    void DataBrowserTreeItem::DrawControls()
    {
        if ( IsFile() )
        {
            ImGui::Text( GetPath().GetExtension() );
        }
    }

    void DataBrowserTreeItem::DrawContextMenu()
    {
        if ( ImGui::Selectable( "Open In Explorer" ) )
        {
            FileSystem::OpenInExplorer( m_path );
        }

        if ( ImGui::Selectable( "Copy File Path" ) )
        {
            ImGui::SetClipboardText( m_path.c_str() );
        }

        if ( ImGui::Selectable( "Copy Data Path" ) )
        {
            ImGui::SetClipboardText( m_dataPath.c_str() );
        }
    }

    //-------------------------------------------------------------------------

    DataBrowserTreeView::DataBrowserTreeView( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& dataDirectoryPath, TVector<String> const& fileRestrictions )
        : m_typeRegistry( typeRegistry )
        , m_dataDirectoryPath( dataDirectoryPath )
        , m_dataDirectoryPathDepth( dataDirectoryPath.GetPathDepth() )
        , m_fileRestrictions( fileRestrictions )
    {
        KRG_ASSERT( m_dataDirectoryPath.IsValid() );
        KRG_ASSERT( dataDirectoryPath.ExistsAndIsDirectory() );

        // Create root node
        KRG_ASSERT( m_pRoot == nullptr );
        m_pRoot = KRG::New<DataBrowserTreeItem>( "Data", -1, m_dataDirectoryPath, DataPath::FromFileSystemPath( m_dataDirectoryPath, m_dataDirectoryPath ) );
        m_pRoot->SetExpanded( true );

        // Start file watcher
        m_fileSystemWatcher.RegisterChangeListener( this );
        m_fileSystemWatcher.StartWatching( m_dataDirectoryPath );

        //-------------------------------------------------------------------------

        SetItemControlColumnWidth( 100 );
    }

    DataBrowserTreeView::~DataBrowserTreeView()
    {
        KRG_ASSERT( m_pRoot != nullptr );

        // Stop file watcher
        m_fileSystemWatcher.StopWatching();
        m_fileSystemWatcher.UnregisterChangeListener( this );

        // Clean params
        m_dataDirectoryPath = FileSystem::Path();
        m_dataDirectoryPathDepth = -1;
        m_fileRestrictions.clear();
    }

    void DataBrowserTreeView::Update( UpdateContext const& context )
    {
        m_fileSystemWatcher.Update();
    }

    void DataBrowserTreeView::RebuildBrowserTree()
    {
        KRG_ASSERT( m_pRoot != nullptr );

        //-------------------------------------------------------------------------
        // TODO: optimize this and maintain active/selected items

        m_pActiveItem = nullptr;
        m_pSelectedItem = nullptr;

        //-------------------------------------------------------------------------

        m_pRoot->DestroyChildren();
        m_pRoot->SetExpanded( true );

        if ( !FileSystem::GetDirectoryContents( m_dataDirectoryPath, m_foundPaths, FileSystem::DirectoryReaderOutput::OnlyFiles, FileSystem::DirectoryReaderMode::Expand, m_fileRestrictions ) )
        {
            KRG_HALT();
        }

        //-------------------------------------------------------------------------

        for ( auto const& path : m_foundPaths )
        {
            auto& parentItem = FindOrCreateParentForItem( path );

            // Check if this is a registered resource
            ResourceTypeID resourceTypeID;
            char const * pExtension = path.GetExtension();
            if ( strlen( pExtension ) <= 4 )
            {
                resourceTypeID = ResourceTypeID( pExtension );
                if ( !m_typeRegistry.IsRegisteredResourceType( resourceTypeID ) )
                {
                    resourceTypeID = ResourceTypeID();
                }
            }

            // Create file item
            ChildItemFactory( &parentItem, path.GetFileName().c_str(), parentItem.GetHierarchyLevel() + 1, path, DataPath::FromFileSystemPath( m_dataDirectoryPath, path ), resourceTypeID );
        }

        //-------------------------------------------------------------------------

        RefreshVisualState();
    }

    TreeViewItem& DataBrowserTreeView::FindOrCreateParentForItem( FileSystem::Path const& path )
    {
        KRG_ASSERT( path.IsFilePath() );

        TreeViewItem* pCurrentItem = m_pRoot;
        FileSystem::Path directoryPath = m_dataDirectoryPath;
        TInlineVector<String, 10> splitPath = path.Split();

        //-------------------------------------------------------------------------

        int32 const pathDepth = (int32) splitPath.size();
        for ( int32 i = m_dataDirectoryPathDepth + 1; i < pathDepth; i++ )
        {
            directoryPath.Append( splitPath[i] );

            StringID const ID( splitPath[i] );
            auto searchPredicate = [&ID] ( TreeViewItem const* pItem ) { return pItem->GetStringID() == ID; };

            auto pFoundChildItem = pCurrentItem->FindChild( searchPredicate );
            if ( pFoundChildItem == nullptr )
            {
                auto pItem = ChildItemFactory( pCurrentItem, splitPath[i].c_str(), pCurrentItem->GetHierarchyLevel() + 1, directoryPath, DataPath::FromFileSystemPath( m_dataDirectoryPath, directoryPath ) );
                pCurrentItem = pItem;
            }
            else
            {
                pCurrentItem = pFoundChildItem;
            }
        }

        //-------------------------------------------------------------------------

        return *pCurrentItem;
    }
}