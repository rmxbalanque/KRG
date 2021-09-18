#pragma once

#include "ResourceEditor_DataBrowserTreeView.h"
#include "ResourceEditor_DataFileInspector.h"
#include "ResourceEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class DataBrowser
    {
    public:

        DataBrowser( ResourceEditorModel& model );
        ~DataBrowser();

        virtual char const * const GetName() { return "Data Browser"; }
        void Draw( UpdateContext const& context );

    private:

        void UpdateVisibility();
        void UpdateAndDrawBrowserFilters( UpdateContext const& context );
        void OnBrowserItemDoubleClicked( TreeViewItem* pItem );

    private:

        ResourceEditorModel&                                        m_model;
        char                                                        m_filterBuffer[256];
        DataBrowserTreeView                                         m_dataBrowserTreeView;
        DataFileInspector                                           m_dataFileInspector;
        EventBindingID                                              m_onDoubleClickEventID;
        bool                                                        m_showRawFiles = false;

        bool                                                        m_showStaticMeshes = true;
        bool                                                        m_showSkeletalMeshes = true;
        float                                                       m_dataBrowserViewHeight = 300;
    };
}