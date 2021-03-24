#pragma once
#include "System/DevTools/DevelopmentTools.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class EditorTools final : public ImGuiX::DevelopmentTools
    {
    public:

        virtual void Initialize( SettingsRegistry const& settingsRegistry ) override final;
        virtual void Shutdown() override final;
        virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportSystem ) override final;

    private:

        void DrawMainMenu( UpdateContext const& context );
        void DrawDockSpaceAndViewport( UpdateContext const& context, Render::ViewportManager& viewportSystem );

    private:

        ImGuiID                 m_mainDockspaceID;
        bool                    m_mouseWithinEditorViewport = false;
    };
}
#endif