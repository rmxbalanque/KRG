#pragma once

#include "ResourceEditor_Model.h"
#include "ResourceEditor_DataBrowser.h"
#include "Tools\Core\Editors\StandaloneEditor.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceEditor final : public TStandaloneEditor<ResourceEditorModel>
    {
    public:

        ~ResourceEditor();

    private:

        virtual char const* GetName() const override final { return "Kruger Resource Editor"; }

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

        bool DrawWorkspaceWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ResourceEditorWorkspace* pWorkspace );

    private:

        DataBrowser*                        m_pDataBrowser = nullptr;
        float                               m_dataBrowserViewWidth = 150;
    };
}