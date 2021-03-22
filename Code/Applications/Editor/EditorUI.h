#pragma once
#include "System/Imgui/DevelopmentUI.h"
#include "System/Imgui/ImguiX.h"
#include "System/Core/Math/Rectangle.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class EditorUI final : public ImGuiX::DevelopmentUI
    {
    public:

        virtual void Initialize( SettingsRegistry const& settingsRegistry ) override final;
        virtual void Shutdown() override final;
        virtual void Update( UpdateContext const& context, Render::ViewportSystem& viewportSystem ) override final;

    private:

        void DrawDockSpace();

    private:

        ImGuiID                 m_mainDockspaceID;
        Math::Rectangle         m_viewportRect;
    };
}
#endif