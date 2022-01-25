#pragma once

#include "../_Module/API.h"
#include "imgui.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    struct KRG_SYSTEM_RENDER_API Style
    {
        friend class ImguiSystem;

    public:

        // Core imgui colors
        static ImColor const s_backgroundColorLight;
        static ImColor const s_backgroundColorSemiLight;
        static ImColor const s_backgroundColorMedium;
        static ImColor const s_backgroundColorSemiDark;
        static ImColor const s_backgroundColorDark;

        static ImColor const s_itemColorLight;
        static ImColor const s_itemColorSemiLight;
        static ImColor const s_itemColorMedium;
        static ImColor const s_itemColorSemiDark;
        static ImColor const s_itemColorDark;

        static ImColor const s_textColor;
        static ImColor const s_textColorDisabled;

        // Accents
        static ImColor const s_selectionAccent;
        static ImColor const s_selectionAccentAlt;

        // Additional tool colors
        static ImColor const s_gridBackgroundColor;
        static ImColor const s_gridLineColor;
        static ImColor const s_selectionBoxOutlineColor;
        static ImColor const s_selectionBoxFillColor;

        constexpr static float const s_toolTipDelay = 0.4f;

    public:

        static void Apply();
    };
}