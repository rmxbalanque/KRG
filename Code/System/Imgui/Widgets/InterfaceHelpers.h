#pragma once
#include "System/Imgui/_Module/API.h"
#include "System/Imgui/ImguiTheme.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    KRG_SYSTEM_IMGUI_API bool DrawSpinner( char const* pLabel, ImVec4 color = Theme::s_textColor, float radius = 6.0f, float thickness = 3.0f );
}
#endif