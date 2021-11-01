#pragma once
#include "../_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::OrientationGuide
{
    KRG_ENGINE_CORE_API Float2 GetRequiredDimensions();
    KRG_ENGINE_CORE_API void Draw( Render::Viewport const& viewport, ImVec2 const& offset = ImVec2( 3.0f, 4.0f ) );
    KRG_ENGINE_CORE_API void DrawAsChildWindow( Render::Viewport const& viewport );
}