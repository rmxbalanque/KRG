#pragma once
#include "System/DevTools/_Module/API.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/DevTools/System/ImguiTheme.h"

//-------------------------------------------------------------------------

namespace KRG::Math { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    KRG_SYSTEM_DEVTOOLS_API void DrawOrientationGuide( Math::Viewport const& viewport, ImVec2 const& margin = ImVec2( 3.0f, 4.0f ) );

    KRG_SYSTEM_DEVTOOLS_API bool DrawSpinner( char const* pLabel, ImVec4 color = Theme::s_accentColor, float radius = 6.0f, float thickness = 3.0f );

    KRG_SYSTEM_DEVTOOLS_API bool DrawProgressBar( char const* pLabel, float value, ImVec2 size = ImVec2(-1, 10), ImVec4 backgroundColor = Theme::s_backgroundColorMedium, ImVec4 foregroundColor = Theme::s_accentColor );
}