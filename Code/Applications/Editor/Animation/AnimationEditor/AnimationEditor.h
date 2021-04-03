#pragma once
#include "Applications/Editor/Editor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationEditor final : public Editor
    {
        virtual char const* GetName() const override final { return "Kruger Animation Editor"; }

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown() override;

        virtual void DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}