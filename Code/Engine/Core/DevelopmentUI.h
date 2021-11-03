#pragma once
#include "_Module/API.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;
}

//-------------------------------------------------------------------------
// Development Tools Framework
//-------------------------------------------------------------------------
// Base class for any runtime/editor development UI tools

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    class KRG_ENGINE_CORE_API DevelopmentUI
    {
    public:

        virtual ~DevelopmentUI() = default;

        virtual void Initialize( UpdateContext const& context ) = 0;
        virtual void Shutdown( UpdateContext const& context ) = 0;

        // The development tools will be updated at the start and the end of the frame, before and after all the world updates so it will be safe to modify entity state.
        virtual void Update( UpdateContext const& context ) = 0;

        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) = 0;
        virtual void EndHotReload() = 0;
    };
}
#endif