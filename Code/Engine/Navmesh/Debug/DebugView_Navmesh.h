#pragma once

#include "../_Module/API.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Core/DebugTools/DebugTools.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Navmesh
{
    class KRG_ENGINE_NAVMESH_API NavmeshDebugViewController : public Debug::DebugView
    {
    public:

        NavmeshDebugViewController();

        void Initialize();
        void Shutdown();

    private:

        virtual void DrawWindows( UpdateContext const& context ) override final;
        void DrawDebugMenu( UpdateContext const& context );

    private:


    };
}
#endif