#pragma once
#include "_Module/API.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Core/Defines.h"
#include "System/Core/Types/Containers.h"

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
    class KRG_SYSTEM_DEVTOOLS_API DevelopmentToolset
    {
    public:

        virtual ~DevelopmentToolset() = default;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) = 0;
        virtual void Shutdown( UpdateContext const& context ) = 0;

        // The development tools will be updated at the start and the end of the frame, before and after all the world updates so it will be safe to modify entity state.
        virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportSystem ) = 0;
    };
}
#endif