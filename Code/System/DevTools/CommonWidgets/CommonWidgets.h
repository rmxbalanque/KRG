#pragma once
#include "System/DevTools/_Module/API.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"

//-------------------------------------------------------------------------

namespace KRG::Math { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    KRG_SYSTEM_DEVTOOLS_API void DrawOrientationGuide( Math::Viewport const& viewport, ImVec2 const& margin = ImVec2( 3.0f, 4.0f ) );
}