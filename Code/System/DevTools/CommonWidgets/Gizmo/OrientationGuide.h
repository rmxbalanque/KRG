#pragma once
#include "System/DevTools/_Module/API.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/DevTools/System/ImguiTheme.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    KRG_SYSTEM_DEVTOOLS_API void DrawOrientationGuide( Render::Viewport const& viewport, ImVec2 const& margin = ImVec2( 3.0f, 4.0f ) );
}