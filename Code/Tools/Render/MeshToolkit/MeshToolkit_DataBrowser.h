#pragma once
#include "Tools/Editor/Widgets/Datafiles/DataBrowserTreeView.h"
#include "Tools/Editor/Widgets/Datafiles/DataFileInspector.h"
#include "Tools/Editor/Widgets/PropertyGrid/PropertyGrid.h"
#include "Tools/Editor/EditorTool.h"
#include "Tools/Resource/RawAssets/RawAssetInfo.h"

//-------------------------------------------------------------------------

namespace KRG::Render::Tools
{
    class MeshDataFileInspector final : public Editor::DataFileInspector
    {

    public:

        using Editor::DataFileInspector::DataFileInspector;

    private:

        virtual void DrawRawFileInfo() override;
        virtual void OnStartInspectingRawFile() override;
        virtual void OnStopInspectingRawFile() override;

    private:

        RawAssets::RawAssetInfo                             m_assetInfo;
        bool                                                m_validAssetInfo = false;
    };

    //-------------------------------------------------------------------------

    class DataBrowser final : public TEditorTool<EditorModel>
    {
        class MeshBrowserTreeItem final : public Editor::DataBrowserTreeItem
        {
        public:

            using Editor::DataBrowserTreeItem::DataBrowserTreeItem;

        private:

            virtual bool CanBeSetActive() const override { return false; }
            virtual void DrawControls() override;
        };

    public:

        DataBrowser( EditorModel* pModel );
        ~DataBrowser();

        virtual char const * const GetName() { return "Data Browser"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void UpdateVisibility();
        void DrawBrowser( UpdateContext const& context );
        void OnBrowserItemDoubleClicked( Editor::TreeViewItem* pItem );

    private:

        char                                                        m_filterBuffer[256];
        bool                                                        m_showStaticMeshes = true;
        bool                                                        m_showSkeletalMeshes = true;
        bool                                                        m_showRawFiles = false;
        Editor::TDataBrowserTreeView<MeshBrowserTreeItem>           m_dataBrowserTreeView;
        MeshDataFileInspector                                       m_dataFileInspector;
        EventBindingID                                              m_onDoubleClickEventID;
    };
}