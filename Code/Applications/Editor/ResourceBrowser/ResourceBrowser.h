#pragma once
#include "Applications/Editor/EditorContext.h"
#include "Tools/Core/Widgets/TreeListView.h"
#include "Tools/Core/FileSystem/FileSystemWatcher.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceDescriptorCreator;
    namespace Resource{ class RawResourceInspector; }

    //-------------------------------------------------------------------------

    class ResourceBrowser final: public TreeListView, public FileSystem::IFileSystemChangeListener
    {
    public:

        ResourceBrowser( EditorContext& model );
        ~ResourceBrowser();

        char const* const GetWindowName() { return "Resource Browser"; }
        bool Draw( UpdateContext const& context );

        void RebuildBrowserTree() { RebuildTree(); }

    private:

        virtual void RebuildTreeInternal() override;
        virtual void DrawItemContextMenu( TVector<TreeListViewItem*> const& selectedItemsWithContextMenus ) override;
        virtual void DrawAdditionalUI() override;

        void OnBrowserItemDoubleClicked( TreeListViewItem* pItem );

        TreeListViewItem& FindOrCreateParentForItem( FileSystem::Path const& path );

        void UpdateVisibility();
        void DrawCreateNewDescriptorMenu( FileSystem::Path const& path );
        void DrawFilterOptions( UpdateContext const& context );
        bool DrawResourceTypeFilterMenu();

    private:

        EditorContext&                                      m_editorContext;
        char                                                m_nameFilterBuffer[256];
        TVector<ResourceTypeID>                             m_typeFilter;
        bool                                                m_showRawFiles = false;
        bool                                                m_showDeleteConfirmationDialog = false;

        int32                                               m_dataDirectoryPathDepth;
        TVector<FileSystem::Path>                           m_foundPaths;

        ResourceDescriptorCreator*                          m_pResourceDescriptorCreator = nullptr;
        Resource::RawResourceInspector*                     m_pRawResourceInspector = nullptr;
        EventBindingID                                      m_onDoubleClickEventID;
    };
}