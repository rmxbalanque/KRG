#pragma once
#include "Tools/Core/DataFileTools/DataBrowserTreeView.h"
#include "Tools/Core/DataFileTools/DataFileInspector.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Editor/EditorTool.h"
#include "Tools/Core/Resource/RawAssets/RawAssetInfo.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationDataFileInspector final : public DataFileInspector
    {
    public:

        using DataFileInspector::DataFileInspector;

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
        class AnimationBrowserTreeItem final : public DataBrowserTreeItem
        {
        public:

            using DataBrowserTreeItem::DataBrowserTreeItem;

        private:

            virtual bool CanBeSetActive() const override { return false; }
            virtual void DrawControls() override;
        };

    public:

        DataBrowser( EditorModel* pModel );
        ~DataBrowser();

        virtual char const* const GetName() { return "Data Browser"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void UpdateVisibility();
        void DrawBrowser( UpdateContext const& context );
        void OnBrowserItemDoubleClicked( TreeViewItem* pItem );

    private:

        char                                                    m_filterBuffer[256];
        bool                                                    m_showRawFiles = false;
        TDataBrowserTreeView<AnimationBrowserTreeItem>          m_dataBrowserTreeView;
        AnimationDataFileInspector                              m_dataFileInspector;
        EventBindingID                                          m_onDoubleClickEventID;
    };
}