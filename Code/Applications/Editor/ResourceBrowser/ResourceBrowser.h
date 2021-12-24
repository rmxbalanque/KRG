#pragma once
#include "Applications/Editor/Editor_Model.h"
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

        ResourceBrowser( EditorModel& model );
        ~ResourceBrowser();

        char const* const GetWindowName() { return "Resource Browser"; }
        bool Draw( UpdateContext const& context );

        void RebuildBrowserTree();

    private:

        void OnBrowserItemDoubleClicked( TreeListViewItem* pItem );

        TreeListViewItem& FindOrCreateParentForItem( FileSystem::Path const& path );

        void UpdateVisibility();

        virtual void DrawItemContextMenu( TreeListViewItem* pItem ) override;
        void DrawCreateNewDescriptorMenu( FileSystem::Path const& path );

        void DrawFilterOptions( UpdateContext const& context );
        bool DrawResourceTypeFilterMenu();

    private:

        EditorModel&                                        m_model;
        char                                                m_nameFilterBuffer[256];
        TVector<ResourceTypeID>                             m_typeFilter;
        bool                                                m_showRawFiles = false;

        int32                                               m_dataDirectoryPathDepth;
        TVector<FileSystem::Path>                           m_foundPaths;

        ResourceDescriptorCreator*                          m_pResourceDescriptorCreator = nullptr;
        Resource::RawResourceInspector*                     m_pRawResourceInspector = nullptr;
        EventBindingID                                      m_onDoubleClickEventID;
    };
}