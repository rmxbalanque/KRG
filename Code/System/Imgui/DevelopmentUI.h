#pragma once
#include "_Module/API.h"
#include "System/Core/Core/Defines.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Viewport.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class SettingsRegistry;
    class UpdateContext;
    namespace Render { class ViewportSystem; }
}

//-------------------------------------------------------------------------
// Base class for all development UI
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    class KRG_SYSTEM_IMGUI_API DevelopmentUI
    {
    public:

        virtual ~DevelopmentUI() = default;

        virtual void Initialize( SettingsRegistry const& settingsRegistry ) = 0;
        virtual void Shutdown() = 0;
        virtual void Update( UpdateContext const& context, Render::ViewportSystem& viewportSystem ) = 0;
    };
}
#endif