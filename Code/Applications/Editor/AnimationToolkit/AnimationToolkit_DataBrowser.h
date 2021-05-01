#pragma once
#include "Applications/Editor/Common/DataBrowser/DataBrowserTreeView.h"
#include "Applications/Editor/Common/PropertyGrid/PropertyGrid.h"
#include "Applications/Editor/Common/ResourceDescriptorCreator.h"
#include "Applications/Editor/Editor/EditorTool.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "Tools/Resource/RawAssets/RawAssetInfo.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class AnimationResourceDescriptorCreator final : public ResourceDescriptorCreator
    {

    public:

        AnimationResourceDescriptorCreator( EditorModel* pModel );

    protected:

        virtual void CreateNewDescriptor( FileSystem::Path const& rawFile ) const override;
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

        // Data browser
        //-------------------------------------------------------------------------

        void UpdateVisibility();
        void DrawBrowser( UpdateContext const& context );
        void OnBrowserItemDoubleClicked( TreeViewItem* pItem );

        // Info Panel
        //-------------------------------------------------------------------------

        bool OpenInspectedResourceFile( UpdateContext const& context );
        bool SaveInspectedResourceFile( UpdateContext const& context );

        void DrawInfoPanel( UpdateContext const& context );
        void DrawResourceFileInfo( UpdateContext const& context );
        void DrawRawFileInfo( UpdateContext const& context );

    private:

        // Browser
        char                                                m_filterBuffer[256];
        bool                                                m_showRawFiles = false;
        TDataBrowserTreeView<AnimationBrowserTreeItem>      m_dataBrowserTreeView;
        FileSystem::Path                                    m_inspectedFile;
        EventBindingID                                      m_onDoubleClickEventID;

        // Info
        AnimationResourceDescriptorCreator                  m_descriptorCreator;
        TypeSystem::PropertyGrid                            m_propertyGrid;
        TypeSystem::TypeInstanceModel                       m_typeInstance;
        RawAssets::RawAssetInfo                             m_assetInfo;
        bool                                                m_validAssetInfo;
    };
}