#pragma once
#include "System/DevTools/_Module/API.h"
#include "System/DevTools/System/ImguiFont.h"
#include "System/DevTools/System/ImguiTheme.h"
#include "System/DevTools/ThirdParty/imgui/imgui.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    KRG_SYSTEM_DEVTOOLS_API bool InputFloat2( Float2& value, float width = -1, bool readOnly = false );
    KRG_SYSTEM_DEVTOOLS_API bool InputFloat3( Float3& value, float width = -1, bool readOnly = false );
    KRG_SYSTEM_DEVTOOLS_API bool InputFloat4( Float4& value, float width = -1, bool readOnly = false );
}
#endif