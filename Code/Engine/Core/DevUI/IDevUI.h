#pragma once
#include "Engine/Core/_Module/API.h"
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
    class KRG_ENGINE_CORE_API IDevUI
    {
    public:

        virtual ~IDevUI() = default;

        virtual void Initialize( UpdateContext const& context ) = 0;
        virtual void Shutdown( UpdateContext const& context ) = 0;

        // This is called at the start of the frame before we start updating any entities. Any entity/world/map state changes need to be done via this update!
        virtual void StartFrame( UpdateContext const& context ) {}

        // This is called after all entity updates and just before we kick off rendering. It is generally NOT safe to modify any world/map during this update!
        virtual void EndFrame( UpdateContext const& context ) {}

        // Hot Reload Support
        virtual void BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded ) = 0;
        virtual void EndHotReload() = 0;
    };
}
#endif