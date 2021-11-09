#include "ResourceBrowser_TreeView.h"
#include "Applications/Editor/Editor_Model.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"
#include "ResourceBrowser_DescriptorCreator.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceBrowserTreeItem::ResourceBrowserTreeItem( char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, ResourcePath const& resourcePath, ResourceTypeID resourceTypeID )
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

    //-------------------------------------------------------------------------

    ResourceBrowserTreeView::ResourceBrowserTreeView( EditorModel* pModel )
        : m_pModel( pModel )
        , m_dataDirectoryPath( pModel->GetSourceResourceDirectory() )
        , m_dataDirectoryPathDepth( pModel->GetSourceResourceDirectory().GetPathDepth() )
    {
        KRG_ASSERT( m_pModel != nullptr );
        KRG_ASSERT( FileSystem::Exists( m_dataDirectoryPath ) );

        // Create root node
        KRG_ASSERT( m_pRoot == nullptr );
        m_pRoot = KRG::New<ResourceBrowserTreeItem>( "Data", -1, m_dataDirectoryPath, ResourcePath::FromFileSystemPath( m_dataDirectoryPath, m_dataDirectoryPath ) );
        m_pRoot->SetExpanded( true );

        //-------------------------------------------------------------------------

        SetItemControlColumnWidth( 100 );
    }

    ResourceBrowserTreeView::~ResourceBrowserTreeView()
    {
        KRG_ASSERT( m_pRoot != nullptr );

        KRG::Delete( m_pResourceDescriptorCreator );

        // Clean params
        m_dataDirectoryPath = FileSystem::Path();
        m_dataDirectoryPathDepth = -1;
    }

    void ResourceBrowserTreeView::RebuildBrowserTree()
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
            parentItem.CreateChild<ResourceBrowserTreeItem>( path.GetFileName().c_str(), parentItem.GetHierarchyLevel() + 1, path, ResourcePath::FromFileSystemPath( m_dataDirectoryPath, path ), resourceTypeID );
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

    TreeViewItem& ResourceBrowserTreeView::FindOrCreateParentForItem( FileSystem::Path const& path )
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
                auto pItem = pCurrentItem->CreateChild<ResourceBrowserTreeItem>( splitPath[i].c_str(), pCurrentItem->GetHierarchyLevel() + 1, directoryPath, ResourcePath::FromFileSystemPath( m_dataDirectoryPath, directoryPath ) );
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

    //-------------------------------------------------------------------------

    void ResourceBrowserTreeView::DrawExtra()
    {
        if ( m_pResourceDescriptorCreator != nullptr )
        {
            if ( !m_pResourceDescriptorCreator->Draw() )
            {
                KRG::Delete( m_pResourceDescriptorCreator );
            }
        }
    }

    void ResourceBrowserTreeView::DrawItemControls( TreeViewItem* pItem )
    {
        static Color const resourceColor = Colors::GreenYellow;

        auto pResourceItem = (ResourceBrowserTreeItem*) pItem;

        if ( pResourceItem->IsFile() )
        {
            if ( pResourceItem->IsRawFile() )
            {
                ImGui::Text( pResourceItem->GetFilePath().GetExtension() );
            }
            else // Resource file
            {
                ImGui::TextColored( resourceColor.ToFloat4(), pResourceItem->GetFilePath().GetExtension() );
            }
        }
    }

    void ResourceBrowserTreeView::DrawItemContextMenu( TreeViewItem* pItem )
    {
        auto pResourceItem = (ResourceBrowserTreeItem*) pItem;

        if ( pResourceItem->GetFilePath().IsDirectory() )
        {
            DrawCreateNewDescriptorMenu( pResourceItem->GetFilePath() );
        }

        //-------------------------------------------------------------------------

        if ( ImGui::MenuItem( "Open In Explorer" ) )
        {
            FileSystem::OpenInExplorer( pResourceItem->GetFilePath() );
        }

        if ( ImGui::MenuItem( "Copy File Path" ) )
        {
            ImGui::SetClipboardText( pResourceItem->GetFilePath().c_str() );
        }

        if ( ImGui::MenuItem( "Copy Data Path" ) )
        {
            ImGui::SetClipboardText( pResourceItem->GetFilePath().c_str() );
        }
    }

    void ResourceBrowserTreeView::DrawCreateNewDescriptorMenu( FileSystem::Path const& path )
    {
        KRG_ASSERT( path.IsDirectory() );

        TypeSystem::TypeRegistry const* pTypeRegistry = m_pModel->GetTypeRegistry();
        TVector<TypeSystem::TypeInfo const*> descriptorTypeInfos = pTypeRegistry->GetAllDerivedTypes( Resource::ResourceDescriptor::GetStaticTypeID(), false, false );

        // Filter Type Info list
        //-------------------------------------------------------------------------

        for ( auto i = (int32) descriptorTypeInfos.size() - 1; i >= 0; i-- )
        {
            auto pRD = (Resource::ResourceDescriptor const*) descriptorTypeInfos[i]->GetDefaultInstance();
            if ( !pRD->IsUserCreateableDescriptor() )
            {
                descriptorTypeInfos.erase_unsorted( descriptorTypeInfos.begin() + i );
            }
        }

        auto sortPredicate = [pTypeRegistry] ( TypeSystem::TypeInfo const* const& pTypeInfoA, TypeSystem::TypeInfo const* const& pTypeInfoB )
        {
            auto pRDA = (Resource::ResourceDescriptor const*) pTypeInfoA->GetDefaultInstance();
            auto pRDB = (Resource::ResourceDescriptor const*) pTypeInfoB->GetDefaultInstance();

            auto pResourceInfoA = pTypeRegistry->GetResourceInfoForResourceType( pRDA->GetCompiledResourceTypeID() );
            auto pResourceInfoB = pTypeRegistry->GetResourceInfoForResourceType( pRDB->GetCompiledResourceTypeID() );
            return pResourceInfoA->m_friendlyName < pResourceInfoB->m_friendlyName;
        };

        eastl::sort( descriptorTypeInfos.begin(), descriptorTypeInfos.end(), sortPredicate );

        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Create New Descriptor" ) )
        {
            for ( auto pDescriptorTypeInfo : descriptorTypeInfos )
            {
                auto pDefaultInstance = Cast<Resource::ResourceDescriptor>( pDescriptorTypeInfo->GetDefaultInstance() );
                KRG_ASSERT( pDefaultInstance->IsUserCreateableDescriptor() );

                auto pResourceInfo = pTypeRegistry->GetResourceInfoForResourceType( pDefaultInstance->GetCompiledResourceTypeID() );
                if ( ImGui::MenuItem( pResourceInfo->m_friendlyName.c_str() ) )
                {
                    m_pResourceDescriptorCreator = KRG::New<ResourceDescriptorCreator>( m_pModel, pDescriptorTypeInfo->m_ID, path );
                }
            }

            ImGui::EndMenu();
        }
    }
}