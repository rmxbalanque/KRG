#pragma once
#include "../_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::OrientationGuide
{
    KRG_ENGINE_CORE_API Float2 GetSize();
    KRG_ENGINE_CORE_API float GetWidth();
    KRG_ENGINE_CORE_API void Draw( Float2 const& guideOrigin, Render::Viewport const& viewport );
}