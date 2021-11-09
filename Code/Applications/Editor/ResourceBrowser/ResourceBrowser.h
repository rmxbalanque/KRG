#pragma once

#include "ResourceBrowser_TreeView.h"
#include "ResourceBrowser_ResourceInspector.h"
#include "Applications/Editor/Editor_Model.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceBrowser
    {
    public:

        ResourceBrowser( EditorModel& model );
        ~ResourceBrowser();

        virtual char const * const GetName() { return "Data Browser"; }
        bool Draw( UpdateContext const& context );

        inline void RebuildBrowserTree() { m_dataBrowserTreeView.RebuildBrowserTree(); }

    private:

        void UpdateVisibility();
        void UpdateAndDrawBrowserFilters( UpdateContext const& context );
        void OnBrowserItemDoubleClicked( TreeViewItem* pItem );
        bool DrawResourceTypeFilterMenu();

    private:

        EditorModel&                                        m_model;
        char                                                m_nameFilterBuffer[256];
        TVector<ResourceTypeID>                             m_typeFilter;
        bool                                                m_showRawFiles = false;
        float                                               m_dataBrowserViewHeight = 300;

        ResourceBrowserTreeView                             m_dataBrowserTreeView;
        ResourceInspector                                   m_dataFileInspector;
        EventBindingID                                      m_onDoubleClickEventID;
    };
}