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
    namespace Render { class ViewportManager; }
}

//-------------------------------------------------------------------------
// Development Tools Framework
//-------------------------------------------------------------------------
// Base class for any runtime/editor development toolkits

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    class KRG_SYSTEM_DEVTOOLS_API DevelopmentTools
    {
    public:

        virtual ~DevelopmentTools() = default;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) = 0;
        virtual void Shutdown() = 0;

        // The development tools will be updated at the start and the end of the frame, before and after all the world updates so it will be safe to modify entity state.
        virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportSystem ) = 0;
    };
}
#endif