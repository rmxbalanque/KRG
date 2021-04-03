#pragma once
#include "Applications/Editor/Editor.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class MeshEditor final : public Editor
    {
        virtual char const* GetName() const override final { return "Kruger Mesh Editor"; }

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown() override;

        virtual void DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

        void DrawMeshBrowserWindow();
        void DrawMeshInfoWindow();
        void DrawDataFileInfoWindow();

    private:

        DataDirectoryModel                  m_dataDirectoryModel;

        bool                                m_showDataBrowser = false;
        bool                                m_showMeshInfoWindow = false;
        bool                                m_showSkeletalMeshInfoWindow = false;

        bool                                m_showStaticMeshes = true;
        bool                                m_showSkeletalMeshes = true;
        bool                                m_showRawFiles = true;
    };
}