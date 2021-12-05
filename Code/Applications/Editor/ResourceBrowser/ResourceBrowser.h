#pragma once

#include "ResourceBrowser_TreeView.h"
#include "Applications/Editor/Editor_Model.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource{ class RawResourceInspector; }

    //-------------------------------------------------------------------------

    class ResourceBrowser
    {
    public:

        ResourceBrowser( EditorModel& model );
        ~ResourceBrowser();

        virtual char const * const GetName() { return "Resource Browser"; }
        bool Draw( UpdateContext const& context );

        void RebuildBrowserTree();

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

        ResourceBrowserTreeView                             m_treeView;
        Resource::RawResourceInspector*                     m_pRawResourceInspector = nullptr;
        EventBindingID                                      m_onDoubleClickEventID;
    };
}