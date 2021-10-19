#pragma once

#include "../_Module/API.h"
#include "imgui.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    struct KRG_SYSTEM_RENDER_API Theme
    {
        friend class ImguiSystem;

    public:

        static ImVec4 const s_backgroundColorLight;
        static ImVec4 const s_backgroundColorSemiLight;
        static ImVec4 const s_backgroundColorMedium;
        static ImVec4 const s_backgroundColorSemiDark;
        static ImVec4 const s_backgroundColorDark;

        static ImVec4 const s_itemColorLight;
        static ImVec4 const s_itemColorSemiLight;
        static ImVec4 const s_itemColorMedium;
        static ImVec4 const s_itemColorSemiDark;
        static ImVec4 const s_itemColorDark;

        static ImVec4 const s_textColor;
        static ImVec4 const s_textColorDisabled;

        constexpr static float const s_toolTipDelay = 0.4f;

    public:

        static void ApplyTheme();
    };
}