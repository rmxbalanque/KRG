#include "Editor_DataBrowserTreeView.h"
#include "Editor_Model.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    DataBrowserTreeItem::DataBrowserTreeItem( char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, ResourcePath const& resourcePath, ResourceTypeID resourceTypeID )
        : TreeViewItem( pName, hierarchyLevel )
        , m_path( path )
        , m_resourcePath( resourcePath )
        , m_resourceTypeID( resourceTypeID )
        , m_type( path.IsFile() ? Type::File : Type::Directory )
    {
        KRG_ASSERT( m_path.IsValid() );
        KRG_ASSERT( m_resourcePath.IsValid() );

        // Directories are not allowed to have resource type IDs set
        if ( IsDirectory() )
        {
            KRG_ASSERT( !resourceTypeID.IsValid() );
        }

        // Set the unique ID
        m_uniqueID = Hash::GetHash32( path.c_str() );
    }

    void DataBrowserTreeItem::DrawControls()
    {
        static Color const resourceColor = Colors::GreenYellow;

        if ( IsFile() )
        {
            if ( IsRawFile() )
            {
                ImGui::Text( GetFilePath().GetExtension() );
            }
            else // Resource file
            {
                ImGui::TextColored( resourceColor.ToFloat4(), GetFilePath().GetExtension() );
            }
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
            ImGui::SetClipboardText( m_resourcePath.c_str() );
        }
    }

    //-------------------------------------------------------------------------

    DataBrowserTreeView::DataBrowserTreeView( EditorModel* pModel )
        : m_pModel( pModel )
        , m_dataDirectoryPath( pModel->GetRawResourceDirectory() )
        , m_dataDirectoryPathDepth( pModel->GetRawResourceDirectory().GetPathDepth() )
    {
        KRG_ASSERT( m_pModel != nullptr );
        KRG_ASSERT( FileSystem::Exists( m_dataDirectoryPath ) );

        // Create root node
        KRG_ASSERT( m_pRoot == nullptr );
        m_pRoot = KRG::New<DataBrowserTreeItem>( "Data", -1, m_dataDirectoryPath, ResourcePath::FromFileSystemPath( m_dataDirectoryPath, m_dataDirectoryPath ) );
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
    }

    void DataBrowserTreeView::Update( UpdateContext const& context )
    {
        m_fileSystemWatcher.Update();
    }

    void DataBrowserTreeView::RebuildBrowserTree()
    {
        KRG_ASSERT( m_pRoot != nullptr );

        // Record current state
        //-------------------------------------------------------------------------

        TVector<uint32> originalExpandedItems;
        originalExpandedItems.reserve( GetNumItems() );

        auto RecordItemState = [ &originalExpandedItems] ( TreeViewItem const* pItem )
        {
            if ( pItem->IsExpanded() )
            {
                originalExpandedItems.emplace_back( pItem->GetUniqueID() );
            }
        };

        ForEachItemConst( RecordItemState );

        uint32 const activeItemID = m_pActiveItem != nullptr ? m_pActiveItem->GetUniqueID() : 0;
        uint32 const selectedItemID = m_pSelectedItem != nullptr ? m_pSelectedItem->GetUniqueID() : 0;

        // Rebuild Tree
        //-------------------------------------------------------------------------

        m_pActiveItem = nullptr;
        m_pSelectedItem = nullptr;
        m_pRoot->DestroyChildren();
        m_pRoot->SetExpanded( true );

        if ( !FileSystem::GetDirectoryContents( m_dataDirectoryPath, m_foundPaths, FileSystem::DirectoryReaderOutput::OnlyFiles, FileSystem::DirectoryReaderMode::Expand ) )
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
                if ( !m_pModel->GetTypeRegistry()->IsRegisteredResourceType( resourceTypeID ) )
                {
                    resourceTypeID = ResourceTypeID();
                }
            }

            // Create file item
            parentItem.CreateChild<DataBrowserTreeItem>( path.GetFileName().c_str(), parentItem.GetHierarchyLevel() + 1, path, ResourcePath::FromFileSystemPath( m_dataDirectoryPath, path ), resourceTypeID );
        }

        // Restore original state
        //-------------------------------------------------------------------------

        auto RestoreItemState = [&originalExpandedItems] ( TreeViewItem* pItem )
        {
            if ( VectorContains( originalExpandedItems, pItem->GetUniqueID() ) )
            {
                pItem->SetExpanded( true );
            }
        };

        ForEachItem( RestoreItemState );

        auto FindActiveItem = [activeItemID] ( TreeViewItem const* pItem ) { return pItem->GetUniqueID() == activeItemID; };
        m_pActiveItem = m_pRoot->SearchChildren( FindActiveItem );

        auto FindSelectedItem = [selectedItemID] ( TreeViewItem const* pItem ) { return pItem->GetUniqueID() == selectedItemID; };
        m_pSelectedItem = m_pRoot->SearchChildren( FindSelectedItem );

        RefreshVisualState();
    }

    TreeViewItem& DataBrowserTreeView::FindOrCreateParentForItem( FileSystem::Path const& path )
    {
        KRG_ASSERT( path.IsFile() );

        TreeViewItem* pCurrentItem = m_pRoot;
        FileSystem::Path directoryPath = m_dataDirectoryPath;
        TInlineVector<String, 10> splitPath = path.Split();

        //-------------------------------------------------------------------------

        int32 const pathDepth = (int32) splitPath.size();
        for ( int32 i = m_dataDirectoryPathDepth + 1; i < pathDepth; i++ )
        {
            directoryPath.Append( splitPath[i] );

            StringID const ID( splitPath[i] );
            auto searchPredicate = [&ID] ( TreeViewItem const* pItem ) { return pItem->GetLabelID() == ID; };

            auto pFoundChildItem = pCurrentItem->FindChild( searchPredicate );
            if ( pFoundChildItem == nullptr )
            {
                auto pItem = pCurrentItem->CreateChild<DataBrowserTreeItem>( splitPath[i].c_str(), pCurrentItem->GetHierarchyLevel() + 1, directoryPath, ResourcePath::FromFileSystemPath( m_dataDirectoryPath, directoryPath ) );
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