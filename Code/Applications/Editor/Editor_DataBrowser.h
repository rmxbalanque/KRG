#pragma once

#include "Editor_DataBrowserTreeView.h"
#include "Editor_DataFileInspector.h"
#include "Editor_Model.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class DataBrowser
    {
    public:

        DataBrowser( EditorModel& model );
        ~DataBrowser();

        virtual char const * const GetName() { return "Data Browser"; }
        void Draw( UpdateContext const& context );

    private:

        void UpdateVisibility();
        void UpdateAndDrawBrowserFilters( UpdateContext const& context );
        void OnBrowserItemDoubleClicked( TreeViewItem* pItem );
        bool DrawResourceTypeFilterMenu();

    private:

        EditorModel&                                        m_model;
        char                                                        m_nameFilterBuffer[256];
        TVector<ResourceTypeID>                                     m_typeFilter;
        DataBrowserTreeView                                         m_dataBrowserTreeView;
        DataFileInspector                                           m_dataFileInspector;
        EventBindingID                                              m_onDoubleClickEventID;
        bool                                                        m_showRawFiles = false;
        float                                                       m_dataBrowserViewHeight = 300;
    };
}