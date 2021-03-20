#pragma once

#include "../_Module/API.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    struct KRG_SYSTEM_IMGUI_API Theme
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

    private:

        static void ApplyTheme();
    };
}