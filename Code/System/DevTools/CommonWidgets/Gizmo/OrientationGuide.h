#pragma once
#include "System/DevTools/_Module/API.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/DevTools/System/ImguiTheme.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::OrientationGuide
{
    KRG_SYSTEM_DEVTOOLS_API Float2 GetRequiredDimensions();
    KRG_SYSTEM_DEVTOOLS_API void DrawAsStandaloneWindow( Render::Viewport const& viewport, ImVec2 const& offset = ImVec2( 3.0f, 4.0f ) );
    KRG_SYSTEM_DEVTOOLS_API void DrawAsChildWindow( Render::Viewport const& viewport );
}