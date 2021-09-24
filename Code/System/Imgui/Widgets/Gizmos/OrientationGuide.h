#pragma once
#include "System/Imgui/_Module/API.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"
#include "System/Imgui/System/ImguiTheme.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::OrientationGuide
{
    KRG_SYSTEM_IMGUI_API Float2 GetRequiredDimensions();
    KRG_SYSTEM_IMGUI_API void DrawAsStandaloneWindow( Render::Viewport const& viewport, ImVec2 const& offset = ImVec2( 3.0f, 4.0f ) );
    KRG_SYSTEM_IMGUI_API void DrawAsChildWindow( Render::Viewport const& viewport );
}