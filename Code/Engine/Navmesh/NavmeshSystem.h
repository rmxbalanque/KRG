#pragma once

#include "_Module/API.h"
#include "Engine/Navmesh/NavPower.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class KRG_ENGINE_NAVMESH_API NavmeshSystem : public ISystem
    {
        friend class NavmeshDebugViewController;

    public:

        KRG_SYSTEM_ID( NavmeshSystem );

    public:

        NavmeshSystem() = default;

        void Initialize( Debug::DrawingSystem* pDebugDrawingSystem );
        void Shutdown();

        void Update( UpdateContext const& ctx );
     
    private:

        #if WITH_BFX
        NavPowerAllocator                               m_allocator;

        #if KRG_DEVELOPMENT_TOOLS
        NavPowerRenderer                                m_debugRenderer;
        #endif
        #endif
    };
}