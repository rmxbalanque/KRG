#pragma once

#include "Applications/Editor/Editors/Editor.h"
#include "ResourceEditor_Model.h"
#include "ResourceEditor_DataBrowser.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceEditor final : public TEditor<ResourceEditorModel>
    {
    public:

        ~ResourceEditor();

    private:

        virtual char const* GetName() const override final { return "Kruger Mesh Editor"; }

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        DataBrowser*                        m_pDataBrowser = nullptr;
        float                               m_dataBrowserViewWidth = 150;
    };
}