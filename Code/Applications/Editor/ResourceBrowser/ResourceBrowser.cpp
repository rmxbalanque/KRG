#include "ResourceBrowser.h"
#include "RawResourceInspectors/RawResourceInspector.h"
#include "ResourceBrowser_DescriptorCreator.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceBrowserTreeItem final : public TreeListViewItem
    {
    public:

        enum class Type
        {
            Directory = 0,
            File,
        };

    public:

        ResourceBrowserTreeItem( char const* pName, int32 hierarchyLevel, FileSystem::Path const& path, ResourcePath const& resourcePath, ResourceTypeID resourceTypeID = ResourceTypeID() )
            : TreeListViewItem( pName, hierarchyLevel )
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
        }

        virtual uint32 GetUniqueID() const override { return m_resourcePath.GetID(); }
        virtual bool HasContextMenu() const override { return true; }
        virtual bool IsActivatable() const override { return false; }

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
}

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceBrowser::ResourceBrowser( EditorModel& model )
        : m_model( model )
        , m_dataDirectoryPathDepth( m_model.GetRawResourceDirectory().GetPathDepth() )
    {
        Memory::MemsetZero( m_nameFilterBuffer, 256 * sizeof( char ) );
        m_onDoubleClickEventID = OnItemDoubleClicked().Bind( [this] ( TreeListViewItem* pItem ) { OnBrowserItemDoubleClicked( pItem ); } );

        // Create root node
        KRG_ASSERT( m_pRoot == nullptr );
        m_pRoot = KRG::New<ResourceBrowserTreeItem>( "Data", -1, m_model.GetRawResourceDirectory(), ResourcePath::FromFileSystemPath( m_model.GetRawResourceDirectory(), m_model.GetRawResourceDirectory() ) );
        m_pRoot->SetExpanded( true );

        // Refresh visual state
        RebuildBrowserTree();
        UpdateVisibility();
    }

    ResourceBrowser::~ResourceBrowser()
    {
        OnItemDoubleClicked().Unbind( m_onDoubleClickEventID );

        KRG::Delete( m_pResourceDescriptorCreator );
        KRG::Delete( m_pRawResourceInspector );
    }

    //-------------------------------------------------------------------------

    bool ResourceBrowser::Draw( UpdateContext const& context )
    {
        bool isOpen = true;
        if ( ImGui::Begin( GetWindowName(), &isOpen) )
        {
            DrawFilterOptions( context );
            TreeListView::Draw();
        }
        ImGui::End();

        //-------------------------------------------------------------------------

        if ( m_pResourceDescriptorCreator != nullptr )
        {
            if ( !m_pResourceDescriptorCreator->Draw() )
            {
                KRG::Delete( m_pResourceDescriptorCreator );
            }
        }

        if ( m_pRawResourceInspector != nullptr )
        {
            if ( !m_pRawResourceInspector->DrawDialog() )
            {
                KRG::Delete( m_pRawResourceInspector );
            }
        }

        //-------------------------------------------------------------------------

        return isOpen;
    }

    void ResourceBrowser::RebuildBrowserTree()
    {
        KRG_ASSERT( m_pRoot != nullptr );

         // Record current state
         //-------------------------------------------------------------------------

        TVector<uint32> originalExpandedItems;
        originalExpandedItems.reserve( GetNumItems() );

        auto RecordItemState = [&originalExpandedItems] ( TreeListViewItem const* pItem )
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

        if ( !FileSystem::GetDirectoryContents( m_model.GetRawResourceDirectory(), m_foundPaths, FileSystem::DirectoryReaderOutput::OnlyFiles, FileSystem::DirectoryReaderMode::Expand) )
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
                if ( !m_model.GetTypeRegistry()->IsRegisteredResourceType( resourceTypeID ) )
                {
                    resourceTypeID = ResourceTypeID();
                }
            }

            // Create file item
            parentItem.CreateChild<ResourceBrowserTreeItem>( path.GetFileName().c_str(), parentItem.GetHierarchyLevel() + 1, path, ResourcePath::FromFileSystemPath( m_model.GetRawResourceDirectory(), path ), resourceTypeID );
        }

        // Restore original state
        //-------------------------------------------------------------------------

        auto RestoreItemState = [&originalExpandedItems] ( TreeListViewItem* pItem )
        {
            if ( VectorContains( originalExpandedItems, pItem->GetUniqueID() ) )
            {
                pItem->SetExpanded( true );
            }
        };

        ForEachItem( RestoreItemState );

        auto FindActiveItem = [activeItemID] ( TreeListViewItem const* pItem ) { return pItem->GetUniqueID() == activeItemID; };
        m_pActiveItem = m_pRoot->SearchChildren( FindActiveItem );

        auto FindSelectedItem = [selectedItemID] ( TreeListViewItem const* pItem ) { return pItem->GetUniqueID() == selectedItemID; };
        m_pSelectedItem = m_pRoot->SearchChildren( FindSelectedItem );

        UpdateVisibility();
    }

    //-------------------------------------------------------------------------

    void ResourceBrowser::UpdateVisibility()
    {
        auto VisibilityFunc = [this] ( TreeListViewItem const* pItem )
        {
            bool isVisible = true;

            // Type filter
            //-------------------------------------------------------------------------

            auto pDataFileItem = static_cast<ResourceBrowserTreeItem const*>( pItem );
            if ( pDataFileItem->IsFile() )
            {
                if ( pDataFileItem->IsRawFile() )
                {
                    isVisible = m_showRawFiles;
                }
                else // Resource file
                {
                    auto const& resourceTypeID = pDataFileItem->GetResourceTypeID();
                    isVisible = m_typeFilter.empty() || VectorContains( m_typeFilter, resourceTypeID );
                }
            }

            // Text filter
            //-------------------------------------------------------------------------

            if ( isVisible && m_nameFilterBuffer[0] != 0 )
            {
                String lowercaseLabel = pItem->GetDisplayName();
                lowercaseLabel.make_lower();

                char tempBuffer[256];
                strcpy( tempBuffer, m_nameFilterBuffer );

                char* token = strtok( tempBuffer, " " );
                while ( token )
                {
                    if ( lowercaseLabel.find( token ) == String::npos )
                    {
                        isVisible = false;
                        break;
                    }

                    token = strtok( NULL, " " );
                }
            }

            //-------------------------------------------------------------------------

            return isVisible;
        };

        //-------------------------------------------------------------------------

        UpdateItemVisibility( VisibilityFunc );
    }

    void ResourceBrowser::DrawFilterOptions( UpdateContext const& context )
    {
        KRG_PROFILE_FUNCTION();

        constexpr static float const buttonWidth = 22;
        bool shouldUpdateVisibility = false;

        // Text Filter
        //-------------------------------------------------------------------------

        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - buttonWidth - itemSpacing );
        if ( ImGui::InputText( "##Filter", m_nameFilterBuffer, 256 ) )
        {
            // Convert buffer to lower case
            int32 i = 0;
            while ( i < 256 && m_nameFilterBuffer[i] != 0 )
            {
                m_nameFilterBuffer[i] = eastl::CharToLower( m_nameFilterBuffer[i] );
                i++;
            }

            shouldUpdateVisibility = true;

            auto const SetExpansion = []( TreeListViewItem* pItem )
            {
                if ( pItem->IsVisible() )
                {
                    pItem->SetExpanded( true );
                }
            };

            ForEachItem( SetExpansion );
        }

        ImGui::SameLine();
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear Filter", ImVec2( buttonWidth, 0 ) ) )
        {
            m_nameFilterBuffer[0] = 0;
            shouldUpdateVisibility = true;
        }

        // Type Filter + Controls
        //-------------------------------------------------------------------------

        shouldUpdateVisibility |= DrawResourceTypeFilterMenu();

        ImGui::SameLine();
        if ( ImGui::Checkbox( "Raw Files", &m_showRawFiles ) )
        {
            shouldUpdateVisibility = true;
        }

        ImGui::SameLine( ImGui::GetContentRegionAvail().x - ( buttonWidth * 2 ) );
        if ( ImGui::Button( KRG_ICON_PLUS "##Expand All", ImVec2( buttonWidth, 0 ) ) )
        {
            ForEachItem( [] ( TreeListViewItem* pItem ) { pItem->SetExpanded( true ); } );
        }

        ImGui::SameLine();
        if ( ImGui::Button( KRG_ICON_MINUS "##Collapse ALL", ImVec2( buttonWidth, 0 ) ) )
        {
            ForEachItem( [] ( TreeListViewItem* pItem ) { pItem->SetExpanded( false ); } );
        }

        //-------------------------------------------------------------------------

        if ( shouldUpdateVisibility )
        {
            UpdateVisibility();
        }
    }

    bool ResourceBrowser::DrawResourceTypeFilterMenu()
    {
        bool requiresVisibilityUpdate = false;

        ImGui::SetNextItemWidth( 150 );
        if ( ImGui::BeginCombo( "##ResourceTypeFilters", "Resource Filters", ImGuiComboFlags_HeightLarge ) )
        {
            for ( auto const& resourceInfo : m_model.GetTypeRegistry()->GetRegisteredResourceTypes() )
            {
                bool isChecked = VectorContains( m_typeFilter, resourceInfo.second.m_resourceTypeID );
                if ( ImGui::Checkbox( resourceInfo.second.m_friendlyName.c_str(), &isChecked ) )
                {
                    if ( isChecked )
                    {
                        m_typeFilter.emplace_back( resourceInfo.second.m_resourceTypeID );
                    }
                    else
                    {
                        m_typeFilter.erase_first_unsorted( resourceInfo.second.m_resourceTypeID );
                    }

                    requiresVisibilityUpdate = true;
                }
            }

            ImGui::EndCombo();
        }

        //-------------------------------------------------------------------------

        return requiresVisibilityUpdate;
    }

    //-------------------------------------------------------------------------

    TreeListViewItem& ResourceBrowser::FindOrCreateParentForItem( FileSystem::Path const& path )
    {
        KRG_ASSERT( path.IsFile() );

        TreeListViewItem* pCurrentItem = m_pRoot;
        FileSystem::Path directoryPath = m_model.GetRawResourceDirectory();
        TInlineVector<String, 10> splitPath = path.Split();

        //-------------------------------------------------------------------------

        int32 const pathDepth = (int32) splitPath.size();
        for ( int32 i = m_dataDirectoryPathDepth + 1; i < pathDepth; i++ )
        {
            directoryPath.Append( splitPath[i] );

            StringID const ID( splitPath[i] );
            auto searchPredicate = [&ID] ( TreeListViewItem const* pItem ) { return pItem->GetNameID() == ID; };

            auto pFoundChildItem = pCurrentItem->FindChild( searchPredicate );
            if ( pFoundChildItem == nullptr )
            {
                auto pItem = pCurrentItem->CreateChild<ResourceBrowserTreeItem>( splitPath[i].c_str(), pCurrentItem->GetHierarchyLevel() + 1, directoryPath, ResourcePath::FromFileSystemPath( m_model.GetRawResourceDirectory(), directoryPath ) );
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

    void ResourceBrowser::DrawItemContextMenu( TreeListViewItem* pItem )
    {
        auto pResourceItem = (ResourceBrowserTreeItem*) pItem;

        //-------------------------------------------------------------------------

        if ( ImGui::MenuItem( "Open In Explorer" ) )
        {
            FileSystem::OpenInExplorer( pResourceItem->GetFilePath() );
        }

        if ( ImGui::MenuItem( "Copy File Path" ) )
        {
            ImGui::SetClipboardText( pResourceItem->GetFilePath().c_str() );
        }

        if ( ImGui::MenuItem( "Copy Resource Path" ) )
        {
            ImGui::SetClipboardText( pResourceItem->GetResourcePath().c_str() );
        }

        // Directory options
        //-------------------------------------------------------------------------

        if ( pResourceItem->GetFilePath().IsDirectory() )
        {
            ImGui::Separator();

            DrawCreateNewDescriptorMenu( pResourceItem->GetFilePath() );
        }

        // File options
        //-------------------------------------------------------------------------

        if ( pResourceItem->GetFilePath().IsFile() )
        {
            ImGui::Separator();

            if ( ImGui::MenuItem( KRG_ICON_EXCLAMATION_TRIANGLE" Delete" ) )
            {
                ImGui::OpenPopup( "Del" );
            }
        }

        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupModal( "Del" ) )
        {
            if ( ImGui::Button( "Ok", ImVec2( 120, 0 ) ) )
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if ( ImGui::Button( "Cancel", ImVec2( 120, 0 ) ) )
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();

            ImGui::EndPopup();
        }
    }

    void ResourceBrowser::DrawCreateNewDescriptorMenu( FileSystem::Path const& path )
    {
        KRG_ASSERT( path.IsDirectory() );

        TypeSystem::TypeRegistry const* pTypeRegistry = m_model.GetTypeRegistry();
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
                    m_pResourceDescriptorCreator = KRG::New<ResourceDescriptorCreator>( &m_model, pDescriptorTypeInfo->m_ID, path );
                }
            }

            ImGui::EndMenu();
        }
    }

    void ResourceBrowser::OnBrowserItemDoubleClicked( TreeListViewItem* pItem )
    {
        auto pResourceFileItem = static_cast<ResourceBrowserTreeItem const*>( pItem );
        if ( pResourceFileItem->IsDirectory() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( pResourceFileItem->IsResourceFile() )
        {
            m_model.QueueCreateWorkspace( pResourceFileItem->GetResourceID() );
        }
        else // Try create file inspector
        {
            if ( Resource::RawResourceInspectorFactory::CanCreateInspector( pResourceFileItem->GetFilePath() ) )
            {
                m_pRawResourceInspector = Resource::RawResourceInspectorFactory::TryCreateInspector( *m_model.GetTypeRegistry(), *m_model.GetResourceDatabase(), pResourceFileItem->GetFilePath() );
            }
        }
    }
}