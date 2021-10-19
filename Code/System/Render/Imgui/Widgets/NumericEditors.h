#pragma once
#include "System/Render/_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    KRG_SYSTEM_RENDER_API bool InputFloat2( Float2& value, float width = -1, bool readOnly = false );
    KRG_SYSTEM_RENDER_API bool InputFloat3( Float3& value, float width = -1, bool readOnly = false );
    KRG_SYSTEM_RENDER_API bool InputFloat4( Float4& value, float width = -1, bool readOnly = false );
}
#endif