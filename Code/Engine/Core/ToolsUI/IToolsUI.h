#pragma once
#include "Engine/Core/_Module/API.h"
#include "System/Render/RenderViewport.h"
#include "System/Resource/ResourceRequesterID.h"
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
    class KRG_ENGINE_CORE_API IToolsUI
    {
    public:

        virtual ~IToolsUI() = default;

        virtual void Initialize( UpdateContext const& context ) = 0;
        virtual void Shutdown( UpdateContext const& context ) = 0;

        // This is called at the absolute start of the frame before we update the resource system, start updating any entities, etc...
        // Any entity/world/map state changes need to be done via this update!
        virtual void StartFrame( UpdateContext const& context ) {}

        // Optional update run before we update the world at each stage
        virtual void Update( UpdateContext const& context ) {}

        // This is called at the absolute end of the frame just before we kick off rendering. It is generally NOT safe to modify any world/map/entity during this update!!!
        virtual void EndFrame( UpdateContext const& context ) {}

        // Hot Reload Support
        virtual void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload, TVector<ResourceID> const& resourcesToBeReloaded ) = 0;
        virtual void EndHotReload() = 0;
    };
}
#endif