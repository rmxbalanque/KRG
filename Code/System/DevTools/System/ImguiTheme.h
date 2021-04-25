#pragma once

#include "../_Module/API.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    struct KRG_SYSTEM_DEVTOOLS_API Theme
    {
        friend class ImguiSystem;

    public:

        static ImVec4 const s_backgroundColorLight;
        static ImVec4 const s_backgroundColorMedium;
        static ImVec4 const s_backgroundColorDark;
        static ImVec4 const s_panelColor;
        static ImVec4 const s_accentColor;
        static ImVec4 const s_accentColorDark;
        static ImVec4 const s_textColor;
        static ImVec4 const s_textColorDisabled;
        static ImVec4 const s_borderColor;

        constexpr static float const s_toolTipDelay = 0.4f;

    private:

        static void ApplyTheme();
    };
}