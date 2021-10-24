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
        friend class NavmeshDebugView;

    public:

        KRG_SYSTEM_ID( NavmeshSystem );

    public:

        NavmeshSystem() = default;

        void Initialize();
        void Shutdown();

        void Update( UpdateContext const& ctx );
     
    private:

        #if KRG_ENABLE_NAVPOWER
        NavPowerAllocator                               m_allocator;
        #endif
    };
}