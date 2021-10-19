#pragma once
#include "System/Render/_Module/API.h"
#include "System/Render/Imgui/ImguiTheme.h"
#include "imgui.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    KRG_SYSTEM_RENDER_API bool DrawSpinner( char const* pLabel, ImVec4 color = Theme::s_textColor, float radius = 6.0f, float thickness = 3.0f );
}
#endif