#pragma once
#include "System/DevTools/_Module/API.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/DevTools/System/ImguiTheme.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    KRG_SYSTEM_DEVTOOLS_API bool DrawSpinner( char const* pLabel, ImVec4 color = Theme::s_textColor, float radius = 6.0f, float thickness = 3.0f );
}
#endif