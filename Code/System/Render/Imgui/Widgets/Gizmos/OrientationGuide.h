#pragma once
#include "System/Render/_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::OrientationGuide
{
    KRG_SYSTEM_RENDER_API Float2 GetRequiredDimensions();
    KRG_SYSTEM_RENDER_API void DrawAsStandaloneWindow( Render::Viewport const& viewport, ImVec2 const& offset = ImVec2( 3.0f, 4.0f ) );
    KRG_SYSTEM_RENDER_API void DrawAsChildWindow( Render::Viewport const& viewport );
}