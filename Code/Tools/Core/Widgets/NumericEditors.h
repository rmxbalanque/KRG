#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    KRG_TOOLS_CORE_API bool InputFloat2( char const* pID, Float2& value, float width = -1, bool readOnly = false );
    KRG_TOOLS_CORE_API bool InputFloat3( char const* pID, Float3& value, float width = -1, bool readOnly = false );
    KRG_TOOLS_CORE_API bool InputFloat4( char const* pID, Float4& value, float width = -1, bool readOnly = false );
    KRG_TOOLS_CORE_API bool InputFloat4( char const* pID, Vector& value, float width = -1, bool readOnly = false );
}
#endif